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
#include "message.h"

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
  &kernel_threadGetExitStatus,
  &kernel_threadFreeStats,
  &kernel_threadGetIdForStdout,
  &kernel_threadWait,
  &kernel_threadGetStdoutForId,

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
  &thread_run,
  &thread_open,
  &thread_close,

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

  &file_loadElfKernel,
  &file_setOptions,
  &file_getOptions,

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

  &message_handle,
  &message_send,
  &message_getHandler,

  &simulator_printf,
  &wolf,
  &dlmalloc_stats,

  &console_getCursorCol,
  &gfx_bitBltEx,
  &gfx_drawLine
};

_bft_t* _bft = &__bft;

_bft_t* 
bft_get()
{
  return &__bft;
}
