#ifndef _AUDIO_H
#define _AUDIO_H

typedef enum {
  AUDIO_TYPE_SINE = 0,
  AUDIO_TYPE_SAWTOOTH = 1,
  AUDIO_TYPE_SQUARE = 2,
  AUDIO_TYPE_TRIANGLE = 3,
  AUDIO_TYPE_BUFFER = 4,
} audio_type_t;

void audio_execute(unsigned long channel);
void audio_selectChannel(unsigned long channel);
void audio_setType(audio_type_t type);
void audio_setAddress(unsigned long *address);
void audio_setLength(unsigned long length);
void audio_frequency(unsigned long frequency);

#endif
