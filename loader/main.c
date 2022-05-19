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
#include <vitaGL.h>
#include <psp2/ctrl.h>
#include <malloc.h>
#include <kubridge.h>
#include <stdlib.h>

#include "main.h"
#include "config.h"
#include "dialog.h"
#include "so_util.h"
#include "sha1.h"
#include "jni.h"
#include "dyn_stub.h"

#define debug_flag debugPrintf("%s:%d\n", __FILE__, __LINE__);

__attribute__((unused)) int _newlib_heap_size_user = MEMORY_NEWLIB_MB * 1024 * 1024; // NOLINT(bugprone-reserved-identifier)
__attribute__((unused)) unsigned int _pthread_stack_default_user = 1 * 1024 * 1024; // NOLINT(bugprone-reserved-identifier)

so_module kero_mod;

char *replaced_shader_names[] = {
        "Textured.vert",
        "Textured.frag",
        "Color_Fill.vert",
        "Color_Fill.frag",
};
const int n_replaced_shader = sizeof(replaced_shader_names) / sizeof(char *);
char *replaced_shader[sizeof(replaced_shader_names) / sizeof(char *)];

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

void go() {
    debugPrintf("trigger goooooooooooooooo\n");
    exit(0);
}

int fake_load_se(void *se_ctx, char *name, int slot) {
    debugPrintf("fake_load_se(ctx = %x, name = '%s', slot = %x)\n", se_ctx, name, slot);
    return 1;
}

void patch_game(void) {
    // patch 0x17dd8 bne -> beq
    uint16_t beq = 0xd018;
    kuKernelCpuUnrestrictedMemcpy((void *)(LOAD_ADDRESS + 0x17dd8), &beq, sizeof(beq));

    // todo fix audio
    hook_addr(LOAD_ADDRESS + 0xaeb40 + 1, (uintptr_t) &fake_load_se);

    // hook_addr(LOAD_ADDRESS + 0xaeb3a + 1, (uintptr_t) &go);
//    hook_addr(so_symbol(&twom_mod, "__cxa_guard_acquire"), (uintptr_t) &__cxa_guard_acquire);
//    hook_addr(so_symbol(&twom_mod, "__cxa_guard_release"), (uintptr_t) &__cxa_guard_release);
//
//    hook_addr(so_symbol(&twom_mod, "_ZN10FileSystem14IsAbsolutePathEPKc"), (uintptr_t) FileSystem__IsAbsolutePath);
//    hook_addr(so_symbol(&twom_mod, "_ZN13ShaderManager13GetShaderPathEv"), (uintptr_t) ShaderManager__GetShaderPath);
//
//    hook_addr(so_symbol(&twom_mod, "_Z17GetApkAssetOffsetPKcRj"), (uintptr_t) ret0);
//
//    TotalMemeorySizeInMB = (int *) so_symbol(&twom_mod, "TotalMemeorySizeInMB");
//    hook_addr(so_symbol(&twom_mod, "_Z22DeteremineSystemMemoryv"), (uintptr_t) DeteremineSystemMemory);
//
//    hook_addr(so_symbol(&twom_mod, "_ZN14GoogleServices10IsSignedInEv"), (uintptr_t) ret0);
//    hook_addr(so_symbol(&twom_mod, "_ZN26InAppStoreAndroidInterface24IsInAppPurchasePurchasedERK10NameString"),
//              enable_dlcs ? (uintptr_t) ret1 : (uintptr_t) ret0);
//
//    hook_addr(so_symbol(&twom_mod, "_Z12SetGLContextv"), (uintptr_t) ret0);
//    hook_addr(so_symbol(&twom_mod, "_Z16PresentGLContextv"), (uintptr_t) PresentGLContext);
//
//    hook_addr(so_symbol(&twom_mod, "_ZN11GameConsole5PrintEhhPKcz"), (uintptr_t) ret0);
//    hook_addr(so_symbol(&twom_mod, "_ZN11GameConsole12PrintWarningEhPKcz"), (uintptr_t) ret0);
//    hook_addr(so_symbol(&twom_mod, "_ZN11GameConsole10PrintErrorEhPKcz"), (uintptr_t) ret0);

    //_Znwj = (void *)so_symbol(&twom_mod, "_Znwj");
    //CountingSemaphore__Constructor = (void *)so_symbol(&twom_mod, "_ZN17CountingSemaphoreC2Ej");
    //BaseThread__BeginMessage = (void *)so_symbol(&twom_mod, "_ZN10BaseThread12BeginMessageEjj");
    //BaseThread__EndMessage = (void *)so_symbol(&twom_mod, "_ZN10BaseThread10EndMessageEv");
    //BaseThread___ThreadCode = (void *)so_symbol(&twom_mod, "_ZN10BaseThread11_ThreadCodeEv");
    //hook_addr(so_symbol(&twom_mod, "_ZN10BaseThread4InitEv"), (uintptr_t)BaseThread__Init);
    //hook_addr(so_symbol(&twom_mod, "_ZN10BaseThread11SetPriorityEi"), (uintptr_t)ret0);
    //hook_addr(so_symbol(&twom_mod, "_Z18GetCurrentThreadIdv"), (uintptr_t)GetCurrentThreadId);
}

int check_kubridge(void) {
    int search_unk[2];
    return _vshKernelSearchModuleByName("kubridge", search_unk);
}

int file_exists(const char *path) {
    SceIoStat stat;
    return sceIoGetstat(path, &stat) >= 0;
}

void load_shaders() {
    char pathname[30];
    for (int i = 0; i < n_replaced_shader; i++) {
        strcpy(pathname, "app0:shader/");
        strcat(pathname, replaced_shader_names[i]);
        debugPrintf("Load replaced shader: %s\n", pathname);
        // open file
        FILE *fp = fopen(pathname, "r");
        fseek(fp, 0L, SEEK_END);
        int length = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        // read
        char *buf = malloc(length + 1);
        fread(buf, length, 1, fp);
        buf[length] = '\0';
        replaced_shader[i] = buf;
    }
}

int main_thread(SceSize args, void *argp) {
    // init shark
    vglSetupRuntimeShaderCompiler(SHARK_OPT_UNSAFE, SHARK_ENABLE, SHARK_ENABLE,
                                  SHARK_ENABLE);
    vglUseVram(GL_TRUE);
    vglInitExtended(0, SCREEN_W, SCREEN_H,
                    MEMORY_VITAGL_THRESHOLD_MB * 1024 * 1024,
                    SCE_GXM_MULTISAMPLE_4X);

    // load shaders
    load_shaders();

    // setup windows
    int *g_screen_w = (int *) (LOAD_ADDRESS + 0xcdebc);
    int *g_screen_h = (int *) (LOAD_ADDRESS + 0xcdec0);
    int *g_framebuffer_w = (int *) (LOAD_ADDRESS + 0xd20c0);
    int *g_framebuffer_h = (int *) (LOAD_ADDRESS + 0xd20c4);

    *g_screen_w = *g_framebuffer_w = SCREEN_W;
    *g_screen_h = *g_framebuffer_h = SCREEN_H;

    // jni load
    jni_load();

    return 0;
}

int ctrl_thread(SceSize args, void *argp) {

    return 0;
}

int main(int argc, char *argv[]) {
    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    if (check_kubridge() < 0)
        fatal_error("Error kubridge.skprx is not installed.");

    if (!file_exists("ur0:/data/libshacccg.suprx") && !file_exists("ur0:/data/external/libshacccg.suprx"))
        fatal_error("Error libshacccg.suprx is not installed.");

    if (so_load(&kero_mod, SO_PATH, LOAD_ADDRESS) < 0)
        fatal_error("Error could not load %s.", SO_PATH);

    so_relocate(&kero_mod);
    resolve_dynamic_symbol();

    patch_game();
    so_flush_caches(&kero_mod);

    so_initialize(&kero_mod);

    // game thread
//    SceUID thid = sceKernelCreateThread("main_thread", (SceKernelThreadEntry) main_thread, 0x40, 128 * 1024,
//                                        0, 0, NULL);
//    sceKernelStartThread(thid, 0, NULL);
    main_thread(0, 0);

    // ctrl thread
//    SceUID ctrl_thid = sceKernelCreateThread("ctrl_thread", (SceKernelThreadEntry) ctrl_thread, 0x10000100, 128 * 1024, 0, 0, NULL);
//    sceKernelStartThread(ctrl_thid, 0, NULL);

    return sceKernelExitDeleteThread(0);
}
