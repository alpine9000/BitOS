#pragma once

#include "types.h"

#ifdef _KERNEL_BUILD

void audio_execute(unsigned channel);
void audio_selectChannel(unsigned channel);
void audio_setType(audio_type_t type);
void audio_setAddress(unsigned *address);
void audio_setLength(unsigned length);
void audio_frequency(unsigned frequency);
void audio_bell();

#else

#define audio_execute(channel) _bft->audio_execute(channel)
#define audio_selectChannel(channel) _bft->audio_selectChannel(channel)
#define audio_setType(type) _bft->audio_setType(type)
#define audio_setAddress(address) _bft->audio_setAddress(address)
#define audio_setLength(length) _bft->audio_setLength(length)
#define audio_frequency(frequency) _bft->audio_frequency(frequency)

#endif
