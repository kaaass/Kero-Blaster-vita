
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "so_util.h"
#include "inline_hook.h"
#include "sfp.h"

int fake_load_se(void *se_ctx, char *name, int slot) {
    debugPrintf("fake_load_se(ctx = %x, name = '%s', slot = %d)\n", se_ctx, name, slot);
    void **slots = se_ctx + 0x20;
    char *slot_names = *slots + 0x9 + slot * 56;
    strcpy(slot_names, name);
    return 1;
}

int fake_play_se_by_slot(int slot) {
    debugPrintf("fake_play_se_by_slot(%d)\n", slot);
    return 0;
}

int fake_play_se_by_slot_2(int slot) {
    debugPrintf("fake_play_se_by_slot_2(%d)\n", slot);
    return 0;
}

int fake_play_se_by_slot_3(int slot) {
    debugPrintf("fake_play_se_by_slot_3(%d)\n", slot);
    return 0;
}

int (*init_bgm)(void *ctx, int a2, int sample_rate, int bits, float a5, float a6);
int (*bgm_check_cannot_load)(void *ctx);
int (*bgm_update_load_info)(void *ctx);
bool (*bgm_set_by_name)(void *ctx, char *name);
bool (*bgm_set_play_status)(void *ctx, sfp_float a2, bool repeat);
bool (*bgm_set_play_status2)(void *ctx, sfp_float a2);
bool (*bgm_set_unknows_status)(void *ctx, sfp_float a2);

int fake_init_bgm(void *ctx, int a2, int sample_rate, int bits, float a5, float a6) {
    debugPrintf("fake_init_bgm(%x, ...)\n", ctx);
    int ret = init_bgm(ctx, a2, sample_rate, bits, a5, a6);
    debugPrintf("fake_init_bgm() = %d\n", ret);
    return ret;
}

int fake_bgm_check_cannot_load(void *ctx) {
    debugPrintf("fake_bgm_check_cannot_load(%x)\n", ctx);
    int ret = bgm_check_cannot_load(ctx);
    debugPrintf("fake_bgm_check_cannot_load() = %d\n", ret);
    return 0;
}

int fake_bgm_update_load_info(void *ctx) {
    return bgm_update_load_info(ctx);
}

bool fake_bgm_set_by_name(void *ctx, char *name) {
    debugPrintf("fake_bgm_set_by_name(%x, '%s')\n", ctx, name);
    bool ret = bgm_set_by_name(ctx, name);
    debugPrintf("fake_bgm_set_by_name() = %d\n", ret);
    return ret;
}

bool fake_bgm_set_play_status(void *ctx, sfp_float a2, bool repeat) {
    debugPrintf("fake_bgm_set_play_status(%x, %f, %d)\n", ctx, sfp2float(a2), repeat);
    bool ret = bgm_set_play_status(ctx, a2, repeat);
    debugPrintf("fake_bgm_set_play_status() = %d\n", ret);
    return ret;
}

bool fake_bgm_set_play_status2(void *ctx, sfp_float a2) {
    debugPrintf("fake_bgm_set_play_status2(%x, %f)\n", ctx, sfp2float(a2));
    bool ret = bgm_set_play_status2(ctx, a2);
    debugPrintf("fake_bgm_set_play_status2() = %d\n", ret);
    return ret;
}

bool fake_bgm_set_unknows_status(void *ctx, sfp_float a2) {
    debugPrintf("fake_bgm_set_unknows_status(%x, %f)\n", ctx, sfp2float(a2));
    bool ret = bgm_set_unknows_status(ctx, a2);
    debugPrintf("fake_bgm_set_unknows_status() = %d\n", ret);
    return ret;
}

void patch_audio() {
    // todo fix se
    hook_addr(LOAD_ADDRESS + 0xaeb40 + 1, (uintptr_t) &fake_load_se);
    hook_addr(LOAD_ADDRESS + 0x1a248 + 1, (uintptr_t) &fake_play_se_by_slot);
    hook_addr(LOAD_ADDRESS + 0x1a338 + 1, (uintptr_t) &fake_play_se_by_slot_2);
    hook_addr(LOAD_ADDRESS + 0x1a514 + 1, (uintptr_t) &fake_play_se_by_slot_3);

    // todo fix bgm
    inline_hook(LOAD_ADDRESS + 0xae1e8 + 1, (uintptr_t) &fake_init_bgm, (uintptr_t *) &init_bgm);
    inline_hook(LOAD_ADDRESS + 0xae654 + 1, (uintptr_t) &fake_bgm_check_cannot_load, (uintptr_t *) &bgm_check_cannot_load);
    inline_hook(LOAD_ADDRESS + 0xae4d8 + 1, (uintptr_t) &fake_bgm_update_load_info, (uintptr_t *) &bgm_update_load_info);
    inline_hook(LOAD_ADDRESS + 0xae304 + 1, (uintptr_t) &fake_bgm_set_by_name, (uintptr_t *) &bgm_set_by_name);
    inline_hook(LOAD_ADDRESS + 0xae324 + 1, (uintptr_t) &fake_bgm_set_play_status, (uintptr_t *) &bgm_set_play_status);
    inline_hook(LOAD_ADDRESS + 0xae348 + 1, (uintptr_t) &fake_bgm_set_play_status2, (uintptr_t *) &bgm_set_play_status2);
    inline_hook(LOAD_ADDRESS + 0xae38e + 1, (uintptr_t) &fake_bgm_set_unknows_status, (uintptr_t *) &bgm_set_unknows_status);
}
