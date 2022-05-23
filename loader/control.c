/* control.c -- Control stubs, event queue and control thread
 *
 * Copyright (C) 2022 KAAAsS
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <assert.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/threadmgr.h>
#include "control.h"
#include "config.h"
#include "main.h"

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

typedef struct {
    uint32_t sce_button;
    int32_t android_button;
} ButtonMapping;

static ButtonMapping mapping[] = {
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
            event->is_key_event = true;
            event->key_event.is_key_up = is_key_up;
            event->key_event.key_code = key_code;
            // debugPrintf("Trigger key event: up = %d, key = %d\n", is_key_up, key_code);
        } else {
            debugPrintf("[WARN] event buffer full! pending events will be dropped!\n");
        }
    }
    event_buf_unlock();
}

_Noreturn int ctrl_thread(SceSize args, void *argp) {
    uint32_t old_buttons = 0, current_buttons = 0, down_buttons = 0, up_buttons = 0;

    while (1) {
        // Key event
        SceCtrlData pad;
        sceCtrlPeekBufferPositiveExt2(0, &pad, 1);

        old_buttons = current_buttons;
        current_buttons = pad.buttons;
        down_buttons = current_buttons & ~old_buttons;
        up_buttons = ~current_buttons & old_buttons;

        for (int i = 0; i < sizeof(mapping) / sizeof(ButtonMapping); i++) {
            if (down_buttons & mapping[i].sce_button) {
                submit_key_event(false, mapping[i].android_button);
            }
            if (up_buttons & mapping[i].sce_button) {
                submit_key_event(true, mapping[i].android_button);
            }
        }
    }
}

/*
 * Hook functions
 */

int process_control_event() {
    int (*onInputEvent)(void *app, void *event) = (int (*)(void *, void *)) (LOAD_ADDRESS + 0x18028 + 1);
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
    return event->is_key_event ? 1 : 2;
}

int AMotionEvent_getAction(event_t *event) {
    if (event->is_key_event) {
        return event->key_event.is_key_up;
    } else {
        // todo
        return -1;
    }
}

int AKeyEvent_getKeyCode(event_t *event) {
    assert(event->is_key_event);
    return event->key_event.key_code;
}
