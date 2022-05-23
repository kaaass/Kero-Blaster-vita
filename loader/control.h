#include <psp2common/types.h>
#include <stdbool.h>

#ifndef KERO_BLASTER_VITA_CONTROL_H
#define KERO_BLASTER_VITA_CONTROL_H

typedef struct {
    bool is_key_event;
    union {
        struct {
            bool is_key_up;
            int key_code;
        } key_event;
        struct {
            // todo
        } touch_event;
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

int AMotionEvent_getAction(event_t *event);

int AKeyEvent_getKeyCode(event_t *event);

#endif //KERO_BLASTER_VITA_CONTROL_H
