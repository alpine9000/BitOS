#include "panic.h"
#include <stdio.h>
#include <string.h>
#include "syscall.h"
#include "kernel.h"
#include "bft.h"
#include <sys/stat.h>
#include <sys/reent.h>

int errno;

void 
kernel_threadBlocked() 
{
  _bft->kernel_threadBlocked();
}

void 
kernel_spinLock(void* ptr)
{
  _bft->kernel_spinLock(ptr);
}

void 
kernel_unlock(void* ptr)
{
  _bft->kernel_unlock(ptr);
}

thread_h 
kernel_threadGetId()
{
  return _bft->kernel_threadGetId();
}

void 
kernel_threadDie(int status)
{
  _bft->kernel_threadDie(status);
}

thread_h 
thread_spawn(char* command)
{
  return _bft->thread_spawn(command);
}

int 
thread_wait(thread_h tid)
{
  return _bft->thread_wait(tid);
}

int 
thread_load(char* commandLine)
{
  return _bft->thread_load(commandLine);
}

long  
pathconf(const char *path, int name)
{
  return 0;
}


long  
fpathconf(int fildes, int name)
{
  return 0;
}

void 
wolf()
{

}

/* I am guessing these are needed because clients use these without the header that redefines them */


void *  
malloc(size_t size)
{
  return (void*)__trap34(SYS_malloc, size);
}


void * 
calloc(size_t count, size_t size)
{
  char* ptr = (char*)__trap34(SYS_malloc, size*count);

  memset(ptr, 0, size*count);
  return ptr;
}


void 
free(void *ptr)
{
  __trap34(SYS_free, ptr);
}


void *
realloc(void *ptr, size_t size)
{
  return (void*)__trap34(SYS_realloc, ptr, size);
}


int 
tputs(const char *str, int affcnt, int (*putc)(int))
{
  return _bft->tputs(str, affcnt, putc);
}

int 
tcsetattr(int fildes, int optional_actions, const struct termios *termios_p)
{
  return _bft->tcsetattr(fildes, optional_actions, termios_p);
}

int 
tcflow(int fildes, int action)
{
  return _bft->tcflow(fildes, action);
}

int 
tcflush(int fildes, int action)
{
  return _bft->tcflush(fildes, action);
}

char* 
tgoto(const char *cap, int col, int row)
{
  return _bft->tgoto(cap, col, row);
}

int 
tcgetattr(int fd, struct termios *termios_p)
{
  return _bft->tcgetattr(fd, termios_p);
}

int 
tgetnum(char *id)
{
  return _bft->tgetnum(id);
}

char* 
tgetstr(char *id, char **area)
{
  return _bft->tgetstr(id, area);
}
   

int 
tgetflag(char *id)
{
  return _bft->tgetflag(id);
}

int 
tgetent(char *bp, const char *name)
{
  return _bft->tgetent(bp, name);
}

void 
cfmakeraw(struct termios *termios_p)
{
  _bft->cfmakeraw(termios_p);
}


thread_h 
thread_spawnFileDescriptors(char* command, int in, int out, int err)
{
  return _bft->thread_spawnFileDescriptors(command, in, out, err);
}


unsigned 
kernel_enterKernelMode()
{
  return 0;
}

void     
kernel_exitKernelMode(unsigned ___ints_disabled)
{

}
