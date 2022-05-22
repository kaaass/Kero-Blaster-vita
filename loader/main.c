/* main.c -- This War of Mine .so loader
 *
 * Copyright (C) 2021 Andy Nguyen
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <psp2/io/fcntl.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#include <psp2/touch.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <psp2/io/stat.h>
#include <psp2/ctrl.h>
#include <malloc.h>
#include <kubridge.h>
#include <stdlib.h>
#include <EGL/egl1_4.h>

#include "main.h"
#include "config.h"
#include "dialog.h"
#include "so_util.h"
#include "sha1.h"
#include "jni.h"
#include "dyn_stub.h"
#include "opengl.h"
#include "control.h"
#include "audio.h"

#define debug_flag debugPrintf("%s:%d\n", __FILE__, __LINE__);

__attribute__((unused)) int _newlib_heap_size_user = MEMORY_NEWLIB_MB * 1024 * 1024; // NOLINT(bugprone-reserved-identifier)
__attribute__((unused)) unsigned int _pthread_stack_default_user = 1 * 1024 * 1024; // NOLINT(bugprone-reserved-identifier)
__attribute__((unused)) unsigned int sceLibcHeapSize = 12 * 1024 * 1024;

so_module kero_mod;

void *__wrap_memcpy(void *dest, const void *src, size_t n) {
    return sceClibMemcpy(dest, src, n);
}

void *__wrap_memmove(void *dest, const void *src, size_t n) {
    return sceClibMemmove(dest, src, n);
}

void *__wrap_memset(void *s, int c, size_t n) {
    return sceClibMemset(s, c, n);
}

int debugPrintf(char *text, ...) {
#ifdef DEBUG
    va_list list;
    static char string[0x8000];

    va_start(list, text);
    vsprintf(string, text, list);
    va_end(list);
    printf("%s", string);
    fflush(stdout);

    SceUID fd = sceIoOpen("ux0:data/kero_log.txt", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd >= 0) {
        sceIoWrite(fd, string, strlen(string));
        sceIoClose(fd);
    }
#endif
    return 0;
}

void patch_game(void) {
    patch_audio();
}

int check_kubridge(void) {
    int search_unk[2];
    return _vshKernelSearchModuleByName("kubridge", search_unk);
}

int file_exists(const char *path) {
    SceIoStat stat;
    return sceIoGetstat(path, &stat) >= 0;
}

int main(int argc, char *argv[]) {
    debugPrintf("%c[2J", 27);
    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    if (check_kubridge() < 0)
        fatal_error("Error kubridge.skprx is not installed.");

    if (so_load(&kero_mod, SO_PATH, LOAD_ADDRESS) < 0)
        fatal_error("Error could not load %s.", SO_PATH);

    so_relocate(&kero_mod);
    resolve_dynamic_symbol();

    patch_game();
    so_flush_caches(&kero_mod);

    so_initialize(&kero_mod);

    // init pvr_psp2
    init_pvr_psp2();

    // init event buffer
    event_buf_init();

    // jni load
    jni_load();

    // start ctrl thread
    SceUID ctrl_thid = sceKernelCreateThread("ctrl_thread", (SceKernelThreadEntry) ctrl_thread, 0x10000100, 128 * 1024, 0, 0, NULL);
    sceKernelStartThread(ctrl_thid, 0, NULL);

    return sceKernelExitDeleteThread(0);
}
