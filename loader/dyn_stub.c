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
#include <vitaGL.h>
#include <psp2/ctrl.h>

#include "main.h"

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

int ret0(void) {
    return 0;
}

int ret1(void) {
    return 1;
}

size_t __strlen_chk(const char *s, size_t s_len) {
    return strlen(s);
}

int __android_log_print(int prio, const char *tag, const char *fmt, ...) {
#ifdef DEBUG
    va_list list;
    static char string[0x8000];

    va_start(list, fmt);
    vsprintf(string, fmt, list);
    va_end(list);

    debugPrintf("[LOG] %s: %s\n", tag, string);
#endif
    return 0;
}

int __android_log_write(int prio, const char *tag, const char *text) {
#ifdef DEBUG
    printf("[LOG] %s: %s\n", tag, text);
#endif
    return 0;
}

int pipe (int fds[2]) {
    fds[0] = 114;
    fds[1] = 514;
    printf("stub: create pipe %d <-> %d\n", fds[0], fds[1]);
    return 0;
}

STUB_FUNC(AAsset_close)
STUB_FUNC(AAsset_getBuffer)
STUB_FUNC(AAsset_getLength)
STUB_FUNC(AAssetManager_open)
STUB_FUNC(AConfiguration_delete)
STUB_FUNC(AConfiguration_fromAssetManager)
STUB_FUNC(AConfiguration_getCountry)
STUB_FUNC(AConfiguration_getLanguage)
STUB_FUNC(AConfiguration_new)
STUB_FUNC(AInputEvent_getType)
STUB_FUNC(AInputQueue_attachLooper)
STUB_FUNC(AInputQueue_detachLooper)
STUB_FUNC(AInputQueue_finishEvent)
STUB_FUNC(AInputQueue_getEvent)
STUB_FUNC(AInputQueue_preDispatchEvent)
STUB_FUNC(AKeyEvent_getKeyCode)
STUB_FUNC(ALooper_addFd)
STUB_FUNC(ALooper_pollAll)
STUB_FUNC(ALooper_prepare)
STUB_FUNC(AMotionEvent_getAction)
STUB_FUNC(AMotionEvent_getPointerCount)
STUB_FUNC(AMotionEvent_getPointerId)
STUB_FUNC(AMotionEvent_getX)
STUB_FUNC(AMotionEvent_getY)
STUB_FUNC(ANativeWindow_setBuffersGeometry)
// fixme
STUB_FUNC(eglChooseConfig)
STUB_FUNC(eglCreateContext)
STUB_FUNC(eglCreateWindowSurface)
STUB_FUNC(eglDestroySurface)
STUB_FUNC(eglGetConfigAttrib)
STUB_FUNC(eglInitialize)
STUB_FUNC(eglMakeCurrent)
STUB_FUNC(eglQuerySurface)
// fixme
STUB_FUNC(dlclose)
STUB_FUNC(dlopen)
STUB_FUNC(dlsym)
STUB_FUNC(dl_unwind_find_exidx)

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

void glTexImage2DHook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
                      GLenum format, GLenum type, const void *data) {
    if (level == 0)
        glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}

/*
 * Symbol resolve logic
 */

// dynamic symbol trace should be debug only
#if !defined(DEBUG) && defined(TRACE_DYN_SYMBOL)
#undef TRACE_DYN_SYMBOL
#endif

#if defined(TRACE_DYN_SYMBOL)
char *trace_ban_prefix[] = {"str", "mem", "malloc", "realloc", "free","_"};

void trace_print(const char* sym, int a, int b, int c, int d) {
    for (int i = 0; i < sizeof(trace_ban_prefix) / sizeof(char *); i++) {
        int len = strlen(trace_ban_prefix[i]);
        if (!strncmp(sym, trace_ban_prefix[i], len))
            return;
    }
    printf("dyn symbol trace: %s\n", sym);
}
#define DYN_SYMBOL(name, sym) int __trace_##name(int a, int b, int c, int d, int e, int f, int g, int h) { \
    trace_print(#name,a,b,c,d); \
    return ((int (*)(int,int,int,int,int,int,int,int))((void *) &sym))(a,b,c,d,e,f,g,h); \
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
