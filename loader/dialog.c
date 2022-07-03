/* dialog.c -- common dialog for error messages
 *
 * Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>
 * Copyright (C) 2022 KAAAsS
 *
 * This file includes the source code of SDL2, see https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt
 * for the original license.
 */

#include <psp2/kernel/processmgr.h>
#include <psp2/message_dialog.h>
#include <stdio.h>
#include <stdarg.h>
#include <psp2/display.h>
#include <stdbool.h>
#include <psp2/gxm.h>
#include <psp2/types.h>
#include <psp2/kernel/sysmem.h>

#include "main.h"
#include "dialog.h"

#define VITA_GXM_PENDING_SWAPS    2
#define VITA_GXM_BUFFERS          3
#define VITA_GXM_SCREEN_WIDTH     960
#define VITA_GXM_SCREEN_HEIGHT    544
#define VITA_GXM_SCREEN_STRIDE    960
#define VITA_GXM_PIXEL_FORMAT    SCE_DISPLAY_PIXELFORMAT_A8B8G8R8

typedef struct {
    void *address;
    uint8_t wait_vblank;
} VITA_GXM_DisplayData;

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

static void *vita_mem_alloc(SceKernelMemBlockType type, unsigned int size, unsigned int attribs, SceUID *uid) {
    void *mem;

    size = ALIGN(size, 256 * 1024);
    *uid = sceKernelAllocMemBlock("gpu_mem", type, size, NULL);

    if (*uid < 0)
        return NULL;

    if (sceKernelGetMemBlockBase(*uid, &mem) < 0)
        return NULL;

    if (sceGxmMapMemory(mem, size, attribs) < 0)
        return NULL;

    return mem;
}

static void vita_mem_free(SceUID uid) {
    void *mem = NULL;
    if (sceKernelGetMemBlockBase(uid, &mem) < 0)
        return;
    sceGxmUnmapMemory(mem);
    sceKernelFreeMemBlock(uid);
}

static void display_callback(const void *callback_data) {
    SceDisplayFrameBuf framebuf;
    const VITA_GXM_DisplayData *display_data = (const VITA_GXM_DisplayData *) callback_data;

    memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
    framebuf.size = sizeof(SceDisplayFrameBuf);
    framebuf.base = display_data->address;
    framebuf.pitch = VITA_GXM_SCREEN_STRIDE;
    framebuf.pixelformat = VITA_GXM_PIXEL_FORMAT;
    framebuf.width = VITA_GXM_SCREEN_WIDTH;
    framebuf.height = VITA_GXM_SCREEN_HEIGHT;
    sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);

    if (display_data->wait_vblank) {
        sceDisplayWaitVblankStart();
    }
}


static unsigned int back_buffer_index_for_common_dialog = 0;
static unsigned int front_buffer_index_for_common_dialog = 0;
struct {
    VITA_GXM_DisplayData displayData;
    SceGxmSyncObject *sync;
    SceGxmColorSurface surf;
    SceUID uid;
} buffer_for_common_dialog[VITA_GXM_BUFFERS];

void gxm_minimal_init_for_common_dialog(void) {
    SceGxmInitializeParams initializeParams;
    memset(&initializeParams, 0, sizeof(SceGxmInitializeParams));
    initializeParams.flags = 0;
    initializeParams.displayQueueMaxPendingCount = VITA_GXM_PENDING_SWAPS;
    initializeParams.displayQueueCallback = display_callback;
    initializeParams.displayQueueCallbackDataSize = sizeof(VITA_GXM_DisplayData);
    initializeParams.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;
    sceGxmInitialize(&initializeParams);
}

void gxm_minimal_term_for_common_dialog(void) {
    sceGxmTerminate();
}

void gxm_init_for_common_dialog(void) {
    for (int i = 0; i < VITA_GXM_BUFFERS; i += 1) {
        buffer_for_common_dialog[i].displayData.wait_vblank = true;
        buffer_for_common_dialog[i].displayData.address = vita_mem_alloc(
                SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
                4 * VITA_GXM_SCREEN_STRIDE * VITA_GXM_SCREEN_HEIGHT,
                SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
                &buffer_for_common_dialog[i].uid);
        sceGxmColorSurfaceInit(
                &buffer_for_common_dialog[i].surf,
                (SceGxmColorFormat) VITA_GXM_PIXEL_FORMAT,
                SCE_GXM_COLOR_SURFACE_LINEAR,
                SCE_GXM_COLOR_SURFACE_SCALE_NONE,
                SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
                VITA_GXM_SCREEN_WIDTH,
                VITA_GXM_SCREEN_HEIGHT,
                VITA_GXM_SCREEN_STRIDE,
                buffer_for_common_dialog[i].displayData.address
        );
        sceGxmSyncObjectCreate(&buffer_for_common_dialog[i].sync);
    }
    sceGxmDisplayQueueFinish();
}

void gxm_swap_for_common_dialog(void) {
    SceCommonDialogUpdateParam updateParam;
    memset(&updateParam, 0, sizeof(SceCommonDialogUpdateParam));
    updateParam.renderTarget.colorFormat = (SceGxmColorFormat) VITA_GXM_PIXEL_FORMAT;
    updateParam.renderTarget.surfaceType = SCE_GXM_COLOR_SURFACE_LINEAR;
    updateParam.renderTarget.width = VITA_GXM_SCREEN_WIDTH;
    updateParam.renderTarget.height = VITA_GXM_SCREEN_HEIGHT;
    updateParam.renderTarget.strideInPixels = VITA_GXM_SCREEN_STRIDE;

    updateParam.renderTarget.colorSurfaceData = buffer_for_common_dialog[back_buffer_index_for_common_dialog].displayData.address;

    updateParam.displaySyncObject = buffer_for_common_dialog[back_buffer_index_for_common_dialog].sync;
    memset(buffer_for_common_dialog[back_buffer_index_for_common_dialog].displayData.address, 0,
           4 * VITA_GXM_SCREEN_STRIDE * VITA_GXM_SCREEN_HEIGHT);
    sceCommonDialogUpdate(&updateParam);

    sceGxmDisplayQueueAddEntry(buffer_for_common_dialog[front_buffer_index_for_common_dialog].sync,
                               buffer_for_common_dialog[back_buffer_index_for_common_dialog].sync,
                               &buffer_for_common_dialog[back_buffer_index_for_common_dialog].displayData);
    front_buffer_index_for_common_dialog = back_buffer_index_for_common_dialog;
    back_buffer_index_for_common_dialog = (back_buffer_index_for_common_dialog + 1) % VITA_GXM_BUFFERS;
}

void gxm_term_for_common_dialog(void) {
    sceGxmDisplayQueueFinish();
    for (int i = 0; i < VITA_GXM_BUFFERS; i += 1) {
        vita_mem_free(buffer_for_common_dialog[i].uid);
        sceGxmSyncObjectDestroy(buffer_for_common_dialog[i].sync);
    }
}

void fatal_error(const char *fmt, ...) {
    va_list list;
    char string[512];

    va_start(list, fmt);
    vsnprintf(string, sizeof(string), fmt, list);
    va_end(list);

    debugPrintf("[FatalError]: %s\n", string);

    SceMsgDialogParam param;
    SceMsgDialogUserMessageParam msgParam;
    SceMsgDialogButtonsParam buttonParam;
    SceDisplayFrameBuf dispparam;

    SceMsgDialogResult dialog_result;
    SceCommonDialogErrorCode init_result;
    bool setup_minimal_gxm = false;

    memset(&param, 0, sizeof(param));
    sceMsgDialogParamInit(&param);
    param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;

    memset(&msgParam, 0, sizeof(msgParam));

    msgParam.msg = (const SceChar8 *) string;
    memset(&buttonParam, 0, sizeof(buttonParam));

    msgParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_OK;
    param.userMsgParam = &msgParam;

    dispparam.size = sizeof(dispparam);

    init_result = sceMsgDialogInit(&param);

    // Setup display if it hasn't been initialized before
    if (init_result == SCE_COMMON_DIALOG_ERROR_GXM_IS_UNINITIALIZED) {
        gxm_minimal_init_for_common_dialog();
        init_result = sceMsgDialogInit(&param);
        setup_minimal_gxm = true;
    }

    gxm_init_for_common_dialog();

    if (init_result >= 0) {
        while (sceMsgDialogGetStatus() == SCE_COMMON_DIALOG_STATUS_RUNNING) {
            gxm_swap_for_common_dialog();
        }
        sceMsgDialogTerm();
    }

    gxm_term_for_common_dialog();

    if (setup_minimal_gxm) {
        gxm_minimal_term_for_common_dialog();
    }

    sceKernelExitProcess(0);
    while (1);
}
