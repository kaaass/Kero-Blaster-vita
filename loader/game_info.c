/* game_info.c -- Game info (data path, function address) and game file check
 *
 * Copyright (C) 2022 KAAAsS
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <psp2/apputil.h>
#include <string.h>
#include <psp2/io/stat.h>
#include "game_info.h"
#include "main.h"
#include "dialog.h"

/*
 * Game info
 */

uint64_t info_so_hash[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = 0x019745ff,
        [PINK_HEAVEN]  = 0x0db776df,
        [PINK_HOUR]    = 0x0c24916c,
};

char *info_data_path[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = "ux0:data/kero",
        [PINK_HEAVEN]  = "ux0:data/kero/pink_heaven",
        [PINK_HOUR]    = "ux0:data/kero/pink_hour",
};

char *info_so_path() {
    static char path_buf[512];
    path_buf[0] = '\0';
    strcpy(path_buf, DATA_PATH);
    strcat(path_buf, "/libpxgame.so");
    return path_buf;
}

char *info_assets_path() {
    static char path_buf[512];
    path_buf[0] = '\0';
    strcpy(path_buf, DATA_PATH);
    strcat(path_buf, "/assets/");
    return path_buf;
}

bool info_support_joystick[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = true,
        [PINK_HEAVEN]  = true,
        [PINK_HOUR]    = false,
};

char *info_game_version[MAX_SUPPORT_GAME] = {
#if defined(MAIN_VERSION)
        [KERO_BLASTER] = MAIN_VERSION,
#else
        [KERO_BLASTER] = "unknown",
#endif
        [PINK_HEAVEN]  = "1.6.1",
        [PINK_HOUR]    = "1.6.0",
};

uintptr_t address_pxaAudioStream_open[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = LOAD_ADDRESS + 0x1b3d0 + 1,
        [PINK_HEAVEN]  = LOAD_ADDRESS + 0x1b290 + 1,
        [PINK_HOUR]    = LOAD_ADDRESS + 0x19e60 + 1,
};

uintptr_t address_onInputEvent[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = LOAD_ADDRESS + 0x18e74 + 1,
        [PINK_HEAVEN]  = LOAD_ADDRESS + 0x18d30 + 1,
        [PINK_HOUR]    = LOAD_ADDRESS + 0x17f64 + 1,
};

uintptr_t address_onAppCmd[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = LOAD_ADDRESS + 0x18df0 + 1,
        [PINK_HEAVEN]  = LOAD_ADDRESS + 0x18cac + 1,
        [PINK_HOUR]    = LOAD_ADDRESS + 0x17ef4 + 1,
};

uintptr_t address_write_last_error[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = LOAD_ADDRESS + 0xb24c4 + 0x1,
        [PINK_HEAVEN]  = LOAD_ADDRESS + 0xb1a74 + 0x1,
        [PINK_HOUR]    = LOAD_ADDRESS + 0xb0324 + 0x1,
};

uintptr_t address_mutex_lock[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = LOAD_ADDRESS + 0xb4bf8 + 0x1,
        [PINK_HEAVEN]  = LOAD_ADDRESS + 0xb41a8 + 0x1,
        [PINK_HOUR]    = LOAD_ADDRESS + 0xb299c + 0x1,
};

uintptr_t address_mutex_unlock[MAX_SUPPORT_GAME] = {
        [KERO_BLASTER] = LOAD_ADDRESS + 0xb4c12 + 0x1,
        [PINK_HEAVEN]  = LOAD_ADDRESS + 0xb41c2 + 0x1,
        [PINK_HOUR]    = LOAD_ADDRESS + 0xb29b6 + 0x1,
};

/*
 * Check functions
 */

support_game current_game = KERO_BLASTER;

int file_exists(const char *path) {
    SceIoStat stat;
    return sceIoGetstat(path, &stat) >= 0;
}

void check_game() {
    // Init app util
    SceAppUtilInitParam initParam = {};
    SceAppUtilBootParam bootParam = {};
    sceAppUtilInit(&initParam, &bootParam);
    // Load event param
    SceAppUtilAppEventParam eventParam = {};
    sceAppUtilReceiveAppEvent(&eventParam);
    // Check which game to load
    if (eventParam.type == 0x05) {
        char buffer[2048];
        sceAppUtilAppEventParseLiveArea(&eventParam, buffer);
        if (strstr(buffer, "pink_hour")) {
            current_game = PINK_HOUR;
        } else if (strstr(buffer, "pink_heaven")) {
            current_game = PINK_HEAVEN;
        }
    }
    debugPrintf("Current game: %d\n", current_game);
    // Check game files
    if (!file_exists(DATA_PATH)) {
        fatal_error("Game files not found! Please put game resource to: %s.", DATA_PATH);
    }
}

void check_so_hash() {
    // Calc hash
    uint64_t hash = 0, g;
    uint64_t *data = (uint64_t *) LOAD_ADDRESS;
    for (int i = 0; i < 1024; i++) {
        hash = (hash << 4) + data[i];
        if ((g = (hash & 0xf0000000)) != 0)
            hash ^= g >> 24;
        hash &= 0x0fffffff;
    }
    // Check
    if (hash != SO_HASH) {
        debugPrintf("So hash = %016llx, expected = %016llx!\n", hash, SO_HASH);
        fatal_error("File (%s) hash mismatch! Please check the game version!", SO_PATH);
    }
}
