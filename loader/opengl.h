#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef KERO_BLASTER_VITA_OPENGL_H
#define KERO_BLASTER_VITA_OPENGL_H

extern int screen_width;
extern int screen_height;

/*
 * Init
 */

void init_pvr_psp2();
int init_egl();

/*
 * EGL Hooks
 */

EGLContext eglCreateContext_hook(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
EGLSurface eglCreateWindowSurface_hook(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);
EGLDisplay eglGetDisplay_hook(EGLNativeDisplayType display_id);

#endif //KERO_BLASTER_VITA_OPENGL_H
