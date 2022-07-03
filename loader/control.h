#include <psp2common/types.h>
#include <stdbool.h>
#include "sfp.h"

#ifndef KERO_BLASTER_VITA_CONTROL_H
#define KERO_BLASTER_VITA_CONTROL_H

typedef enum {
    TYPE_KEY = 1,
    TYPE_TOUCH = 2,
    TYPE_JOYSTICK = 3,
} event_type_t;

typedef enum {
    MOTION_DOWN = 0,
    MOTION_UP = 1,
    MOTION_MOVE = 2,
    MOTION_CANCEL = 3,
} touch_motion_t;

typedef struct {
    event_type_t type;
    union {
        struct {
            bool is_key_up;
            int key_code;
        } key_event;
        struct {
            touch_motion_t motion;
            float x;
            float y;
        } touch_event;
        struct {
            float x;
            float y;
            float rx;
            float ry;
        } joystick_event;
    };
} event_t;

/*
 * Control Event Ring Buffer
 */

void event_buf_init();

event_t *event_buf_top();

void event_buf_pop();

event_t *event_buf_allocate();

bool event_buf_is_empty();

bool event_buf_is_full();

void event_buf_lock();

void event_buf_unlock();

/*
 * Control thread
 */

_Noreturn int ctrl_thread(SceSize args, void *argp);

/*
 * Hook functions
 */

int process_control_event();

int AInputEvent_getType(event_t *event);

int AInputEvent_getSource(event_t* event);

int AMotionEvent_getAction(event_t *event);

int AKeyEvent_getKeyCode(event_t *event);

int AMotionEvent_getPointerCount(event_t *event);

int AMotionEvent_getPointerId(event_t *event, size_t index);

sfp_float AMotionEvent_getX(event_t *event, size_t index);

sfp_float AMotionEvent_getY(event_t *event, size_t index);

sfp_float AMotionEvent_getAxisValue(event_t* motion_event, int32_t axis, size_t pointer_index);

#endif //KERO_BLASTER_VITA_CONTROL_H
