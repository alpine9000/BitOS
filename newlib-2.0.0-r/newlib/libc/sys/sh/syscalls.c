#include <stdio.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>


#if 0
void __malloc_lock (struct _reent *reent)
{
  __trap34 (SYS_malloc_lock, (int)reent);
}

void __malloc_unlock (struct _reent *reent)
{
   __trap34 (SYS_malloc_unlock, (int)reent);
}


clock_t _times_r (struct tms *tp)
{
  return -1;
}

int _gettimeofday_r (struct _reent *reent, struct timeval *tv, void *tz)
{
  return __trap34 (SYS_gettimeofday, reent, tv, tz);
}


void *_sbrk_r(struct _reent *reent, ptrdiff_t incr)
{
  return (void*)__trap34 (SYS_sbrk, reent, incr);
}

size_t _read_r(struct _reent *ptr, int fd, _PTR buf, size_t cnt)
{
  return __trap34 (SYS_read, fd, (int)buf, cnt);
}

int _lseek_r (struct _reent* re, int file, int ptr, int dir)
{
  return __trap34 (SYS_lseek, file, ptr, dir);
}

int _write_r (struct _reent *re,  int file,  char *ptr,	 int len)
{
  return __trap34 (SYS_write, file, (int)ptr, len);
}

int _close_r (struct _reent *re, int file)
{
  return __trap34 (SYS_close, file, 0, 0);
}

int _fstat_r (struct _reent *re, int file,struct stat *st)
{
  return _file_fstat(file, st);
}

int _open_r (struct _reent *re, const char *path, int flags)
{
  return __trap34 (SYS_open, (int)path, flags, 0);
}

int _creat_r (struct _reent *re, const char *path, int mode)
{
  return __trap34 (SYS_creat, (int)path, mode, 0);
}

int _unlink_r (struct _reent *re, char* file)
{
  return __trap34 (SYS_unlink, file);
}

int _isatty_r (struct _reent *re, int fd)
{
  return _file_isatty(fd);
}

int _kill_r (int n, int m)
{
  return __trap34 (SYS_exit, 0xdead, 0, 0);
}

int _getpid_r (int n)
{
  return 1;
}
#endif
