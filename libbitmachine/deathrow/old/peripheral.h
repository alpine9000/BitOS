#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H

typedef struct {
  unsigned long channelSelect;
  unsigned long frequency;
  unsigned long ramp;
  unsigned long decay;
  long hold;
  unsigned long type;
  unsigned long address;
  unsigned long length;
  unsigned long execute;
} audio_t;

typedef struct {
  unsigned long stop;
  unsigned long yield;
  unsigned long startStopWatch;
  unsigned long stopStopWatch;
  unsigned long stopWatchElapsed;
} simulator_t;

typedef struct {
  unsigned long seconds;
  unsigned long useconds;
  unsigned long elapsedMilliSeconds;
} simulator_time_t;

typedef struct {
  unsigned long fd;
  unsigned long filename;
  unsigned long status;
  int read;
  unsigned long close;
} simulator_file_t;

typedef struct {
  simulator_t simulator;

  unsigned long consoleWrite;
  unsigned long consoleColor;
  unsigned long consoleRead;
  unsigned long consoleReadBufferSize;
  unsigned long consoleSelectKeyState;
  unsigned long consoleKeyState;

  unsigned long videoFrameBuffer;
 
  unsigned long videoAddressX;
  unsigned long videoAddressY;
  unsigned long videoWidth;
  unsigned long videoHeight;
  unsigned long videoPixel;
  unsigned long videoRect;


  unsigned long videoData;
  unsigned long videoSaveData;
  unsigned long videoLoadData;


  unsigned long bltSrc;
  unsigned long bltDest;
  unsigned long bltSize;
  unsigned long bltFrameBuffers;
  unsigned long bltFrameBufferScaling;

  unsigned long vblank;
  unsigned long videoScaling;
  unsigned long videoResolution;
  simulator_time_t time;

  audio_t audio;

  simulator_file_t file;
} peripheral_t;

typedef struct {
  unsigned long data;
} video_ram_t;

extern volatile peripheral_t peripheral;
extern volatile video_ram_t videoRam;

#endif
