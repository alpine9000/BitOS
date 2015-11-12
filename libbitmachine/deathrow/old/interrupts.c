#include <stdio.h>
#include "peripheral.h"
#include "gfx.h"
#include "simulator.h"
#include "file.h"

#define	_SYS_exit       _SYS_reserved
#define	_SYS_fork       _SYS_reserved

#define	_SYS_read	_file_read
#define	_SYS_write	_file_write
#define	_SYS_open	_file_open
#define	_SYS_close	_file_close
#define	_SYS_wait4	_SYS_reserved	
#define	_SYS_creat	_SYS_reserved	
#define	_SYS_link	_SYS_reserved	
#define	_SYS_unlink	_SYS_reserved	
#define	_SYS_execv	_SYS_reserved	
#define	_SYS_chdir	_SYS_reserved	
#define	_SYS_mknod	_SYS_reserved
#define	_SYS_chmod	_SYS_reserved
#define	_SYS_chown	_SYS_reserved
#define	_SYS_lseek	_SYS_reserved
#define	_SYS_getpid	_SYS_reserved
#define _SYS_isatty     _file_isatty
#define _SYS_fstat      _file_fstat
#define _SYS_time 	_SYS_reserved
#define _SYS_ARG	 _SYS_reserved
#define	_SYS_stat	_SYS_reserved
#define	_SYS_pipe	_SYS_reserved
#define	_SYS_execve	_SYS_reserved
#define _SYS_truncate	_SYS_reserved
#define _SYS_ftruncate	_SYS_reserved

#define _SYS_argc	_SYS_reserved
#define _SYS_argnlen	_SYS_reserved
#define _SYS_argn	_SYS_reserved
#define _SYS_utime      _SYS_reserved
#define _SYS_wait        _SYS_reserved

int _SYS_reserved()
{
  return -1;
}

static char lookup[] = { '0', '1', '2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

static inline void guru(unsigned int address, char* cause)
{
  peripheral.videoFrameBuffer = 0;
  peripheral.videoAddressX = 0;
  peripheral.videoAddressY = 0;
  peripheral.videoWidth = videoWidth;
  peripheral.videoHeight = videoHeight;
  peripheral.videoRect = 0xFFFFFFFF;
  gfx_drawStringEx(0, 0, cause, 0xFF000000, 1, 1);  
  int i,j;
  char ascii[9], *tmp = (char*)&address;
  for (i=0, j = 0; i<4; ++i)
  {
    //    ascii[j++] = lookup[(tmp[i] & 0xf0) >> 4];
    //ascii[j++] = lookup[tmp[i] & 0xf];
    ascii[j++] = lookup[(tmp[i] & 0xf0) >> 4];
    ascii[j++] = lookup[tmp[i] & 0xf];
  }
  ascii[8] = 0;
  for (i = 0; cause[i] != 0; i++);
  gfx_drawStringEx((i+1)*6, 0, ascii, 0xFF000000, 1, 1);  
  simulator_stop();
}

void illegalSlot(unsigned int address)
{
  guru(address, "ILLEGAL SLOT AT");
}

void panic(unsigned int address)
{
  guru(address, "PANIC AT");
}

unsigned long syscall_table[] = {
  (unsigned long)_SYS_reserved,
  (unsigned long)_SYS_exit     ,
  (unsigned long)_SYS_fork     ,
  (unsigned long)_SYS_read     ,
  (unsigned long)_SYS_write    ,
  (unsigned long)_SYS_open     ,
  (unsigned long)_SYS_close    ,
  (unsigned long)_SYS_wait4    ,
  (unsigned long)_SYS_creat    ,
  (unsigned long)_SYS_link     ,
  (unsigned long)_SYS_unlink	,
  (unsigned long)_SYS_execv	,
  (unsigned long)_SYS_chdir	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_mknod	,
  (unsigned long)_SYS_chmod	,
  (unsigned long)_SYS_chown	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_lseek	,
  (unsigned long)_SYS_getpid	,
  (unsigned long)_SYS_isatty      ,
  (unsigned long)_SYS_fstat       ,
  (unsigned long)_SYS_time 	,
  (unsigned long)_SYS_ARG		,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_reserved	,
  (unsigned long)_SYS_stat	
};
