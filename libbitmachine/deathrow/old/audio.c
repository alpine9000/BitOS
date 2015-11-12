#include "audio.h"
#include "peripheral.h"

void audio_execute(unsigned long channel)
{
  peripheral.audio.execute = channel;
}


void audio_selectChannel(unsigned long channel)
{
  peripheral.audio.channelSelect = channel;
}

void audio_frequency(unsigned long frequency)
{
  peripheral.audio.frequency = frequency;
}

void audio_setType(audio_type_t type)
{
  peripheral.audio.type = (unsigned long)type;
}

void audio_setAddress(unsigned long *address)
{
  peripheral.audio.address = (unsigned long)address;
}

void audio_setLength(unsigned long length)
{
  peripheral.audio.length = length;
}
