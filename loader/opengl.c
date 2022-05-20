#include <psp2common/types.h>
#include <stddef.h>
#include <psp2/kernel/modulemgr.h>
#include <gpu_es4/psp2_pvr_hint.h>
#include <stdlib.h>
#include <string.h>
#include "opengl.h"

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
    EGLConfig config;
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
    if (egl_display == EGL_NO_DISPLAY) {
        debugPrintf("Error: eglGetDisplay\n");
        exit(0);
    }

    eRetStatus = eglInitialize(egl_display, &major, &minor);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglInitialize\n");
        exit(0);
    }

    eRetStatus = eglBindAPI(EGL_OPENGL_ES_API);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglBindAPI\n");
        exit(0);
    }

    eRetStatus = eglChooseConfig(egl_display, cfg_attribs, &config, 1, &config_count);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglChooseConfig\n");
        exit(0);
    }

    Psp2NativeWindow win;
    win.type = PSP2_DRAWABLE_TYPE_WINDOW;
    win.windowSize = PSP2_WINDOW_960X544;
    win.numFlipBuffers = 2;
    win.flipChainThrdAffinity = 0;

    egl_surface = eglCreateWindowSurface(egl_display, config, &win, NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        debugPrintf("Error: eglCreateWindowSurface\n");
        exit(0);
    }

    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, ctx_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        debugPrintf("Error: eglCreateContext\n");
        exit(0);
    }

    eRetStatus = eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    if (eRetStatus != EGL_TRUE) {
        debugPrintf("Error: eglMakeCurrent\n");
        exit(0);
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    debugPrintf("EGL Version          : %d.%d\n", major, minor);
    debugPrintf("GL Vendor            : %s\n", vendor);
    debugPrintf("GL Renderer          : %s\n", renderer);
    debugPrintf("GL Version (string)  : %s\n", version);
    debugPrintf("GLSL Version         : %s\n", glslVersion);

    return 0;
}

/*
 * EGL Hooks
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
    // init EGL
    debugPrintf("eglGetDisplay_hook triggered, init EGL");
    init_egl();

    return egl_display;
}

/*
 * OpenGL Hooks
 */

void glGetShaderiv_hook(GLuint handle, GLenum pname, GLint *params) {
    debugPrintf("glGetShaderiv_hook %d %d\n", handle, pname);
    glGetShaderiv(handle, pname, params);
    if (!*params) {
        debugPrintf("[WARN] shader compile failed %d %d\n", *params, glGetError());
    }
}

GLint glGetUniformLocation_hook(GLuint prog, const GLchar *name) {
    if (!strcmp(name, "texture")) {
        name = "texture0";
    }
    debugPrintf("glGetUniformLocation(%d, %s)\n", prog, name);
    return glGetUniformLocation(prog, name);
}

void glTexImage2D_hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
                       GLenum format, GLenum type, const void *data) {
    debugPrintf("glTexImage2D(%x %x %x w=%d h=%d %x %x %x d=%x)\n",
                target, level, internalformat, width, height, border, format, type, data);
    glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}

void glViewport_hook(GLint x, GLint y, GLsizei width, GLsizei height) {
    debugPrintf("glViewport(%d, %d, %d, %d)\n", x, y, width, height);
    glViewport(x, y, width, height);
}

void glBindTexture_hook(GLenum target, GLuint texture) {
    debugPrintf("glBindTexture(%x, %d)\n", target, texture);
    glBindTexture(target, texture);
}

void glDrawArrays_hook(GLenum mode, GLint first, GLsizei count) {
    debugPrintf("glDrawArrays(%x, %d, %d)\n", mode, first, count);
    glDrawArrays(mode, first, count);
}

void glGenTextures_hook(GLsizei n, GLuint *textures) {
    glGenTextures(n, textures);
    debugPrintf("glGenTextures(%d, %x) = %d\n", n, textures, *textures);
}

void glCompileShader_hook(GLuint shader) {
    debugPrintf("glCompileShader(%d)\n", shader);
    glCompileShader(shader);
}
