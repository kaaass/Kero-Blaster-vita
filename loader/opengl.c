#include <psp2common/types.h>
#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stddef.h>
#include <psp2/kernel/modulemgr.h>
#include <gpu_es4/psp2_pvr_hint.h>
#include <stdlib.h>

int debugPrintf(char *text, ...);

static EGLDisplay egl_display;
static EGLSurface egl_surface;
static EGLContext egl_context;

/*
 * Init stuff
 */

void init_pvr_psp2() {
    sceKernelLoadStartModule("vs0:sys/external/libfios2.suprx", 0, NULL, 0, NULL, NULL);
    sceKernelLoadStartModule("vs0:sys/external/libc.suprx", 0, NULL, 0, NULL, NULL);
    sceKernelLoadStartModule("app0:module/libgpu_es4_ext.suprx", 0, NULL, 0, NULL, NULL);
    sceKernelLoadStartModule("app0:module/libIMGEGL.suprx", 0, NULL, 0, NULL, NULL);

    // hint PVR_PSP2
    PVRSRV_PSP2_APPHINT hint;
    PVRSRVInitializeAppHint(&hint);
    hint.ui32UNCTexHeapSize = 60 * 1024 * 1024;
    hint.ui32CDRAMTexHeapSize = 96 * 1024 * 1024;
    PVRSRVCreateVirtualAppHint(&hint);
}

int init_egl() {
    EGLConfig configs[2];
    EGLBoolean eRetStatus;
    EGLint major, minor;
    EGLint config_count;

    EGLint cfg_attribs[] = {
            EGL_BUFFER_SIZE, EGL_DONT_CARE,
            EGL_DEPTH_SIZE, 16,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_STENCIL_SIZE, 8,
            EGL_SURFACE_TYPE, 5,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    EGLint ctx_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eRetStatus = eglInitialize(egl_display, &major, &minor);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglInitialize\n");
        exit(0);
    }
    debugPrintf("EGL version: %d.%d\n", major, minor);

    eRetStatus = eglBindAPI(EGL_OPENGL_ES_API);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglBindAPI\n");
        exit(0);
    }

    eRetStatus = eglGetConfigs(egl_display, configs, 2, &config_count);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglGetConfigs\n");
        exit(0);
    }

    eRetStatus = eglChooseConfig(egl_display, cfg_attribs, configs, 2, &config_count);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglChooseConfig\n");
        exit(0);
    }

    Psp2NativeWindow win;
    win.type = PSP2_DRAWABLE_TYPE_WINDOW;
    win.windowSize = PSP2_WINDOW_960X544;
    win.numFlipBuffers = 2;
    win.flipChainThrdAffinity = 0;

    egl_surface = eglCreateWindowSurface(egl_display, configs[0], &win, NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        debugPrintf("Error: eglCreateWindowSurface\n");
        exit(0);
    }

    egl_context = eglCreateContext(egl_display, configs[0], EGL_NO_CONTEXT, ctx_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        debugPrintf("Error: eglCreateContext\n");
        exit(0);
    }

    eRetStatus = eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglMakeCurrent\n");
        exit(0);
    }

    return 0;
}

/*
 * EGL Hook
 */

EGLContext
eglCreateContext_hook(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
    return egl_context;
}

EGLSurface
eglCreateWindowSurface_hook(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list) {
    return egl_surface;
}

EGLDisplay eglGetDisplay_hook(EGLNativeDisplayType display_id) {
    return egl_display;
}