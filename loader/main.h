#ifndef __MAIN_H__
#define __MAIN_H__

#include <psp2/touch.h>
#include "config.h"

#if defined(__cplusplus)
extern "C" {
#endif

struct so_module;
extern struct so_module kero_mod;

int debugPrintf(char *text, ...);

int ret0();

int sceKernelChangeThreadCpuAffinityMask(SceUID thid, int cpuAffinityMask);

SceUID _vshKernelSearchModuleByName(const char *, int *);

extern SceTouchPanelInfo panelInfoFront, panelInfoBack;

#if defined(__cplusplus)
}
#endif

#endif
