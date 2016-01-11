#pragma once

#define PERIPHERAL_MAX_FRAMEBUFFERS 10

typedef struct {
  unsigned channelSelect;
  unsigned frequency;
  unsigned ramp;
  unsigned decay;
  long hold;
  unsigned type;
  unsigned address;
  unsigned length;
  unsigned execute;
} audio_t;

typedef struct {
  unsigned stop;
  unsigned yield;
  unsigned yieldOnRTE;
  unsigned startStopWatch;
  unsigned stopStopWatch;
  unsigned stopWatchElapsed;
  unsigned trace;
  unsigned traceReset;
  unsigned print;
} simulator_t;

typedef struct {
  unsigned seconds;
  unsigned useconds;
  unsigned elapsedMilliSeconds;
} simulator_time_t;

typedef struct {
  unsigned load;
  unsigned size;
  unsigned relocate;
  //  unsigned entry;  
  void(*entry)();
  unsigned kernelLoad;
} simulator_elf_t;

typedef struct {
  unsigned oldPathname;
  unsigned newPathname;
  unsigned status;
  unsigned rename;
} file_rename_t;

typedef struct {
  unsigned path;
  unsigned status;
  unsigned descriptor;
} file_unlink_t;

typedef struct {
  unsigned long opendirStruct;
  unsigned long opendir;
  unsigned long readdir;
  unsigned long mkdir;
} simulator_dir_t;

typedef struct {
  unsigned fd;
  unsigned filename;
  unsigned flags;

  int stat;
  unsigned statPath;
  unsigned statStruct; // Address of struct st or 0
  unsigned statStatus;

  unsigned status;

  char* address; // Used for read/write DMA

  unsigned readLength;
  unsigned doRead;
  unsigned writeLength;
  unsigned close;
  unsigned closeStatus;

  int size;
  int seekDirection;
  int seek;
  int position;
  simulator_elf_t elf;
  simulator_dir_t dir;
  file_rename_t rename;
  file_unlink_t unlink;
} simulator_file_t;

typedef struct {
  unsigned srcOrigin;
  unsigned destOrigin;
  unsigned srcSize;
  unsigned destSize;
  unsigned frameBufferScaling;
  unsigned bltFromSrcWithScaling;
  unsigned bltFromSrcWithSize;
} blt_t;

typedef struct {
  int videoAddressX;
  int videoAddressY;
  unsigned videoWidth;
  unsigned videoHeight;
  unsigned videoPixel;
  unsigned videoRect;
  unsigned videoData;
  unsigned videoSaveData;
  unsigned videoLoadData;
  blt_t blt;
  unsigned videoResolution;
  unsigned alpha;
} video_t;

typedef struct {
  unsigned x;
  unsigned y;
  unsigned button;
} mouse_t;

typedef struct {
  unsigned mallocAddress;
  unsigned mallocSize;
  unsigned freeAddress;
  unsigned pid;
  unsigned list;
  unsigned freePid;
} malloc_track_t;

typedef struct {
  unsigned consoleWrite;
  unsigned consoleColor;
  unsigned consoleRead;
  unsigned consoleReadBufferSize;
  unsigned consoleSelectKeyState;
  unsigned consoleKeyState;
} console_t;

typedef struct {
  unsigned fb;
  unsigned name;
  unsigned size;
  unsigned x;
  unsigned y;
  unsigned color;
  unsigned text;
} programmable_character_generator_t;
  

typedef struct {
  simulator_t simulator;
  
  console_t console;

  unsigned vblank;
  unsigned videoScaling;
  unsigned devicePixelRatio;

  simulator_time_t time;

  audio_t audio;

  simulator_file_t file;

  mouse_t mouse;

  video_t video[PERIPHERAL_MAX_FRAMEBUFFERS];

  malloc_track_t malloc;

  programmable_character_generator_t pcg;

  unsigned kernelCmdLength;
  unsigned kernelCmd;

} peripheral_t;

typedef struct {
  unsigned data;
} video_ram_t;

#ifdef _KERNEL_BUILD
extern volatile peripheral_t peripheral;
extern volatile video_ram_t videoRam;
#else
#define peripheral (*((volatile peripheral_t*)0x10000000))
#define videoRam (*((volatile video_ram_t*)0x20000000))
#endif

