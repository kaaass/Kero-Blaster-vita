/* audio.cpp -- Audio hooks
 *
 * Copyright (C) 2022 KAAAsS
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <oboe/AudioStreamBuilder.h>
#include "main.h"
#include "so_util.h"

/*
 * Hooks
 */

int pxaAudioStream_open_hook(void *base, int channel, int sample_rate, int perf) {
    auto **audioStream = static_cast<oboe::AudioStream **>((void *) ((uint32_t) base + 0xC));
    oboe::AudioStreamBuilder builder;
    auto ret = static_cast<int>(builder.setChannelCount(channel)
            ->setFormat(oboe::AudioFormat::I16)
            ->setSampleRate(sample_rate)
            ->setSharingMode(oboe::SharingMode::Shared)
            ->setCallback(static_cast<oboe::AudioStreamCallback *>(base))
            ->setDeviceId(0)
            ->openStream(audioStream));
    // debugPrintf("AudioStream_open(%p, %d, %d, %d) = %d\n", base, channel, sample_rate, perf, ret);
    return ret;
}

extern "C" {
void patch_audio() {
    hook_addr(LOAD_ADDRESS + 0x19f20 + 1, (uintptr_t) &pxaAudioStream_open_hook);
}
}
