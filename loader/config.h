#ifndef __CONFIG_H__
#define __CONFIG_H__

#define LOAD_ADDRESS 0x98000000

#define MEMORY_NEWLIB_MB 100

#define DATA_PATH "ux0:data/kero"
#define SO_PATH DATA_PATH "/" "libpxgame.so"
#define ASSETS_PATH DATA_PATH "/assets/"

#define EVENT_BUFFER_LEN 50

// #define TRACE_DYN_SYMBOL
// #define SHOW_RESOLVE_INFO

#endif
