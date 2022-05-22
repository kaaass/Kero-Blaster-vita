#define DYN_SYMBOL_DEFAULT(name) DYN_SYMBOL(name, name)
#define DYN_SYMBOL_SFP(name) DYN_SYMBOL(name, name##_sfp)
#define DYN_SYMBOL_DATA_DEFAULT(name) DYN_SYMBOL_DATA(name, name)

DYN_SYMBOL_DEFAULT(__android_log_print)
DYN_SYMBOL_DEFAULT(__android_log_write)
DYN_SYMBOL_DEFAULT(__cxa_allocate_exception)
DYN_SYMBOL_DEFAULT(__cxa_atexit)
DYN_SYMBOL_DEFAULT(__cxa_begin_catch)
DYN_SYMBOL_DEFAULT(__cxa_finalize)
DYN_SYMBOL_DEFAULT(__cxa_free_exception)
DYN_SYMBOL_DEFAULT(__cxa_guard_acquire)
DYN_SYMBOL_DEFAULT(__cxa_guard_release)
DYN_SYMBOL_DEFAULT(__cxa_throw)
DYN_SYMBOL_DATA_DEFAULT(__errno)
DYN_SYMBOL_DEFAULT(__gxx_personality_v0)
DYN_SYMBOL_DEFAULT(__memcpy_chk)
DYN_SYMBOL_DATA(__sF, __sF_fake)
DYN_SYMBOL_DEFAULT(__stack_chk_fail)
DYN_SYMBOL_DATA(__stack_chk_guard, __stack_chk_guard_fake)
DYN_SYMBOL_DEFAULT(__strcat_chk)
DYN_SYMBOL_DEFAULT(__strcpy_chk)
DYN_SYMBOL_DEFAULT(__strlen_chk)
DYN_SYMBOL_DEFAULT(__vsprintf_chk)
DYN_SYMBOL_DEFAULT(_ZdaPv)
DYN_SYMBOL_DEFAULT(_ZdlPv)
DYN_SYMBOL_DEFAULT(_Znaj)
DYN_SYMBOL_DEFAULT(_Znwj)
DYN_SYMBOL_DEFAULT(_ZNSt11logic_errorC2EPKc)
DYN_SYMBOL_DEFAULT(_ZNSt12length_errorD1Ev)
DYN_SYMBOL_DEFAULT(_ZSt9terminatev)
DYN_SYMBOL_DEFAULT(_ZTISt12length_error)
DYN_SYMBOL_DEFAULT(_ZTVSt12length_error)
DYN_SYMBOL_DATA_DEFAULT(_ZTVN10__cxxabiv117__class_type_infoE)
DYN_SYMBOL_DATA_DEFAULT(_ZTVN10__cxxabiv120__si_class_type_infoE)
DYN_SYMBOL_DATA_DEFAULT(_ZTVN10__cxxabiv121__vmi_class_type_infoE)
DYN_SYMBOL_DEFAULT(_ZNSt6__ndk16chrono12steady_clock3nowEv)
DYN_SYMBOL(_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6resizeEjc, _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6resizeEjc)
DYN_SYMBOL_DEFAULT(_ZNKSt6__ndk120__vector_base_commonILb1EE20__throw_length_errorEv)
DYN_SYMBOL(_ZNSt6__ndk112__next_primeEj, next_prime)
DYN_SYMBOL(_ZN4oboe18AudioStreamBuilder10openStreamERNSt6__ndk110shared_ptrINS_11AudioStreamEEE, ret0)
DYN_SYMBOL_DEFAULT(_ZNSt6__ndk15mutex4lockEv)
DYN_SYMBOL_DEFAULT(_ZNSt6__ndk15mutex6unlockEv)
DYN_SYMBOL_DEFAULT(_ZNSt6__ndk15mutex8try_lockEv)
DYN_SYMBOL_DEFAULT(_ZNSt6__ndk15mutexD1Ev)
DYN_SYMBOL_DEFAULT(_ZNSt6__ndk119__shared_weak_count14__release_weakEv)
DYN_SYMBOL_DEFAULT(AAsset_close)
DYN_SYMBOL_DEFAULT(AAsset_getBuffer)
DYN_SYMBOL_DEFAULT(AAsset_getLength)
DYN_SYMBOL_DEFAULT(AAssetManager_open)
DYN_SYMBOL_DEFAULT(abort)
DYN_SYMBOL_DEFAULT(AConfiguration_delete)
DYN_SYMBOL_DEFAULT(AConfiguration_fromAssetManager)
DYN_SYMBOL_DEFAULT(AConfiguration_getCountry)
DYN_SYMBOL_DEFAULT(AConfiguration_getLanguage)
DYN_SYMBOL_DEFAULT(AConfiguration_new)
DYN_SYMBOL_DEFAULT(AInputEvent_getType)
DYN_SYMBOL_DEFAULT(AInputQueue_attachLooper)
DYN_SYMBOL_DEFAULT(AInputQueue_detachLooper)
DYN_SYMBOL_DEFAULT(AInputQueue_finishEvent)
DYN_SYMBOL_DEFAULT(AInputQueue_getEvent)
DYN_SYMBOL_DEFAULT(AInputQueue_preDispatchEvent)
DYN_SYMBOL_DEFAULT(AKeyEvent_getKeyCode)
DYN_SYMBOL_DEFAULT(ALooper_addFd)
DYN_SYMBOL_DEFAULT(ALooper_pollAll)
DYN_SYMBOL_DEFAULT(ALooper_prepare)
DYN_SYMBOL_DEFAULT(AMotionEvent_getAction)
DYN_SYMBOL_DEFAULT(AMotionEvent_getPointerCount)
DYN_SYMBOL_DEFAULT(AMotionEvent_getPointerId)
DYN_SYMBOL_DEFAULT(AMotionEvent_getX)
DYN_SYMBOL_DEFAULT(AMotionEvent_getY)
DYN_SYMBOL_DEFAULT(ANativeWindow_setBuffersGeometry)
DYN_SYMBOL_SFP(atan)
DYN_SYMBOL_DEFAULT(atoi)
DYN_SYMBOL_SFP(ceil)
DYN_SYMBOL_SFP(ceilf)
DYN_SYMBOL_DEFAULT(clock_gettime)
DYN_SYMBOL_DEFAULT(close)
DYN_SYMBOL_SFP(cosf)
DYN_SYMBOL_SFP(difftime)
DYN_SYMBOL_DEFAULT(dlclose)
DYN_SYMBOL_DEFAULT(dlopen)
DYN_SYMBOL_DEFAULT(dlsym)
DYN_SYMBOL_DEFAULT(dl_unwind_find_exidx)
DYN_SYMBOL(eglChooseConfig, ret1)
DYN_SYMBOL(eglCreateContext, eglCreateContext_hook)
DYN_SYMBOL(eglCreateWindowSurface, eglCreateWindowSurface_hook)
DYN_SYMBOL(eglDestroySurface, ret1)
DYN_SYMBOL(eglGetConfigAttrib, ret1)
DYN_SYMBOL(eglGetDisplay, eglGetDisplay_hook)
DYN_SYMBOL(eglInitialize, ret1)
DYN_SYMBOL(eglMakeCurrent, ret1)
DYN_SYMBOL_DEFAULT(eglQuerySurface)
DYN_SYMBOL_DEFAULT(eglSwapBuffers)
DYN_SYMBOL_DEFAULT(exit)
DYN_SYMBOL_DEFAULT(fclose)
DYN_SYMBOL_DEFAULT(feof)
DYN_SYMBOL_DEFAULT(ferror)
DYN_SYMBOL_DEFAULT(fflush)
DYN_SYMBOL_DEFAULT(fgetc)
DYN_SYMBOL_DEFAULT(fgetpos)
DYN_SYMBOL_DEFAULT(fgets)
DYN_SYMBOL_DEFAULT(fopen)
DYN_SYMBOL_DEFAULT(fprintf)
DYN_SYMBOL_DEFAULT(fputc)
DYN_SYMBOL_DEFAULT(fread)
DYN_SYMBOL_DEFAULT(free)
DYN_SYMBOL_DEFAULT(fseek)
DYN_SYMBOL_DEFAULT(ftell)
DYN_SYMBOL_DEFAULT(fwrite)
DYN_SYMBOL_DEFAULT(glAttachShader)
DYN_SYMBOL_DEFAULT(glBindBuffer)
DYN_SYMBOL_DEFAULT(glBindFramebuffer)
DYN_SYMBOL_DEFAULT(glBindTexture)
DYN_SYMBOL_DEFAULT(glBlendEquation)
DYN_SYMBOL_DEFAULT(glBlendFunc)
DYN_SYMBOL_DEFAULT(glBufferData)
DYN_SYMBOL_DEFAULT(glClear)
DYN_SYMBOL_SFP(glClearColor)
DYN_SYMBOL_DEFAULT(glCompileShader)
DYN_SYMBOL_DEFAULT(glCreateProgram)
DYN_SYMBOL_DEFAULT(glCreateShader)
DYN_SYMBOL_DEFAULT(glDeleteProgram)
DYN_SYMBOL_DEFAULT(glDeleteShader)
DYN_SYMBOL_DEFAULT(glDeleteTextures)
DYN_SYMBOL_DEFAULT(glDisable)
DYN_SYMBOL_DEFAULT(glDisableVertexAttribArray)
DYN_SYMBOL_DEFAULT(glDrawArrays)
DYN_SYMBOL_DEFAULT(glEnable)
DYN_SYMBOL_DEFAULT(glEnableVertexAttribArray)
DYN_SYMBOL_DEFAULT(glFramebufferTexture2D)
DYN_SYMBOL_DEFAULT(glGenBuffers)
DYN_SYMBOL_DEFAULT(glGenFramebuffers)
DYN_SYMBOL_DEFAULT(glGenTextures)
DYN_SYMBOL_DEFAULT(glGetAttribLocation)
DYN_SYMBOL_DEFAULT(glGetError)
DYN_SYMBOL_DEFAULT(glGetShaderInfoLog)
DYN_SYMBOL_DEFAULT(glGetShaderiv)
DYN_SYMBOL_DEFAULT(glGetUniformLocation)
DYN_SYMBOL_DEFAULT(glLinkProgram)
DYN_SYMBOL_DEFAULT(glShaderSource)
DYN_SYMBOL_DEFAULT(glTexImage2D)
DYN_SYMBOL_SFP(glTexParameterf)
DYN_SYMBOL_DEFAULT(glUniform1i)
DYN_SYMBOL_DEFAULT(glUniformMatrix4fv)
DYN_SYMBOL_DEFAULT(glUseProgram)
DYN_SYMBOL_DEFAULT(glVertexAttribPointer)
DYN_SYMBOL_DEFAULT(glViewport)
DYN_SYMBOL_DEFAULT(localtime)
DYN_SYMBOL_DEFAULT(localtime_r)
DYN_SYMBOL_DEFAULT(lrand48)
DYN_SYMBOL_DEFAULT(malloc)
DYN_SYMBOL(memcmp, sceClibMemcmp)
DYN_SYMBOL(memcpy, sceClibMemcpy)
DYN_SYMBOL(memset, sceClibMemset)
DYN_SYMBOL_DEFAULT(mkdir)
DYN_SYMBOL_DEFAULT(mktime)
DYN_SYMBOL_DEFAULT(pipe)
DYN_SYMBOL_SFP(powf)
DYN_SYMBOL(pthread_attr_init, ret0)
DYN_SYMBOL(pthread_attr_setdetachstate, ret0)
DYN_SYMBOL(pthread_cond_broadcast, pthread_cond_broadcast_fake)
DYN_SYMBOL(pthread_cond_destroy, pthread_cond_destroy_fake)
DYN_SYMBOL(pthread_cond_init, pthread_cond_init_fake)
DYN_SYMBOL(pthread_cond_wait, pthread_cond_wait_fake)
DYN_SYMBOL(pthread_create, pthread_create_fake)
DYN_SYMBOL_DEFAULT(pthread_exit)
DYN_SYMBOL_DEFAULT(pthread_join)
DYN_SYMBOL(pthread_mutex_destroy, pthread_mutex_destroy_fake)
DYN_SYMBOL(pthread_mutex_init, pthread_mutex_init_fake)
DYN_SYMBOL(pthread_mutex_lock, pthread_mutex_lock_fake)
DYN_SYMBOL(pthread_mutex_trylock, pthread_mutex_trylock_fake)
DYN_SYMBOL(pthread_mutex_unlock, pthread_mutex_unlock_fake)
DYN_SYMBOL_DEFAULT(read)
DYN_SYMBOL_DEFAULT(realloc)
DYN_SYMBOL_DEFAULT(remove)
DYN_SYMBOL_SFP(sin)
DYN_SYMBOL_SFP(sincos)
DYN_SYMBOL_SFP(sincosf)
DYN_SYMBOL_SFP(sinf)
DYN_SYMBOL_DEFAULT(strcat)
DYN_SYMBOL(strcmp, sceClibStrcmp)
DYN_SYMBOL_DEFAULT(strcpy)
DYN_SYMBOL_DEFAULT(strerror)
DYN_SYMBOL_DEFAULT(strlen)
DYN_SYMBOL_SFP(strtod)
DYN_SYMBOL_DEFAULT(time)
DYN_SYMBOL_DEFAULT(ungetc)
DYN_SYMBOL_DEFAULT(usleep)
DYN_SYMBOL_DEFAULT(vsnprintf)
DYN_SYMBOL_DEFAULT(vsprintf)
DYN_SYMBOL_DEFAULT(write)
