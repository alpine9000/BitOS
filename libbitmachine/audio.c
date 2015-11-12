#include "audio.h"
#include "peripheral.h"
#include "kernel.h"

#define _audio_lock()  unsigned ___ints_disabled = kernel_disableInts()
#define _audio_unlock()  kernel_enableInts(___ints_disabled)

void audio_execute(unsigned channel)
{
  peripheral.audio.execute = channel;
}


void audio_selectChannel(unsigned channel)
{
  peripheral.audio.channelSelect = channel;
}

void audio_frequency(unsigned frequency)
{
  peripheral.audio.frequency = frequency;
}

void audio_setType(audio_type_t type)
{
  peripheral.audio.type = (unsigned)type;
}

void audio_setAddress(unsigned *address)
{
  peripheral.audio.address = (unsigned)address;
}

void audio_setLength(unsigned length)
{
  peripheral.audio.length = length;
}

void audio_bell()
{
  _audio_lock();
  peripheral.audio.channelSelect = 0;
  peripheral.audio.type = 0;
  peripheral.audio.ramp = 5;
  peripheral.audio.decay = 5;
  peripheral.audio.hold= 50;
  peripheral.audio.frequency = 250000;
  peripheral.audio.execute = 0;
  _audio_unlock();
}
