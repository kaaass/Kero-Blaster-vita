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
#include "main.h"

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

void patch_game() {
    patch_audio();

    hook_addr(LOAD_ADDRESS + 0xb24c4 + 0x1, (uintptr_t) &write_last_error_hook);
}
