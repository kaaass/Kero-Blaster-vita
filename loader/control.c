/* control.c -- Control stubs, event queue and control thread
 *
 * Copyright (C) 2022 KAAAsS, Andy Nguyen
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <assert.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/threadmgr.h>
#include <string.h>
#include "control.h"
#include "config.h"
#include "main.h"
#include "opengl.h"
#include "game_info.h"

#define DEAD_ZONE 20

enum {
    AKEYCODE_DPAD_UP = 19,
    AKEYCODE_DPAD_DOWN = 20,
    AKEYCODE_DPAD_LEFT = 21,
    AKEYCODE_DPAD_RIGHT = 22,
    AKEYCODE_A = 29,
    AKEYCODE_S = 47,
    AKEYCODE_X = 52,
    AKEYCODE_Z = 54,
    AKEYCODE_ESCAPE = 111,
    AKEYCODE_F1 = 131,
    AKEYCODE_F2 = 132,
    AKEYCODE_F3 = 133,
};

enum {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_RX = 11,
    AXIS_RY = 14,
};

typedef struct {
    uint32_t sce_button;
    int32_t android_button;
} button_mapping_t;

static button_mapping_t mapping[] = {
        {SCE_CTRL_CROSS,    AKEYCODE_X},
        {SCE_CTRL_CIRCLE,   AKEYCODE_Z},
        {SCE_CTRL_SQUARE,   AKEYCODE_S},
        {SCE_CTRL_TRIANGLE, AKEYCODE_A},
        {SCE_CTRL_UP,       AKEYCODE_DPAD_UP},
        {SCE_CTRL_LEFT,     AKEYCODE_DPAD_LEFT},
        {SCE_CTRL_RIGHT,    AKEYCODE_DPAD_RIGHT},
        {SCE_CTRL_DOWN,     AKEYCODE_DPAD_DOWN},
        {SCE_CTRL_SELECT,   AKEYCODE_ESCAPE},
        {SCE_CTRL_L1,       AKEYCODE_F1},
        {SCE_CTRL_R1,       AKEYCODE_F2},
        {SCE_CTRL_START,    AKEYCODE_F3},
};

/*
 * Control Event Ring Buffer
 */

static event_t buffer[EVENT_BUFFER_LEN];
static int buffer_st;
static int buffer_ed;
static SceKernelLwMutexWork buffer_mutex;

void event_buf_init() {
    buffer_st = 0;
    buffer_ed = 1;
    sceKernelCreateLwMutex(&buffer_mutex, "event buffer mutex", 0, 0, NULL);
}

event_t *event_buf_top() {
    if (event_buf_is_empty()) {
        return NULL;
    }
    return &buffer[buffer_st];
}

void event_buf_pop() {
    if (event_buf_is_empty()) {
        return;
    }
    buffer_st = (buffer_st + 1) % EVENT_BUFFER_LEN;
}

event_t *event_buf_allocate() {
    if (event_buf_is_full()) {
        return NULL;
    }
    int alloc_pos = (buffer_ed - 1 + EVENT_BUFFER_LEN) % EVENT_BUFFER_LEN;
    buffer_ed = (buffer_ed + 1) % EVENT_BUFFER_LEN;
    return &buffer[alloc_pos];
}

bool event_buf_is_empty() {
    return buffer_ed == (buffer_st + 1) % EVENT_BUFFER_LEN;
}

bool event_buf_is_full() {
    return buffer_ed == buffer_st;
}

void event_buf_lock() {
    sceKernelLockLwMutex(&buffer_mutex, 1, NULL);
}

void event_buf_unlock() {
    sceKernelUnlockLwMutex(&buffer_mutex, 1);
}

/*
 * Control thread
 */

void submit_key_event(bool is_key_up, int key_code) {
    event_buf_lock();
    {
        event_t *event = event_buf_allocate();
        if (event != NULL) {
            event->type = TYPE_KEY;
            event->key_event.is_key_up = is_key_up;
            event->key_event.key_code = key_code;
            // debugPrintf("Trigger key event: up = %d, key = %d\n", is_key_up, key_code);
        } else {
            debugPrintf("[WARN] event buffer full! pending events will be dropped!\n");
        }
    }
    event_buf_unlock();
}

void submit_touch_event(touch_motion_t type, float x, float y) {
    event_buf_lock();
    {
        event_t *event = event_buf_allocate();
        if (event != NULL) {
            event->type = TYPE_TOUCH;
            event->touch_event.motion = type;
            event->touch_event.x = x;
            event->touch_event.y = y;
            // debugPrintf("Trigger touch event: type = %d, x = %f, y = %f\n", type, x, y);
        } else {
            debugPrintf("[WARN] event buffer full! pending events will be dropped!\n");
        }
    }
    event_buf_unlock();
}

void submit_joystick_event(float x, float y, float rx, float ry) {
    event_buf_lock();
    {
        event_t *event = event_buf_allocate();
        if (event != NULL) {
            event->type = TYPE_JOYSTICK;
            event->joystick_event.x = x;
            event->joystick_event.y = y;
            event->joystick_event.rx = rx;
            event->joystick_event.ry = ry;
            // debugPrintf("Trigger joystick event: x = %f, y = %f, rx = %f, ry = %f\n", x, y, rx, ry);
        } else {
            debugPrintf("[WARN] event buffer full! pending events will be dropped!\n");
        }
    }
    event_buf_unlock();
}

_Noreturn int ctrl_thread(SceSize args, void *argp) {
    uint32_t old_buttons = 0, current_buttons = 0, down_buttons = 0, up_buttons = 0;
    float last_x[2] = {-1, -1};
    float last_y[2] = {-1, -1};
    float last_pad[4] = {0, 0, 0, 0};
    float cur_pad[4] = {0, 0, 0, 0};

    while (1) {
        // Touch event
        SceTouchData touch;
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);

        for (int i = 0; i < 2; i++) {
            if (i < touch.reportNum) {
                float x = (float) touch.report[i].x * (float) screen_width / 1920.0f;
                float y = (float) touch.report[i].y * (float) screen_height / 1088.0f;

                if (last_x[i] == -1 || last_y[i] == -1)
                    submit_touch_event(MOTION_DOWN, x, y);
                else if (last_x[i] != x || last_y[i] != y)
                    submit_touch_event(MOTION_MOVE, x, y);
                last_x[i] = x;
                last_y[i] = y;
            } else {
                if (last_x[i] != -1 || last_y[i] != -1)
                    submit_touch_event(MOTION_UP, last_x[i], last_y[i]);
                last_x[i] = -1;
                last_y[i] = -1;
            }
        }

        // Key event
        SceCtrlData pad;
        sceCtrlPeekBufferPositiveExt2(0, &pad, 1);

        old_buttons = current_buttons;
        current_buttons = pad.buttons;

        // for joystick
        cur_pad[0] = pad.lx >= 128 - DEAD_ZONE && pad.lx <= 128 + DEAD_ZONE ? 0.0f : ((float) pad.lx - 128.0f) / 128.0f;
        cur_pad[1] = pad.ly >= 128 - DEAD_ZONE && pad.ly <= 128 + DEAD_ZONE ? 0.0f : ((float) pad.ly - 128.0f) / 128.0f;
        cur_pad[2] = pad.rx >= 128 - DEAD_ZONE && pad.rx <= 128 + DEAD_ZONE ? 0.0f : ((float) pad.rx - 128.0f) / 128.0f;
        cur_pad[3] = pad.ry >= 128 - DEAD_ZONE && pad.ry <= 128 + DEAD_ZONE ? 0.0f : ((float) pad.ry - 128.0f) / 128.0f;

        // add left joystick mapping if joystick not supported
        if (!SUPPORT_JOYSTICK) {
            if (cur_pad[0] <= -0.7) {
                current_buttons |= SCE_CTRL_LEFT;
            } else if (cur_pad[0] > 0.7) {
                current_buttons |= SCE_CTRL_RIGHT;
            }
            if (cur_pad[1] <= -0.7) {
                current_buttons |= SCE_CTRL_UP;
            } else if (cur_pad[1] > 0.7) {
                current_buttons |= SCE_CTRL_DOWN;
            }
        }

        down_buttons = current_buttons & ~old_buttons;
        up_buttons = ~current_buttons & old_buttons;

        for (int i = 0; i < sizeof(mapping) / sizeof(button_mapping_t); i++) {
            if (down_buttons & mapping[i].sce_button) {
                submit_key_event(false, mapping[i].android_button);
            }
            if (up_buttons & mapping[i].sce_button) {
                submit_key_event(true, mapping[i].android_button);
            }
        }

        // Joystick event
        if (SUPPORT_JOYSTICK) {
            if (cur_pad[0] != last_pad[0] || cur_pad[1] != last_pad[1]) {
                submit_joystick_event(cur_pad[0], cur_pad[1], cur_pad[2], cur_pad[3]);
                memcpy(last_pad, cur_pad, sizeof(cur_pad));
            }
        }
    }
}

/*
 * Hook functions
 */

int process_control_event() {
    int (*onInputEvent)(void *app, void *event) = (int (*)(void *, void *)) FUNC_ADDR(onInputEvent);
    int ret;

    event_buf_lock();
    {
        event_t *event = event_buf_top();
        if (event != NULL) {
            onInputEvent(NULL, event);
            event_buf_pop();
        }
        ret = event_buf_is_empty() ? -1 : 1;
    }
    event_buf_unlock();
    return ret;
}

int AInputEvent_getType(event_t *event) {
    switch (event->type) {
        case TYPE_KEY:
            return 1;
        case TYPE_TOUCH:
        case TYPE_JOYSTICK:
            return 2;
        default:
            return -1;
    }
}

int AInputEvent_getSource(event_t* event) {
    switch (event->type) {
        case TYPE_KEY:
        case TYPE_TOUCH:
            return 0x3002;
        case TYPE_JOYSTICK:
            return 0x1000411;
        default:
            return 0;
    }
}

int AMotionEvent_getAction(event_t *event) {
    if (event->type == TYPE_KEY) {
        return event->key_event.is_key_up;
    } else if (event->type == TYPE_TOUCH) {
        return event->touch_event.motion;
    } else {
        return -1;
    }
}

int AKeyEvent_getKeyCode(event_t *event) {
    assert(event->type == TYPE_KEY);
    return event->key_event.key_code;
}

int AMotionEvent_getPointerCount(event_t *event) {
    assert(event->type == TYPE_TOUCH);
    return 1;
}

int AMotionEvent_getPointerId(event_t *event, size_t index) {
    assert(index == 0);
    return 0;
}

sfp_float AMotionEvent_getX(event_t *event, size_t index) {
    assert(index == 0);
    return float2sfp(event->touch_event.x);
}

sfp_float AMotionEvent_getY(event_t *event, size_t index) {
    assert(index == 0);
    return float2sfp(event->touch_event.y);
}

sfp_float AMotionEvent_getAxisValue(event_t* event, int32_t axis, size_t pointer_index) {
    assert(pointer_index == 0);
    assert(event->type == TYPE_JOYSTICK);
    switch (axis) {
        case AXIS_X:
            return float2sfp(event->joystick_event.x);
        case AXIS_Y:
            return float2sfp(event->joystick_event.y);
        case AXIS_RX:
            return float2sfp(event->joystick_event.rx);
        case AXIS_RY:
            return float2sfp(event->joystick_event.ry);
        default:
            return 0;
    }
}
