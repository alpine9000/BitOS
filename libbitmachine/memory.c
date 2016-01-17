#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include "memory.h"
#include "memory_config.h"
#include "panic.h"
#include "peripheral.h"
#include "kernel.h"

//static char __attribute__ ((section (".heap"))) heap[__MEMORY_HEAP_SIZE];
extern char heap[__MEMORY_HEAP_SIZE];
unsigned _memory_total = sizeof(heap);

extern void *  
dlmalloc(size_t size);

extern void 
dlfree(void *ptr);

extern void* 
dlrealloc(void *ptr, size_t size);

#define _memory_lock() unsigned ___ints_disabled = kernel_disableInts()
#define _memory_unlock() kernel_enableInts(___ints_disabled)

#define _sbrk_lock() unsigned ___ints_disabled = kernel_disableInts()
#define _sbrk_unlock() kernel_enableInts(___ints_disabled)


void* 
memory_sbrk(ptrdiff_t incr)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  if (incr < 0) {
    panic("memory_sbrk: negative incr");
  }

  //  _sbrk_lock();
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0){
    heap_end = &heap[0];
  }
  prev_heap_end = heap_end;
  if (heap_end + incr > &heap[0]+sizeof(heap))  {
    panic("_memory_sbrk - heap exhausted");
  }
  heap_end += incr;
  //  _sbrk_unlock();
  return (caddr_t) prev_heap_end;
}

void* 
memory_malloc(size_t size)
{
  _memory_lock();
  
  void* ptr = dlmalloc(size);
  
  peripheral.malloc.mallocSize = size;
  peripheral.malloc.pid = (unsigned)kernel_threadGetId();
  peripheral.malloc.mallocAddress = (unsigned)ptr;

  _memory_unlock();

  return ptr;
}

void 
memory_free(void *ptr)
{
  _memory_lock();
  
  peripheral.malloc.freeAddress = (unsigned)ptr;
  
  dlfree(ptr);

  _memory_unlock();
}

void* 
memory_realloc(void* ptr, size_t size)
{
  _memory_lock();

  void* newPtr = dlrealloc(ptr, size);

  peripheral.malloc.freeAddress = (unsigned)ptr;
  peripheral.malloc.mallocSize = size;
  peripheral.malloc.pid = (unsigned)kernel_threadGetId();
  peripheral.malloc.mallocAddress = (unsigned)newPtr;
  _memory_unlock();

  return newPtr;
}


void *
_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
  return memory_sbrk(incr);
}

// ints disabled by ISR before only call to this
void 
memory_cleanupThread(thread_h tid)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  if (tid != 0) {

    peripheral.malloc.list = (unsigned)tid;
    
    unsigned address;
    while ((address = peripheral.malloc.mallocAddress) != 0) {
      //if (!kernel_threadGetIsActiveImage((void*)address)) {
	free((void*)address);
      //} 
    }

    peripheral.malloc.freePid = (unsigned)tid;
  }
}

void* 
malloc(size_t size)
{
  KERNEL_ASSERT_KERNEL_MODE();
  return memory_malloc(size);
}

/*void * calloc(size_t count, size_t size)
{						
  _memory_lock();
  char* ptr = dlmalloc(size*count);
  _memory_unlock();

  memset(ptr, 0, size*count);
  return ptr;
  }*/


void 
free(void *ptr)
{
  KERNEL_ASSERT_KERNEL_MODE();
  memory_free(ptr);
}


void *realloc(void *ptr, size_t size)
{
  KERNEL_ASSERT_KERNEL_MODE();
  return memory_realloc(ptr, size);
}

void malloc_abort(void)
{
  panic("malloc aborted\n");
}
