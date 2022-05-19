cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else ()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif ()
endif ()

project(Kero-Blaster_vita C CXX)
include("${VITASDK}/share/vita.cmake" REQUIRED)
set(VITA_APP_NAME "Kero Blaster")
set(VITA_TITLEID "KEROBLAST")
set(VITA_VERSION "01.00")
set(VITA_MKSFOEX_FLAGS "-d ATTRIBUTE2=12")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wl,-q,--wrap,memcpy,--wrap,memmove,--wrap,memset -D_GNU_SOURCE -Wall -O3 -mfloat-abi=softfp")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")

add_definitions(-D__psp2__)

add_executable(Kero-Blaster
        loader/main.c
        loader/dialog.c
        loader/so_util.c
        loader/sha1.c
        loader/jni.c
        loader/dyn_stub.c
        loader/opengl.c
        )

target_link_libraries(Kero-Blaster
        -Wl,--whole-archive pthread -Wl,--no-whole-archive
        m
        stdc++
        mathneon
        kubridge_stub
        SceAppMgr_stub
        SceAppUtil_stub
        SceAudio_stub
        SceAudioIn_stub
        SceCtrl_stub
        SceCommonDialog_stub
        SceDisplay_stub
        SceKernelDmacMgr_stub
        SceRazorHud_stub
        SceRazorCapture_stub
        SceFios2_stub
        SceLibcBridge_stub
        SceSysmodule_stub
        ScePower_stub
        SceTouch_stub
        SceVshBridge_stub
        libIMGEGL_stub_weak
        libgpu_es4_ext_stub_weak
        libGLESv2_stub_weak
        )

vita_create_self(eboot.bin Kero-Blaster UNSAFE)
vita_create_vpk(Kero-Blaster.vpk ${VITA_TITLEID} eboot.bin
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE ${CMAKE_SOURCE_DIR}/sce_sys sce_sys
        FILE ${CMAKE_SOURCE_DIR}/shader shader
        FILE ${CMAKE_SOURCE_DIR}/module module
        )

add_custom_target(launch
        COMMAND echo "screen on" | nc $ENV{PSVITAIP} 1338
        COMMAND echo "destroy" | nc $ENV{PSVITAIP} 1338
        COMMAND curl -T eboot.bin ftp://$ENV{PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/eboot.bin DEPENDS eboot.bin
        COMMAND echo "launch ${VITA_TITLEID}" | nc $ENV{PSVITAIP} 1338
        )
