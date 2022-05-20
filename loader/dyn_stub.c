#include <psp2/io/fcntl.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h>
#include <psp2/apputil.h>
#include <psp2/power.h>
#include <psp2/rtc.h>
#include <psp2/touch.h>
#include <kubridge.h>

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
#include <psp2/ctrl.h>
#include <stdbool.h>
#include <sys/syslimits.h>
#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "main.h"
#include "jni.h"
#include "opengl.h"
#include "control.h"
#include "so_util.h"

#define STUB_FUNC_FULL(name, rettype, retval, ...) rettype name(__VARGS__) { \
                            printf("stub: '" #name "' unimplemented\n"); \
                            return (retval); \
                        }
#define STUB_FUNC(name) STUB_FUNC_FULL(name, void *, NULL)

extern void *__cxa_atexit;
extern void *__cxa_finalize;
extern void *__stack_chk_fail;
static int __stack_chk_guard_fake = 0x42424242;
static FILE __sF_fake[0x100][3];
extern void *_ZdaPv;
extern void *_ZdlPv;
extern void *_Znaj;
extern void *_Znwj;
extern void *_ZTVN10__cxxabiv117__class_type_infoE;
extern void *_ZTVN10__cxxabiv120__si_class_type_infoE;
extern void *_ZTVN10__cxxabiv121__vmi_class_type_infoE;

extern void *__cxa_allocate_exception();
extern void *__cxa_begin_catch();
extern void *__cxa_free_exception();
extern void *__cxa_guard_acquire();
extern void *__cxa_guard_release();
extern void *__cxa_throw();
extern void *__memcpy_chk();
extern void *__gxx_personality_v0();
extern void *__strcat_chk();
extern void *__strcpy_chk();
extern void *__vsprintf_chk();
extern void *_ZNSt11logic_errorC2EPKc();
extern void *_ZNSt12length_errorD1Ev();
extern void *_ZSt9terminatev();
extern void *_ZTISt12length_error();
extern void *_ZTVSt12length_error();
extern void *_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6resizeEjc();
extern void *_ZNSt6chrono3_V212steady_clock3nowEv(void *clock);

bool *g_is_error = (bool *) (LOAD_ADDRESS + 0xd31e8);
char **last_error = LOAD_ADDRESS + 0xd31ec;

int ret0(void) {
    return 0;
}

int ret1(void) {
    return 1;
}

size_t __strlen_chk(const char *s, size_t s_len) {
    return strlen(s);
}

void check_last_error() {
    if (*g_is_error) {
        debugPrintf("[LastError]: %s\n", *last_error);
    }
}

void _ZNSt6__ndk16chrono12steady_clock3nowEv(uint64_t *clock) {
    _ZNSt6chrono3_V212steady_clock3nowEv(clock);
    // debugPrintf("_ZNSt6chrono3_V212steady_clock3nowEv = %llu\n", clock);
}

int __android_log_print(int prio, const char *tag, const char *fmt, ...) {
#ifdef DEBUG
    va_list list;
    static char string[0x8000];

    va_start(list, fmt);
    vsprintf(string, fmt, list);
    va_end(list);

    debugPrintf("[LOG] %s: %s\n", tag, string);
    check_last_error();
#endif
    return 0;
}

int __android_log_write(int prio, const char *tag, const char *text) {
#ifdef DEBUG
    printf("[LOG] %s: %s\n", tag, text);
    check_last_error();
#endif
    return 0;
}

int pipe (int fds[2]) {
    fds[0] = 114;
    fds[1] = 514;
    printf("stub: create pipe %d <-> %d\n", fds[0], fds[1]);
    return 0;
}

int ALooper_pollAll(int timeout, int *outFd, int *outEvents, void **outData) {
    // debugPrintf("ALooper_pollAll(outFd = %x, outEvents = %x, outData = %x)\n", outFd, outEvents, outData);

    static int call_count = 0;
    static uintptr_t empty_callback[3] = {0, 0, (uintptr_t) &ret0};

    // return empty callback
    *outData = empty_callback;
    // process event
    call_count++;
    if (call_count == 1) {
        void (*onAppCmd)(void *app, int cmd) = (void (*)(void *, int)) (LOAD_ADDRESS + 0x17fb8 + 1);
        // trigger INIT_WINDOW
        *(uintptr_t *) (fake_activity.instance + 0x24) = 0x42424242;
        onAppCmd(fake_activity.instance, 1);
        return 1;
    } else if (call_count == 2) {
        // do nothing
        return -1;
    } else {
        return process_control_event();
    }
}

typedef struct {
    FILE *fp;
    void *buf;
    off_t length;
} Asset;

Asset *AAssetManager_open(void *mgr, char *filename, int mode) {
    char pathname[PATH_MAX];
    debugPrintf("Open asset file: %s\n", filename);
    if (mode != 3) {
        debugPrintf("Unsupport asset open mode (%d)!\n", mode);
        return NULL;
    }
    strcpy(pathname, ASSETS_PATH);
    strcat(pathname, filename);
    FILE *fp = fopen(pathname, "rb");
    if (fp == NULL) {
        return NULL;
    }
    Asset *result = malloc(sizeof(Asset));
    result->fp = fp;
    result->buf = NULL;
    result->length = 0;
    return result;
}

off_t AAsset_getLength(Asset *asset) {
    return asset->length;
}

void *AAsset_getBuffer(Asset *asset) {
    if (asset->buf == NULL) {
        fseek(asset->fp, 0L, SEEK_END);
        off_t len = ftell(asset->fp);
        fseek(asset->fp, 0L, SEEK_SET);
        void *buf = malloc(len);
        fread(buf, len, 1, asset->fp);
        asset->buf = buf;
        asset->length = len;
    }
    return asset->buf;
}

void AAsset_close(Asset *asset) {
    fclose(asset->fp);
    if (asset->buf) {
        free(asset->buf);
        asset->buf = NULL;
    }
    free(asset);
}

STUB_FUNC(AConfiguration_delete)
STUB_FUNC(AConfiguration_fromAssetManager)
STUB_FUNC(AConfiguration_getCountry)
STUB_FUNC(AConfiguration_getLanguage)
STUB_FUNC(AConfiguration_new)
STUB_FUNC(AInputQueue_attachLooper)
STUB_FUNC(AInputQueue_detachLooper)
STUB_FUNC(AInputQueue_finishEvent)
STUB_FUNC(AInputQueue_getEvent)
STUB_FUNC(AInputQueue_preDispatchEvent)
STUB_FUNC(ALooper_addFd)
STUB_FUNC(ALooper_prepare)
STUB_FUNC(AMotionEvent_getPointerCount)
STUB_FUNC(AMotionEvent_getPointerId)
STUB_FUNC(AMotionEvent_getX)
STUB_FUNC(AMotionEvent_getY)
STUB_FUNC(ANativeWindow_setBuffersGeometry)
STUB_FUNC(dlclose)
STUB_FUNC(dlopen)
STUB_FUNC(dlsym)
// fixme
STUB_FUNC(dl_unwind_find_exidx)

STUB_FUNC(_ZNKSt6__ndk120__vector_base_commonILb1EE20__throw_length_errorEv)
STUB_FUNC(_ZNSt6__ndk112__next_primeEj)
STUB_FUNC(_ZNSt6__ndk119__shared_weak_count14__release_weakEv)
STUB_FUNC(_ZNSt6__ndk15mutex4lockEv)
STUB_FUNC(_ZNSt6__ndk15mutex6unlockEv)
STUB_FUNC(_ZNSt6__ndk15mutex8try_lockEv)
STUB_FUNC(_ZNSt6__ndk15mutexD1Ev)

int pthread_create_fake(pthread_t *thread, const void *unused, void *entry, void *arg) {
    return pthread_create(thread, NULL, entry, arg);
}

int pthread_mutex_init_fake(pthread_mutex_t **uid,
                            const pthread_mutexattr_t *mutexattr) {
    pthread_mutex_t *m = calloc(1, sizeof(pthread_mutex_t));
    if (!m)
        return -1;

    const int recursive = (mutexattr && *(const int *)mutexattr == 1);
    *m = recursive ? PTHREAD_RECURSIVE_MUTEX_INITIALIZER
                   : PTHREAD_MUTEX_INITIALIZER;

    int ret = pthread_mutex_init(m, mutexattr);
    if (ret < 0) {
        free(m);
        return -1;
    }

    *uid = m;

    return 0;
}

int pthread_mutex_destroy_fake(pthread_mutex_t **uid) {
    if (uid && *uid && (uintptr_t)*uid > 0x8000) {
        pthread_mutex_destroy(*uid);
        free(*uid);
        *uid = NULL;
    }
    return 0;
}

int pthread_mutex_lock_fake(pthread_mutex_t **uid) {
    int ret = 0;
    if (!*uid) {
        ret = pthread_mutex_init_fake(uid, NULL);
    } else if ((uintptr_t)*uid == 0x4000) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        ret = pthread_mutex_init_fake(uid, &attr);
        pthread_mutexattr_destroy(&attr);
    } else if ((uintptr_t)*uid == 0x8000) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        ret = pthread_mutex_init_fake(uid, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    if (ret < 0)
        return ret;
    return pthread_mutex_lock(*uid);
}

int pthread_mutex_unlock_fake(pthread_mutex_t **uid) {
    int ret = 0;
    if (!*uid) {
        ret = pthread_mutex_init_fake(uid, NULL);
    } else if ((uintptr_t)*uid == 0x4000) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        ret = pthread_mutex_init_fake(uid, &attr);
        pthread_mutexattr_destroy(&attr);
    } else if ((uintptr_t)*uid == 0x8000) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        ret = pthread_mutex_init_fake(uid, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    if (ret < 0)
        return ret;
    return pthread_mutex_unlock(*uid);
}

int pthread_mutex_trylock_fake(pthread_mutex_t **uid) {
    int ret = -1;
    if (uid && *uid && (uintptr_t)*uid > 0x8000) {
        ret = pthread_mutex_trylock(*uid);
    } else {
        debugPrintf("[WARN] pthread_mutex_trylock_fake unimplement branch!");
    }
    return ret;
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

/*
 * Softfp wrapper
 */

int64_t atan_sfp(int64_t a1) {
    double fa1;
    int64_t ires;

    fa1 = *(double *)(&a1);
    double fres = atan(fa1);
    ires = *(int64_t *)(&fres);

    return ires;
}

int64_t ceil_sfp(int64_t a1) {
    double fa1;
    int64_t ires;

    fa1 = *(double *)(&a1);
    double fres = ceil(fa1);
    ires = *(int64_t *)(&fres);

    return ires;
}

int ceilf_sfp(int a1) {
    float fa1;
    int ires;

    fa1 = *(float *)(&a1);
    float fres = ceilf(fa1);
    ires = *(int *)(&fres);

    return ires;
}

int cosf_sfp(int a1) {
    float fa1;
    int ires;

    fa1 = *(float *)(&a1);
    float fres = cosf(fa1);
    ires = *(int *)(&fres);

    return ires;
}

int powf_sfp(int a1, int a2) {
    float fa1, fa2;
    int ires;

    fa1 = *(float *)(&a1);
    fa2 = *(float *)(&a2);
    float fres = powf(fa1, fa2);
    ires = *(int *)(&fres);

    return ires;
}

int64_t sin_sfp(int64_t a1) {
    double fa1;
    int64_t ires;

    fa1 = *(double *)(&a1);
    double fres = sin(fa1);
    ires = *(int64_t *)(&fres);

    return ires;
}

void sincos_sfp(int64_t a, double *b, double *c) {
    double fa1;

    fa1 = *(double *)(&a);
    sincos(fa1, b, c);
}

void sincosf_sfp(float a, float *b, float *c) {
    float fa1;

    fa1 = *(float *)(&a);
    sincosf(fa1, b, c);
}

int sinf_sfp(int a1) {
    float fa1;
    int ires;

    fa1 = *(float *)(&a1);
    float fres = sinf(fa1);
    ires = *(int *)(&fres);

    return ires;
}

void glClearColor_sfp(int i_red, int i_green, int i_blue, int i_alpha) {
    float red, green, blue, alpha;

    red = *(float *)(&i_red);
    green = *(float *)(&i_green);
    blue = *(float *)(&i_blue);
    alpha = *(float *)(&i_alpha);
    glClearColor(red, green, blue, alpha);
}

void glTexParameterf_sfp(GLenum target, GLenum pname, int i_param) {
    float param;

    param = *(float *)(&i_param);
    glTexParameterf(target, pname, param);
}

/*
 * Symbol resolve logic
 */

// dynamic symbol trace should be debug only
#if !defined(DEBUG) && defined(TRACE_DYN_SYMBOL)
#undef TRACE_DYN_SYMBOL
#endif

#if defined(TRACE_DYN_SYMBOL)
char *trace_ban_prefix[] = {"str", "mem", "malloc", "realloc", "free","_",
                            "sin"};

void trace_print(const char* sym, int a, int b, int c, int d) {
    for (int i = 0; i < sizeof(trace_ban_prefix) / sizeof(char *); i++) {
        int len = strlen(trace_ban_prefix[i]);
        if (!strncmp(sym, trace_ban_prefix[i], len))
            return;
    }
    printf("dyn symbol trace: %s\n", sym);
}
#define DYN_SYMBOL(name, sym) int __trace_##name(int p0,int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8, int p9) { \
    trace_print(#name,p0,p1,p2,p3); \
    return ((int (*)(int,int,int,int,int,int,int,int,int,int))((void *) &sym))(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9); \
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

void resolve_dynamic_symbol() {
    so_resolve(&kero_mod, default_dynlib, sizeof(default_dynlib), 1);
}
