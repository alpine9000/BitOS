#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/syslimits.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/dirent.h>
#include <utime.h>
#include <fcntl.h>
#include <errno.h>
#include "panic.h"
#include "kernel.h"
#include "lock.h"
#include "simulator.h"
#include "syscall.h"
#include "peripheral.h"
#include "memory_config.h"
#include "bft.h"

void 
_bitos_lock_init(unsigned* lock)
{
  _bft->_bitos_lock_init(lock);
}

void 
_bitos_lock_init_recursive(_LOCK_RECURSIVE_T* lock) 
{
  _bft->_bitos_lock_init_recursive(lock);
}

void 
_bitos_lock_close(unsigned* lock) 
{
  _bft->_bitos_lock_close(lock);
}

void 
_bitos_lock_close_recursive(_LOCK_RECURSIVE_T* lock)
{
  _bft->_bitos_lock_close_recursive(lock);
}

unsigned 
_bitos_lock_acquire(unsigned* lock) 
{
  return _bft->_bitos_lock_acquire(lock);
}

unsigned 
_bitos_lock_acquire_recursive(_LOCK_RECURSIVE_T* lock)
{
  return _bft->_bitos_lock_acquire_recursive(lock);
}

unsigned 
_bitos_lock_try_acquire(unsigned* lock) 
{
  return _bft->_bitos_lock_try_acquire(lock);
}
 
unsigned 
_bitos_lock_try_acquire_recursive(_LOCK_RECURSIVE_T* lock)
{
  return _bft->_bitos_lock_try_acquire_recursive(lock);
}

void 
_bitos_lock_release(unsigned* lock) 
{
  _bft->_bitos_lock_release(lock);
}

void 
_bitos_lock_release_recursive(_LOCK_RECURSIVE_T* lock)
{
  _bft->_bitos_lock_release_recursive(lock);
}

clock_t 
_times_r (struct _reent *reent, struct tms *tp)
{
  return __trap34 (SYS_times, (int)tp);
}

int 
_gettimeofday_r (struct _reent *reent, struct timeval *tv, void *tz)
{
  return __trap34 (SYS_gtod, (int)tv, (int)tz);
}

size_t 
_read_r(struct _reent *ptr, int fd, _PTR buf, size_t cnt)
{
  return __trap34 (SYS_read, fd, (int)buf, cnt);
}


int 
_lseek_r (struct _reent* re, int file, int ptr, int dir)
{
  return __trap34 (SYS_lseek, file, ptr, dir);
}


int 
_write_r (struct _reent *re,  int file,  char *ptr,	 int len)
{
  return __trap34 (SYS_write, file, (int)ptr, len);
}

int 
_close_r (struct _reent *re, int file)
{
  return __trap34 (SYS_close, file, 0, 0);
}

int 
_fstat_r (struct _reent *re, int file,struct stat *st)
{
  return __trap34(SYS_fstat, file, st);
}


int 
_open_r (struct _reent *re, const char *path, int flags)
{
  return  __trap34 (SYS_open, (int)path, flags, &re->_errno);
}

int 
_creat_r (struct _reent *re, const char *path, int mode)
{
  return __trap34 (SYS_creat, (int)path, mode, 0);
}

int 
_unlink_r (struct _reent *re, char* file)
{
  return __trap34 (SYS_unlink, (int)file, 0);
}

int 
_isatty_r (struct _reent *re, int fd)
{
  return __trap34(SYS_isatty, fd);// _file_isatty(fd);
}

int 
_kill_r (int n, int m)
{
  return __trap34 (SYS_exit, 0xdead, 0, 0);
}

int 
_getpid_r (int n)
{
  return __trap34 (SYS_getpid);
}


FILE *
popen(const char *command, const char *mode)
{
  return (FILE*)__trap34 (SYS_popen, (int)command);
}


int 
pclose(FILE *stream)
{
  if (stream != 0) {
    int retval =  (int)__trap34 (SYS_pclose, (int)stream);
    return retval;
  } else {
    return -1;
  }
}

DIR *
opendir(const char *dirname)
{
  return (DIR*) __trap34 (SYS_opendir, (int)dirname); 
}

struct dirent *
readdir(DIR *dirp)
{
  return (struct dirent*)__trap34 (SYS_readdir, (int)dirp);
}


long 
sysconf(int name)
{
  static char buffer[255];
  //  panic("sysconf - not implemented");
  //  errno = EINVAL;
  if (name == _SC_PAGESIZE) {
    return 4096;
  } else if (name == _SC_CLK_TCK) {
    return 1000;
  } else if (name == _SC_PHYS_PAGES) {
    return __MEMORY_HEAP_SIZE;
  } else if (name == _SC_OPEN_MAX) {
    return 1024;
  } else {
    sprintf(buffer, "sysconf - unhandled option %d", name);
    panic(buffer);
  }
  return -1;
}

int 
_stat_r(struct _reent *r, const char *path, struct stat *st)
{
  return  __trap34 (SYS_stat, (int)path, (int)st, &r->_errno);    
}


int 
access(const char *path, int amode)
{
  struct _reent r;
  struct stat st;
  return  _stat_r(&r, path, &st);
}

int 
chdir(const char *path)
{
  return __trap34 (SYS_chdir, path);
}

char *
getcwd(char *buf, size_t size)
{
  return (char*) __trap34 (SYS_getcwd, (int)buf, size);
}

char *
getwd(char *buf)
{
  return (char*)__trap34 (SYS_getcwd, (int)buf, PATH_MAX);
}

int 
mkdir(const char *path, mode_t mode)
{
  return __trap34 (SYS_mkdir, (int)path, mode);
}

int 
pipe(int fds[2])
{
  return __trap34 (SYS_pipe, (int)fds);
}

int 
chmod(const char *path, unsigned mode)
{
  return -1;
}

unsigned 
umask(unsigned cmask)
{
  //panic("umask - not implemented");
  return 0;
}


int 
lstat(const char *restrict path, struct stat *restrict buf)
{
  return stat(path, buf);
}

int 
closedir(DIR *dirp)
{
  // //panic("closedir - not implemented");
  free(dirp);
  return 0;
}


long double 
strtold(const char *restrict nptr, char **restrict endptr)
{
  panic("strtold - not implemented");
  return 0;
}

int  
utime(const char *path, const struct utimbuf *times)
{
  panic("utime - not implemented");
  return -1;
}


int 
_link_r(struct _reent *r,const char *path1, const char *path2)
{
  //  panic("_link_r - not implemented");
  printf("link %s %s ?!?!\n", path1, path2);
  return -1;
}

int 
chown(const char *path, uid_t owner, gid_t group)
{
  panic("chown - not implemented");
  return -1;
}

int 
rmdir(const char *path)
{
  panic("rmdir - not implemented");
  return -1;
}

int 
dup2(int fildes, int fildes2)
{
  panic("dup2 - not implemented");
  return -1;
}

unsigned int 
sleep(unsigned int seconds)
{
  panic("sleep - not implemented");
  return 0;
}

unsigned 
alarm(unsigned seconds)
{
  panic("alarm - not implemented");
  return 0;
}

int 
execvp(const char *file, char *const argv[])
{
  panic("execvp - not implemented");
  return 0;
}

pid_t  
fork(void)
{
  return -1;
}

pid_t 
_wait_r(struct _reent *reent, int *stat_loc)
{
  panic("_wait_r - not implemented");
  return 0;
}

int 
execv(const char *path, char *const argv[])
{
  panic("execv - not implemented");
  return 0;
}

struct passwd * 
getpwnam(const char *login)
{
  panic("getpwnam - not implemented");
  return 0;
}

int 
dup(int fildes)
{
  panic("dup - not implemented");
  return 0;
}

int 
ftruncate(int fildes, off_t length)
{
  panic("ftruncate - not implemented");
  return 0;
}

char *
getlogin(void)
{
  panic("getlogin - not implemented");
  return 0;

}

int 
_rename_r(struct _reent *re, const char *old, const char *new)
{
  return __trap34 (SYS_rename, (int)old, (int)new);
}


int 
select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict errorfds, struct timeval *restrict timeout)
{
  //  simulator_printf("select %d %x %x %x %x\n", nfds, readfds, writefds, errorfds, timeout);
  return 0;
}


void* 
_malloc_r (struct _reent *reent, size_t size)
{
  return (void*)__trap34(SYS_malloc, size);
}

void* 
_realloc_r (struct _reent *reent, void* ptr, size_t size)
{
  return (void*)__trap34(SYS_realloc, ptr, size);
}

void 
_free_r (struct _reent *reent, void* ptr)
{
  __trap34(SYS_free, ptr);
}

void* 
_calloc_r(struct _reent* reent, size_t n, size_t m)
{
  char* ptr = (char*)__trap34(SYS_malloc, n*m);

  memset(ptr, 0, n*m);
  return ptr;
}

/*char *
basename(char *path)
{
  char *base = strrchr(path, '/');
  return base ? base+1 : path;
  }*/

char* 
basename(char *path)
{
  char *p;
  if( path == NULL || *path == '\0' )
    return ".";
  p = path + strlen(path) - 1;
  while( *p == '/' ) {
    if( p == path )
      return path;
    *p-- = '\0';
  }
  while( p >= path && *p != '/' )
    p--;
  return p + 1;
}

