#include <EGL/eglplatform.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#ifndef KERO_BLASTER_VITA_OPENGL_H
#define KERO_BLASTER_VITA_OPENGL_H

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

/*
 * OpenGL Hooks
 */

void glGetShaderiv_hook(GLuint handle, GLenum pname, GLint *params);
GLint glGetUniformLocation_hook(GLuint prog, const GLchar *name);
void glTexImage2D_hook(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
                       GLenum format, GLenum type, const void *data);
void glViewport_hook(GLint x, GLint y, GLsizei width, GLsizei height);
void glBindTexture_hook(GLenum target, GLuint texture);
void glDrawArrays_hook(GLenum mode, GLint first, GLsizei count);
void glGenTextures_hook(GLsizei n, GLuint* textures);

#endif //KERO_BLASTER_VITA_OPENGL_H
