#include "bft.h"
#include "window.h"
#include "kernel.h"
#include "gfx.h"
#include "audio.h"
#include "console.h"
#include "thread.h"
#include "simulator.h"
#include "file.h"
#include "_termios.h"
#include "lock.h"

extern void 
dlmalloc_stats(void);

extern int
wolf(int argc, char** argv);

// Changing this (except adding to the end) will stop all current client binaries from working.

_bft_t __bft  = {

  &window_create,
  &window_getFrameBuffer,
  &window_close,
  &window_isKeyDown,

  &kernel_version,
  &kernel_threadSetInfo,
  &kernel_threadBlocked,
  &kernel_spinLock,
  &kernel_unlock,
  &kernel_threadGetId,
  &kernel_threadDie,
  &kernel_threadGetWindow,
  &kernel_threadSpawn,
  &kernel_threadGetStats,
  &kernel_threadGetFds,
  &dlmalloc_stats,

  &gfx_fillRect,
  &gfx_drawPixel,
  &gfx_bitBlt,
  &gfx_drawStringEx,
  &gfx_createFrameBuffer,
  &gfx_loadData,
  &gfx_saveData,
  &gfx_getVideoWidth,
  &gfx_getVideoHeight,
  &gfx_drawStringRetro,


  &audio_execute,
  &audio_selectChannel,
  &audio_setType,
  &audio_setAddress,
  &audio_setLength,
  &audio_frequency,

  &_console_read_char,
  &_console_char_avail,
  &_console_write_char,
  &console_isKeyDown,
  &console_setCursorPos,
  &console_setCursorCol,
  &console_setCursorRow,
  &console_setBehaviour,
  &console_clearBehaviour,
  &console_getCursorRow,
  &console_getColumns,
  &console_getLines,
  &console_backspace,
  &console_insertAtCursor,
  &console_deleteAtCursor,
  &console_clearToEndOfLine,
  &console_reset,

  &thread_spawn,
  &thread_spawnFileDescriptors,
  &thread_load,
  &kernel_threadWait,

  &_kernel_newlib_lock_init_recursive,
  &_kernel_newlib_lock_close_recursive,
  &_kernel_newlib_lock_acquire_recursive,
  &_kernel_newlib_lock_try_acquire_recursive,
  &_kernel_newlib_lock_release_recursive,
  &_kernel_newlib_lock_init,
  &_kernel_newlib_lock_close,
  &_kernel_newlib_lock_acquire,
  &_kernel_newlib_lock_try_acquire,
  &_kernel_newlib_lock_release,

  &simulator_printf,

  &file_loadElfKernel,

  &tputs,
  &tcsetattr,
  &tcflow,
  &tcflush,
  &tgoto,
  &tcgetattr,
  &tgetnum,
  &tgetstr,
  &tgetflag,
  &tgetent,
  &cfmakeraw,

  &wolf,

  &kernel_threadGetExitStatus,

  &signal_registerHandler,
  &signal_fire,

  &kernel_threadFreeStats,

  &kernel_threadGetIdForStdout
};

_bft_t* _bft = &__bft;

_bft_t* 
bft_get()
{
  return &__bft;
}
