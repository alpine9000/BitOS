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

int 
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
