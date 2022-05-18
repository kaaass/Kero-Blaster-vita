#ifndef VITA_JNI_H
#define VITA_JNI_H

typedef struct ANativeActivityCallbacks {
    void (*onStart)(void* activity);
    void (*onResume)(void* activity);
    void* (*onSaveInstanceState)(void* activity, size_t* outSize);
    void (*onPause)(void* activity);
    void (*onStop)(void* activity);
    void (*onDestroy)(void* activity);
    void (*onWindowFocusChanged)(void* activity, int hasFocus);
    void (*onNativeWindowCreated)(void* activity, void* window);
    void (*onNativeWindowResized)(void* activity, void* window);
    void (*onNativeWindowRedrawNeeded)(void* activity, void* window);
    void (*onNativeWindowDestroyed)(void* activity, void* window);
    void (*onInputQueueCreated)(void* activity, void* queue);
    void (*onInputQueueDestroyed)(void* activity, void* queue);
    void (*onContentRectChanged)(void* activity, const void* rect);
    void (*onConfigurationChanged)(void* activity);
    void (*onLowMemory)(void* activity);
} ANativeActivityCallbacks;

typedef struct ANativeActivity {
    struct ANativeActivityCallbacks* callbacks;
    void* vm;
    void* env;
    void* clazz;
    const char* internalDataPath;
    const char* externalDataPath;
    int32_t sdkVersion;
    void* instance;
    void* assetManager;
    const char* obbPath;
} ANativeActivity;

extern char fake_vm[];
extern char fake_env[];
extern ANativeActivity fake_activity;
extern ANativeActivityCallbacks fake_callbacks;

void jni_load();

#endif //VITA_JNI_H
