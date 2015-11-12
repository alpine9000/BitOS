#include <stdio.h>
#include "peripheral.h"
#include "gfx.h"
#include "simulator.h"
#include "file.h"
#include "memory.h"
#include "bft.h"
#include "process.h"
#include "kernel.h"

#define _notImplemented    _SYS_reserved

/* TODO - put these back in the correct order (order of the numbers in syscall.h */
#define	_SYS_exit          _notImplemented
#define	_SYS_fork          _notImplemented
#define	_SYS_read	   file_read
#define	_SYS_write	   file_write
#define	_SYS_open	   file_open
#define	_SYS_close	   file_close
#define	_SYS_wait4	   _notImplemented	//file_loaded
#define	_SYS_creat	   _notImplemented	
#define	_SYS_link	   _notImplemented	
#define	_SYS_unlink	   file_unlink
#define	_SYS_execv	   _notImplemented	
#define	_SYS_chdir	   file_chdir
#define	_SYS_mknod	   _notImplemented
#define	_SYS_chmod	   _notImplemented
#define	_SYS_chown	   _notImplemented
#define	_SYS_lseek	   file_lseek
#define _SYS_isatty        file_isatty
#define _SYS_fstat         file_fstat
#define _SYS_time 	   _notImplemented
#define _SYS_ARG	   _notImplemented
#define	_SYS_pipe	   file_pipe
#define	_SYS_execve	   _notImplemented
#define _SYS_truncate	   _notImplemented
#define _SYS_ftruncate	   _notImplemented
#define _SYS_brk           memory_sbrk
#define _SYS_bft           bft_get
#define _SYS_malloc        memory_malloc
#define _SYS_free          memory_free
#define _SYS_realloc       memory_realloc
#define _SYS_popen         process_open
#define _SYS_pclose        process_close
#define _SYS_times         kernel_times
#define _SYS_stat          file_stat
#define _SYS_opendir       file_opendir
#define _SYS_readdir       file_readdir
#define _SYS_getcwd        kernel_getcwd
#define _SYS_argc	   _notImplemented
#define _SYS_argnlen	   _notImplemented
#define _SYS_argn	   _notImplemented
#define _SYS_utime         _notImplemented
#define _SYS_wait          _notImplemented
#define _SYS_mkdir         file_mkdir
#define _SYS_gtod          simulator_gtod
#define	_SYS_getpid	   kernel_getPid
#define _SYS_rename        file_rename

int _notImplemented()
{
  return -1;
}

static char lookup[] = { '0', '1', '2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

static inline void guru(unsigned int address, char* cause, char* message)
{
  kernel_disableInts();
  volatile video_t* video = &peripheral.video[0];
  video->videoAddressX = 0;
  video->videoAddressY = 0;
  video->videoWidth = videoWidth;
  video->videoHeight = videoHeight;
  video->videoRect = 0xFF000000;
  gfx_drawStringEx(0, 0, 0, cause, 0xFFFF0000, 1, 1);  
  int i,j;
  char ascii[9], *tmp = (char*)&address;
  for (i=0, j = 0; i<4; ++i)
  {
    ascii[j++] = lookup[(tmp[i] & 0xf0) >> 4];
    ascii[j++] = lookup[tmp[i] & 0xf];
  }
  ascii[8] = 0;
  for (i = 0; cause[i] != 0; i++);
  gfx_drawStringEx(0, (i+1)*6, 0, ascii, 0xFFFF0000, 1, 1);  
  gfx_drawStringEx(0, 0, 12, message, 0xFFFF0000, 1, 1);  
  for(i = 0; message[i] != 0; i++) {
    peripheral.console.consoleWrite = message[i];
  }
  peripheral.console.consoleWrite = '\n';
  simulator_stop();
}

void illegalSlot(unsigned int address)
{
  guru(address, "ILLEGAL SLOT AT", "");
}

void handlePanic(char *message, unsigned int address)
{
  guru(address, "PANIC AT", message);
}

unsigned syscall_table[] = {
  (unsigned)_notImplemented,
  (unsigned)_SYS_exit     ,
  (unsigned)_SYS_fork     ,
  (unsigned)_SYS_read     ,
  (unsigned)_SYS_write    ,
  (unsigned)_SYS_open     ,
  (unsigned)_SYS_close    ,
  (unsigned)_SYS_wait4    ,
  (unsigned)_SYS_creat    ,
  (unsigned)_SYS_link     ,
  (unsigned)_SYS_unlink	,
  (unsigned)_SYS_execv	,
  (unsigned)_SYS_chdir	,
  (unsigned)_SYS_reserved,
  (unsigned)_SYS_mknod	,
  (unsigned)_SYS_chmod	,
  (unsigned)_SYS_chown	,
  (unsigned)_SYS_reserved,
  (unsigned)_SYS_reserved,
  (unsigned)_SYS_lseek	,
  (unsigned)_SYS_getpid	,
  (unsigned)_SYS_isatty      ,
  (unsigned)_SYS_fstat       ,
  (unsigned)_SYS_time 	,
  (unsigned)_SYS_ARG		,
  (unsigned)_SYS_brk,
  (unsigned)_SYS_bft	,
  (unsigned)_SYS_malloc	,
  (unsigned)_SYS_free	,
  (unsigned)_SYS_realloc	,
  (unsigned)_SYS_gtod ,
  (unsigned)_SYS_popen	,
  (unsigned)_SYS_times	,
  (unsigned)_SYS_reserved	,
  (unsigned)_SYS_opendir	,
  (unsigned)_SYS_reserved	,
  (unsigned)_SYS_readdir	,
  (unsigned)_SYS_getcwd	,
  (unsigned)_SYS_stat,
  (unsigned)_SYS_mkdir,
  (unsigned)_SYS_pclose,
  (unsigned)_SYS_rename,
  (unsigned)_SYS_pipe,
  
};
