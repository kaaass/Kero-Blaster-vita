/* patch.c -- Patch several game functions
 *
 * Copyright (C) 2022 KAAAsS
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <stdarg.h>
#include "audio.h"
#include "so_util.h"
#include "config.h"
#include "dyn_stub.h"
#include "main.h"
#include "game_info.h"

void write_last_error_hook(char *fmt, ...) {
    va_list list;
    static char string[0x8000];

    va_start(list, fmt);
    vsprintf(string, fmt, list);
    va_end(list);
#if defined(DEBUG)
    debugPrintf("[LastError]: %s\n", string);
#else
    printf("[LastError]: %s\n", string);
#endif
}

int mutex_lock_hook(void **lock) {
    void *result = lock;
    // game didn't check lock == NULL here, which would cause a crash when exit (#1)
    if (result) {
        result = *(void **) result;
        if (result) {
            result = *(void **) result;
            if (result) {
                _ZNSt6__ndk15mutex4lockEv(result);
                return 1;
            }
        }
    }
    return 0;
}

int mutex_unlock_hook(void **lock) {
    void *result = lock;
    // game didn't check lock == NULL here, which would cause a crash when exit (#1)
    if (result) {
        result = *(void **) result;
        if (result) {
            result = *(void **) result;
            if (result) {
                _ZNSt6__ndk15mutex6unlockEv(result);
                return 1;
            }
        }
    }
    return 0;
}

void patch_game() {
    patch_audio();

    hook_addr(FUNC_ADDR(write_last_error), (uintptr_t) &write_last_error_hook);
    hook_addr(FUNC_ADDR(mutex_lock), (uintptr_t) &mutex_lock_hook);
    hook_addr(FUNC_ADDR(mutex_unlock), (uintptr_t) &mutex_unlock_hook);
}
