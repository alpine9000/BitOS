#pragma once

#include <sys/lock.h>
#include <termios.h>
#include "types.h"

typedef struct {
  window_h
  (*window_create)(char* title, unsigned x, unsigned y, unsigned w, unsigned h);
  int
  (*window_getFrameBuffer)(window_h window);
  void
  (*window_close)(window_h window);
  unsigned
  (*window_isKeyDown)(window_h window, unsigned key);

  const char*
  (*kernel_version)(void);
  int
  (*kernel_threadSetInfo)(thread_info_t type, unsigned info);
  void
  (*kernel_threadBlocked)();
  void
  (*kernel_spinLock)(void* ptr);
  void
  (*kernel_unlock)(void* ptr);
  thread_h
  (*kernel_threadGetId)();
  void
  (*kernel_threadDie)(int status)__attribute__ ((noreturn));
  window_h
  (*kernel_threadGetWindow)(void);
  thread_h
  (*kernel_threadSpawn)(int (*entry)(int argc, char**argv),  char**argv, fds_t* fds);
  thread_status_t**
  (*kernel_threadGetStatus)();
  fds_t*
  (*kernel_threadGetFds)(void);
  int
  (*kernel_threadGetExitStatus)(thread_h);
  void
  (*kernel_threadFreeStats)(thread_status_t** stats);
  thread_h
  (*kernel_threadGetIdForStdout)(unsigned fd);
  int
  (*kernel_threadWait)(thread_h tid);
  int
  (*kernel_threadGetStdoutForId)(thread_h tid);
 

  void
  (*gfx_fillRect)(unsigned fb, int x, int y, unsigned width, unsigned height, unsigned color);
  void
  (*gfx_drawPixel)(unsigned fb, int x, int y, unsigned color);
  void
  (* gfx_bitBlt)(unsigned fb, short srcX, short srcY, short destX, short destY, unsigned short width, unsigned short height, unsigned src);
  void
  (*gfx_drawStringEx)(unsigned fb, int  x, int y, char *c, unsigned color, int size, int spaceSize);
  unsigned
  (*gfx_createFrameBuffer)(unsigned w, unsigned h);
  void
  (*gfx_loadData)(unsigned fb);
  void
  (*gfx_saveData)(unsigned fb);
  unsigned
  (*gfx_getVideoWidth)();
  unsigned
  (*gfx_getVideoHeight)();
  void
  (*gfx_drawStringRetro)(unsigned fb, int  x, int y, char *c, unsigned color, int size, int spaceSize);

  void
  (*audio_execute)(unsigned channel);
  void
  (*audio_selectChannel)(unsigned channel);
  void
  (*audio_setType)(audio_type_t type);
  void
  (*audio_setAddress)(unsigned *address);
  void
  (*audio_setLength)(unsigned length);
  void
  (*audio_frequency)(unsigned frequency);

  int
  (*_console_read_char)();
  unsigned char
  (*_console_char_avail)();
  void
  (* _console_write_char)(char c);
  unsigned
  (*console_isKeyDown)(unsigned key);
  void
  (*console_setCursorPos)(unsigned col, unsigned row);
  void
  (*console_setCursorCol)(unsigned col);
  void
  (*console_setCursorRow)(unsigned row);
  void
  (*console_setBehaviour)(unsigned mask);
  void
  (*console_clearBehaviour)(unsigned mask);
  unsigned
  (*console_getCursorRow)();
  unsigned
  (*console_getColumns)();
  unsigned
  (*console_getLines)();
  void
  (*console_backspace)(void);
  void
  (*console_insertAtCursor)();
  void
  (*console_deleteAtCursor)();
  void
  (*console_clearToEndOfLine)();
  void
  (*console_reset)();

  thread_h
  (*thread_spawn)(char* command);
  thread_h
  (*thread_spawnFileDescriptors)(char* command, int in, int out, int err);
  int
  (*thread_run)(char* commandLine);
  int
  (*thread_open)(char* command);
  int
  (*thread_close)(int fd);

  void
  (*_bitos_lock_init_recursive)(_LOCK_RECURSIVE_T* lock);
  void
  (*_bitos_lock_close_recursive)(_LOCK_RECURSIVE_T* lock);
  unsigned
  (*_bitos_lock_acquire_recursive)(_LOCK_RECURSIVE_T* lock);
  unsigned
  (*_bitos_lock_try_acquire_recursive)(_LOCK_RECURSIVE_T* lock);
  void
  (*_bitos_lock_release_recursive)(_LOCK_RECURSIVE_T* lock);
  void
  (*_bitos_lock_init)(unsigned* lock);
  void
  (*_bitos_lock_close)(unsigned* lock);
  unsigned
  (*_bitos_lock_acquire)(unsigned* lock);
  unsigned
  (*_bitos_lock_try_acquire)(unsigned* lock);
  void
  (*_bitos_lock_release)(unsigned* lock);

  void
  (*file_loadElfKernel)(unsigned fd);
  int
  (*file_setOptions)(int fd, unsigned options);
  int
  (*file_getOptions)(int fd);

  int
  (*tputs)(const char *str, int affcnt, int (*putc)(int));
  int
  (*tcsetattr)(int fildes, int optional_actions, const struct termios *termios_p);
  int
  (*tcflow)(int fildes, int action);
  int
  (*tcflush)(int fildes, int action);
  char*
  (*tgoto)(const char *cap, int col, int row);
  int
  (*tcgetattr)(int fd, struct termios *termios_p);
  int
  (*tgetnum)(char *id);
  char*
  (*tgetstr)(char *id, char **area);
  int
  (*tgetflag)(char *id);
  int
  (*tgetent)(char *bp, const char *name);
  void
  (*cfmakeraw)(struct termios *termios_p);

  int
  (*message_handle)(int id, message_handler_t func);
  int
  (*message_send)(thread_h tid, int id, void* data);
  int
  (*message_getHandler)(message_handler_t* handler, int id);

  int
  (*simulator_printf)(const char *format, ...);
  int
  (*wolf)(int argc, char** argv);
  void
  (*malloc_stats)();

  unsigned
  (*console_getCursorCol)();

  void 
  (*gfx_bitBltEx)(unsigned fb, short sx, short sy, short dx, short dy, unsigned short sw, unsigned short sh, unsigned short dw, unsigned short dh, unsigned src);
  void
  (*gfx_drawLine)(unsigned fb, int x0, int y0, int x1, int y1, unsigned color);
} _bft_t;

extern _bft_t* _bft;

void    bft_init();
_bft_t* bft_get();


