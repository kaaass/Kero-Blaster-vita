#ifndef KERO_BLASTER_VITA_GAME_INFO_H
#define KERO_BLASTER_VITA_GAME_INFO_H

#include <stdbool.h>

typedef enum support_game {
    KERO_BLASTER,
    PINK_HOUR,
    PINK_HEAVEN,
    MAX_SUPPORT_GAME
} support_game;

extern support_game current_game;

/*
 * Info macros
 */

#define SO_HASH (info_so_hash[current_game])
#define DATA_PATH (info_data_path[current_game])
#define SO_PATH (info_so_path())
#define ASSETS_PATH (info_assets_path())
#define SUPPORT_JOYSTICK (info_support_joystick[current_game])
#define FUNC_ADDR(sym) (address_##sym[current_game])

/*
 * Check functions
 */

void check_game();

void check_so_hash();

/*
 * Internal data
 */

extern uint64_t info_so_hash[MAX_SUPPORT_GAME];
extern char *info_data_path[MAX_SUPPORT_GAME];
char *info_so_path();
char *info_assets_path();
extern bool info_support_joystick[MAX_SUPPORT_GAME];

extern uintptr_t address_pxaAudioStream_open[MAX_SUPPORT_GAME];
extern uintptr_t address_onInputEvent[MAX_SUPPORT_GAME];
extern uintptr_t address_onAppCmd[MAX_SUPPORT_GAME];
extern uintptr_t address_write_last_error[MAX_SUPPORT_GAME];
extern uintptr_t address_mutex_lock[MAX_SUPPORT_GAME];
extern uintptr_t address_mutex_unlock[MAX_SUPPORT_GAME];

#endif //KERO_BLASTER_VITA_GAME_INFO_H
