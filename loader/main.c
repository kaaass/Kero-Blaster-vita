/* main.c -- This War of Mine .so loader
 *
 * Copyright (C) 2021 Andy Nguyen
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h>
#include <psp2/apputil.h>
#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/rtc.h>
#include <psp2/touch.h>
#include <kubridge.h>
#include <vitashark.h>
#include <vitaGL.h>
#include <zlib.h>

#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <wchar.h>
#include <wctype.h>

#include <math.h>
#include <math_neon.h>

#include <errno.h>
#include <ctype.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <psp2/io/stat.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <sys/poll.h>
#include <locale.h>

#include "main.h"
#include "config.h"
#include "dialog.h"
#include "so_util.h"
#include "sha1.h"
#include "jni.h"

#define debug_flag debugPrintf("%s:%d\n", __FILE__, __LINE__);

__attribute__((unused)) int _newlib_heap_size_user = MEMORY_NEWLIB_MB * 1024 * 1024; // NOLINT(bugprone-reserved-identifier)
__attribute__((unused)) unsigned int _pthread_stack_default_user = 1 * 1024 * 1024; // NOLINT(bugprone-reserved-identifier)

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

    SceUID fd = sceIoOpen("ux0:data/kero_log.txt", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 0777);
    if (fd >= 0) {
        sceIoWrite(fd, string, strlen(string));
        sceIoClose(fd);
    }
#endif
    return 0;
}

int ret0(void) {
    return 0;
}

int ret1(void) {
    return 1;
}

#define STUB_FUNC_FULL(name, rettype, retval, ...) rettype name(__VARGS__) { \
                            printf("stub: '" #name "' unimplemented\n"); \
                            return (retval); \
                        }
#define STUB_FUNC(name) STUB_FUNC_FULL(name, void *, NULL)

void *AAssetManager_open(void *mgr, const char *filename, int mode) {
    printf("AAssetManager_open\n");
    return NULL;
}

#define ADDR_NATIVE_WINDOW 0x41420000

STUB_FUNC(AAssetManager_fromJava)
STUB_FUNC(AAsset_close)
STUB_FUNC(AAsset_getRemainingLength)
STUB_FUNC(AAsset_read)
STUB_FUNC(AAsset_seek)
STUB_FUNC(AConfiguration_delete)
STUB_FUNC(AConfiguration_fromAssetManager)
STUB_FUNC(AConfiguration_getLanguage)
STUB_FUNC(AConfiguration_new)
STUB_FUNC_FULL(ANativeWindow_fromSurface, void *, (void *) ADDR_NATIVE_WINDOW, void *env, void *surface)
STUB_FUNC(ANativeWindow_getHeight)
STUB_FUNC(ANativeWindow_getWidth)
STUB_FUNC(ANativeWindow_release)
STUB_FUNC(ANativeWindow_setBuffersGeometry)
STUB_FUNC(eglChooseConfig)
STUB_FUNC(eglCreateContext)
STUB_FUNC(eglCreateWindowSurface)
STUB_FUNC(eglGetConfigAttrib)
STUB_FUNC(eglInitialize)
STUB_FUNC(eglMakeCurrent)
STUB_FUNC(glCopyTexSubImage2D)
STUB_FUNC(ioctl)
STUB_FUNC(slCreateEngine)
STUB_FUNC(syscall)
STUB_FUNC_FULL(sysconf, long, 0, int name)
STUB_FUNC(writev)

int gettid(void) { return sceKernelGetThreadId(); }

int pthread_once_fake(volatile int *once_control, void (*init_routine)(void)) {
    if (!once_control || !init_routine)
        return -1;
    if (__sync_lock_test_and_set(once_control, 1) == 0)
        (*init_routine)();
    return 0;
}

int pthread_create_fake(pthread_t *thread, const void *unused, void *entry, void *arg) {
    return pthread_create(thread, NULL, entry, arg);
}

int pthread_cond_init_fake(pthread_cond_t **cnd, const int *condattr) {
    pthread_cond_t *c = calloc(1, sizeof(pthread_cond_t));
    if (!c)
        return -1;

    *c = PTHREAD_COND_INITIALIZER;

    int ret = pthread_cond_init(c, NULL);
    if (ret < 0) {
        free(c);
        return -1;
    }

    *cnd = c;

    return 0;
}

int pthread_cond_broadcast_fake(pthread_cond_t **cnd) {
    if (!*cnd) {
        if (pthread_cond_init_fake(cnd, NULL) < 0)
            return -1;
    }
    return pthread_cond_broadcast(*cnd);
}

int pthread_cond_signal_fake(pthread_cond_t **cnd) {
    if (!*cnd) {
        if (pthread_cond_init_fake(cnd, NULL) < 0)
            return -1;
    }
    return pthread_cond_signal(*cnd);
}

int pthread_cond_destroy_fake(pthread_cond_t **cnd) {
    if (cnd && *cnd) {
        pthread_cond_destroy(*cnd);
        free(*cnd);
        *cnd = NULL;
    }
    return 0;
}

int pthread_cond_wait_fake(pthread_cond_t **cnd, pthread_mutex_t **mtx) {
    if (!*cnd) {
        if (pthread_cond_init_fake(cnd, NULL) < 0)
            return -1;
    }
    return pthread_cond_wait(*cnd, *mtx);
}

int pthread_cond_timedwait_fake(pthread_cond_t **cnd, pthread_mutex_t **mtx,
                                const struct timespec *t) {
    if (!*cnd) {
        if (pthread_cond_init_fake(cnd, NULL) < 0)
            return -1;
    }
    return pthread_cond_timedwait(*cnd, *mtx, t);
}

int pthread_mutexattr_init_fake(int *attr) {
    *attr = 0;
    return 0;
}

int pthread_mutexattr_destroy_fake(int *attr) {
    return 0;
}

int pthread_mutexattr_settype_fake(int *attr, int type) {
    *attr = type;
    return 0;
}

int pthread_mutex_init_fake(SceKernelLwMutexWork **work, int *mutexattr) {
    int recursive = (mutexattr && *mutexattr == 1);
    *work = (SceKernelLwMutexWork *) memalign(8, sizeof(SceKernelLwMutexWork));
    if (sceKernelCreateLwMutex(*work, "mutex", recursive ? SCE_KERNEL_MUTEX_ATTR_RECURSIVE : 0, 0, NULL) < 0)
        return -1;
    return 0;
}

int pthread_mutex_destroy_fake(SceKernelLwMutexWork **work) {
    if (sceKernelDeleteLwMutex(*work) < 0)
        return -1;
    free(*work);
    return 0;
}

int pthread_mutex_lock_fake(SceKernelLwMutexWork **work) {
    if (sceKernelLockLwMutex(*work, 1, NULL) < 0)
        return -1;
    return 0;
}

int pthread_mutex_trylock_fake(SceKernelLwMutexWork **work) {
    if (sceKernelTryLockLwMutex(*work, 1) < 0)
        return -1;
    return 0;
}

int pthread_mutex_unlock_fake(SceKernelLwMutexWork **work) {
    if (sceKernelUnlockLwMutex(*work, 1) < 0)
        return -1;
    return 0;
}

int sem_init_fake(int *uid, int pshared, unsigned value) {
    *uid = sceKernelCreateSema("sema", 0, value, 0x7fffffff, NULL);
    if (*uid < 0)
        return -1;
    return 0;
}

int sem_post_fake(int *uid) {
    if (sceKernelSignalSema(*uid, 1) < 0)
        return -1;
    return 0;
}

int sem_wait_fake(int *uid) {
    if (sceKernelWaitSema(*uid, 1, NULL) < 0)
        return -1;
    return 0;
}

int sem_timedwait_fake(int *uid, const struct timespec *abstime) {
    struct timespec now = {0};
    clock_gettime(0, &now);
    SceUInt timeout =
            (abstime->tv_sec * 1000 * 1000 + abstime->tv_nsec / 1000) - (now.tv_sec * 1000 * 1000 + now.tv_nsec / 1000);
    if (timeout < 0)
        timeout = 0;
    if (sceKernelWaitSema(*uid, 1, &timeout) < 0)
        return -1;
    return 0;
}

int sem_destroy_fake(int *uid) {
    if (sceKernelDeleteSema(*uid) < 0)
        return -1;
    return 0;
}

int	gethostname (char *name, size_t len) {
    if (len < 10)
        return -1;
    strcpy(name, "localhost");
    return 0;
}

void patch_game(void) {
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

extern void *__cxa_atexit;
extern void *__cxa_finalize;
extern void *__stack_chk_fail;
static int __stack_chk_guard_fake = 0x42424242;
static char *__ctype_ = (char *) &_ctype_;
static FILE __sF_fake[0x100][3];

int stat_hook(const char *pathname, void *statbuf) {
    struct stat st;
    int res = stat(pathname, &st);
    if (res == 0)
        *(uint64_t *) (statbuf + 0x30) = st.st_size;
    return res;
}

void glTexImage2DHook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
                      GLenum format, GLenum type, const void *data) {
    if (level == 0)
        glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}

void glCompressedTexImage2DHook(GLenum target, GLint level, GLenum format, GLsizei width, GLsizei height, GLint border,
                                GLsizei imageSize, const void *data) {
    // mips for PVRTC textures break when they're under 1 block in size
    if (level == 0)
        glCompressedTexImage2D(target, level, format, width, height, border, imageSize, data);
}

// dynamic symbol trace should be debug only
#if !defined(DEBUG) && defined(TRACE_DYN_SYMBOL)
#undef TRACE_DYN_SYMBOL
#endif

#if defined(TRACE_DYN_SYMBOL)
char *trace_ban_prefix[] = {"str", "mem", "malloc", "realloc", "free", "pthread",
                            "wctob", "btowc"};

void trace_print(const char* sym, int a, int b, int c, int d) {
    if (!strcmp(sym, "malloc")) {
        printf("malloc: %x\n", *(size_t *)((void *) &a));
    }
    if (!strcmp(sym, "strcpy")) {
        printf("strcpy from: %x, %x, %x\n", a, b, c);
    }
    for (int i = 0; i < sizeof(trace_ban_prefix) / sizeof(char *); i++) {
        int len = strlen(trace_ban_prefix[i]);
        if (!strncmp(sym, trace_ban_prefix[i], len))
            return;
    }
    printf("dyn symbol trace: %s\n", sym);
}

#define DYN_SYMBOL(name, sym) int __trace_##name(int a, int b, int c, int d) { \
    trace_print(#name,a,b,c,d); \
    return ((int (*)(int, int, int, int))((void *) &sym))(a,b,c,d); \
}
#define DYN_SYMBOL_DATA(name, sym)
#include "dyn_symbols.h"
#undef DYN_SYMBOL
#undef DYN_SYMBOL_DATA
#endif

static so_default_dynlib default_dynlib[] = {
#if defined(TRACE_DYN_SYMBOL)
#define DYN_SYMBOL(name, sym) {#name, (uintptr_t) &(__trace_##name)},
#else
#define DYN_SYMBOL(name, sym) {#name, (uintptr_t) &(sym)},
#endif
#define DYN_SYMBOL_DATA(name, sym) {#name, (uintptr_t) &(sym)},
#include "dyn_symbols.h"
#undef DYN_SYMBOL
#undef DYN_SYMBOL_DATA
};

int check_kubridge(void) {
    int search_unk[2];
    return _vshKernelSearchModuleByName("kubridge", search_unk);
}

int file_exists(const char *path) {
    SceIoStat stat;
    return sceIoGetstat(path, &stat) >= 0;
}

int main_thread(SceSize args, void *argp) {
    // init shark
    vglSetupRuntimeShaderCompiler(SHARK_OPT_UNSAFE, SHARK_ENABLE, SHARK_ENABLE,
                                  SHARK_ENABLE);
    vglUseVram(GL_TRUE);
    vglInitExtended(0, SCREEN_W, SCREEN_H,
                    MEMORY_VITAGL_THRESHOLD_MB * 1024 * 1024,
                    SCE_GXM_MULTISAMPLE_4X);

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
    so_resolve(&kero_mod, default_dynlib, sizeof(default_dynlib), 1);

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
