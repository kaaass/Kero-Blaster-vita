#ifndef __CONFIG_H__
#define __CONFIG_H__

#define DEBUG

#define LOAD_ADDRESS 0x98000000

#define MEMORY_NEWLIB_MB 240
#define MEMORY_VITAGL_THRESHOLD_MB 12

#define DATA_PATH "ux0:data/kero"
#define SO_PATH DATA_PATH "/" "libpxgame.so"
#define ASSETS_PATH DATA_PATH "/assets/"

#define SCREEN_W 960
#define SCREEN_H 544

#define TRACE_DYN_SYMBOL
// #define SHOW_RESOLVE_INFO

#endif
