/* jni.c -- JNI hooks
 *
 * Copyright (C) 2022 KAAAsS, Andy Nguyen
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "jni.h"
#include "dialog.h"
#include "so_util.h"
#include "game_info.h"

char fake_vm[0x1000];
char fake_env[0x1000];
ANativeActivity fake_activity;
ANativeActivityCallbacks fake_callbacks;

typedef enum {
    JNI_METHOD_UNKNOWN = 0,
    JNI_METHOD_messageBox,
} jni_method_id_t;

typedef struct {
    char *name;
    jni_method_id_t id;
} jni_method_mapping_t;

typedef enum {
    JNI_FIELD_UNKNOWN = 0,
    JNI_FIELD_versionName,
} jni_field_id_t;

typedef struct {
    char *name;
    jni_field_id_t id;
} jni_field_mapping_t;

static jni_method_mapping_t method_mappings[] = {
        {"messageBox", JNI_METHOD_messageBox},
};

static jni_field_mapping_t field_mappings[] = {
        {"versionName", JNI_FIELD_versionName},
};

jni_method_id_t jni_get_method_by_name(const char *name) {
    for (int i = 0; i < sizeof(method_mappings) / sizeof(jni_method_mapping_t); i++) {
        if (strcmp(name, method_mappings[i].name) == 0) {
            return method_mappings[i].id;
        }
    }
    return JNI_METHOD_UNKNOWN;
}

jni_field_id_t jni_get_field_by_name(const char *name) {
    for (int i = 0; i < sizeof(field_mappings) / sizeof(jni_field_mapping_t); i++) {
        if (strcmp(name, field_mappings[i].name) == 0) {
            return field_mappings[i].id;
        }
    }
    return JNI_FIELD_UNKNOWN;
}

int GetMethodID(void *env, void *class, const char *name, const char *sig) {
    debugPrintf("GetMethodID: %s\n", name);
    return jni_get_method_by_name(name);
}

int GetStaticMethodID(void *env, void *class, const char *name, const char *sig) {
    debugPrintf("GetStaticMethodID: %s\n", name);
    return jni_get_method_by_name(name);
}

int GetFieldID(void *env, void *class, char *name, char *type) {
    debugPrintf("GetFieldID: %s\n", name);
    return jni_get_field_by_name(name);
}

void CallVoidMethodV(void *env, void *obj, int methodID, uintptr_t *args) {
    debugPrintf("CallVoidMethodV: methodID = %d\n", methodID);
}

void *GetObjectField(void *env, void *obj, int fieldID) {
    debugPrintf("GetObjectField: fieldID = %d\n", fieldID);
    switch (fieldID) {
        case JNI_FIELD_versionName:
#if defined(VITA_VERSION)
        {
            static char version[256];
            version[0] = '\0';
            strcpy(version, GAME_VERSION);
            strcat(version, "_" VITA_VERSION);
            return (void *) version;
        }
#else
            return (void *) GAME_VERSION;
#endif
        default:
            return "";
    }
}

void CallObjectMethodV(void *env, void *obj, int methodID, uintptr_t *args) {
    debugPrintf("CallObjectMethodV: methodID = %d\n", methodID);
}

bool CallBooleanMethodV(void *env, void *obj, int methodID, uintptr_t *args) {
    debugPrintf("CallBooleanMethodV: methodID = %d\n", methodID);
    return false;
}

int CallIntMethodV(void *env, void *obj, int methodID, uintptr_t *args) {
    debugPrintf("CallIntMethodV: methodID = %d\n", methodID);
    switch (methodID) {
        case JNI_METHOD_messageBox: {
            char *title = *(char **) &args[0];
            char *content = *(char **) &args[1];
            bool show_cancel = *(bool *) &args[2];
            fatal_error("[%s]\n%s", title, content);
            return 1;
        }
    }
    return 0;
}

void CallStaticVoidMethodV(void *env, void *obj, int methodID, uintptr_t *args) {
    debugPrintf("CallStaticVoidMethodV: methodID = %d\n", methodID);
}

int CallStaticBooleanMethodV(void *env, void *obj, int methodID, uintptr_t *args) {
    debugPrintf("CallStaticBooleanMethodV: methodID = %d\n", methodID);
    return 0;
}

void *FindClass(void *env, const char *name) {
    debugPrintf("FindClass: %s\n", name);
    return (void *) 0x41414141;
}

void *NewGlobalRef(void) {
    return (void *) 0x42424242;
}

void *NewObjectV(void *env, void *clazz, int methodID, uintptr_t args) {
    return (void *) 0x43434343;
}

void *GetObjectClass(void *env, void *obj) {
    return (void *) 0x44444444;
}

char *NewStringUTF(void *env, char *bytes) {
    return bytes;
}

char *GetStringUTFChars(void *env, char *string, int *isCopy) {
    return string;
}

void *GetDirectBufferAddress(void *env, void *obj) {
    return 0;
}

int GetJavaVM(void *env, void **vm) {
    *vm = fake_vm;
    return 0;
}

int GetEnv(void *vm, void **env, int r2) {
    *env = fake_env;
    return 0;
}

void init_jni_env() {
    memset(fake_vm, 'A', sizeof(fake_vm));
    *(uintptr_t *) (fake_vm + 0x00) = (uintptr_t) fake_vm; // just point to itself...
    *(uintptr_t *) (fake_vm + 0x10) = (uintptr_t) ret0;
    *(uintptr_t *) (fake_vm + 0x18) = (uintptr_t) GetEnv;

    memset(fake_env, 'A', sizeof(fake_env));
    *(uintptr_t *) (fake_env + 0x00) = (uintptr_t) fake_env; // just point to itself...
    *(uintptr_t *) (fake_env + 0x18) = (uintptr_t) FindClass;
    *(uintptr_t *) (fake_env + 0x3C) = (uintptr_t) ret0; // ExceptionOccurred
    *(uintptr_t *) (fake_env + 0x54) = (uintptr_t) NewGlobalRef;
    *(uintptr_t *) (fake_env + 0x5C) = (uintptr_t) ret0; // DeleteLocalRef
    *(uintptr_t *) (fake_env + 0x58) = (uintptr_t) ret0; // DeleteGlobalRef
    *(uintptr_t *) (fake_env + 0x74) = (uintptr_t) NewObjectV;
    *(uintptr_t *) (fake_env + 0x7C) = (uintptr_t) GetObjectClass;
    *(uintptr_t *) (fake_env + 0x84) = (uintptr_t) GetMethodID;
    *(uintptr_t *) (fake_env + 0x8C) = (uintptr_t) CallObjectMethodV;
    *(uintptr_t *) (fake_env + 0x98) = (uintptr_t) CallBooleanMethodV;
    *(uintptr_t *) (fake_env + 0xC8) = (uintptr_t) CallIntMethodV;
    *(uintptr_t *) (fake_env + 0xF8) = (uintptr_t) CallVoidMethodV;
    *(uintptr_t *) (fake_env + 0x178) = (uintptr_t) GetFieldID;
    *(uintptr_t *) (fake_env + 0x17C) = (uintptr_t) GetObjectField;
    *(uintptr_t *) (fake_env + 0x1C4) = (uintptr_t) GetStaticMethodID;
    *(uintptr_t *) (fake_env + 0x1D8) = (uintptr_t) CallStaticBooleanMethodV;
    *(uintptr_t *) (fake_env + 0x238) = (uintptr_t) CallStaticVoidMethodV;
    *(uintptr_t *) (fake_env + 0x29C) = (uintptr_t) NewStringUTF;
    *(uintptr_t *) (fake_env + 0x2A4) = (uintptr_t) GetStringUTFChars;
    *(uintptr_t *) (fake_env + 0x2A8) = (uintptr_t) ret0; // ReleaseStringUTFChars
    *(uintptr_t *) (fake_env + 0x36C) = (uintptr_t) GetJavaVM;
    *(uintptr_t *) (fake_env + 0x398) = (uintptr_t) GetDirectBufferAddress;
}

void init_native_activity() {
    memset(&fake_activity, 0, sizeof(ANativeActivity));
    memset(&fake_callbacks, 0, sizeof(ANativeActivityCallbacks));

    fake_activity.callbacks = &fake_callbacks;
    fake_activity.vm = &fake_vm;
    fake_activity.env = &fake_env;
    fake_activity.clazz = (void *) 0x41414141;
    fake_activity.internalDataPath = DATA_PATH;
    fake_activity.externalDataPath = DATA_PATH;
    fake_activity.sdkVersion = 114514;
    fake_activity.assetManager = (void *) 0x41414141;
    fake_activity.obbPath = DATA_PATH;
}

void jni_load() {

    init_jni_env();
    init_native_activity();

    void (*ANativeActivity_onCreate)(ANativeActivity *, void *, size_t) =
    (void *) so_symbol(&kero_mod, "ANativeActivity_onCreate");
    ANativeActivity_onCreate(&fake_activity, NULL, 0);
}
