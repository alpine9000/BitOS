#include "kernel.h"
#include "panic.h"
#include <sys/lock.h>

extern unsigned 
_kernel_atomic_lock_asm(void* ptr);


void
kernel_spinLock(void* ptr)
{
  for(;;) {
    if ( _kernel_atomic_lock_asm(ptr)) {
      return;
    } else {
      kernel_threadBlocked();
    }
  }
}


void
kernel_unlock(void* ptr)
{
  *(char *)ptr = 0;
}


void 
_kernel_newlib_lock_init_recursive(_LOCK_RECURSIVE_T* lock) 
{
  lock->thread = 0;
  lock->count = 0;
  lock->lock = 0;
}


void 
_kernel_newlib_lock_close_recursive(_LOCK_RECURSIVE_T* lock)
{
  lock->thread = 0;
  lock->count = 0;
  lock->lock = 0;
}


unsigned 
_kernel_newlib_lock_acquire_recursive(_LOCK_RECURSIVE_T* lock)
{
  thread_h tid = kernel_threadGetId();

  for (;;) {
    kernel_spinLock(&lock->lock);
    if (lock->thread == 0) {
      lock->thread = (unsigned)tid;
    }
    if (lock->thread == (unsigned)tid) {
      lock->count++;
      kernel_unlock(&lock->lock);
      return lock->count;
    }
    kernel_unlock(&lock->lock);
  }
}

 
unsigned 
_kernel_newlib_lock_try_acquire_recursive(_LOCK_RECURSIVE_T* lock)
{
  panic("_kernel_newlib_lock_try_acquire_recursive: not implemented");
  return 0;
}


void 
_kernel_newlib_lock_release_recursive(_LOCK_RECURSIVE_T* lock)
{
#ifdef _KERNEL_ASSERTS
  thread_h tid = kernel_threadGetId();
#endif

  kernel_spinLock(&lock->lock);

#ifdef _KERNEL_ASSERTS
  
  if (lock->thread != (unsigned)tid) {
    if (lock->count == 0 && lock->thread == 0) {
      kernel_unlock(&lock->lock);
      return;
    }
    panic("_kernel_newlib_lock_release_recursive: lock->thread != tid");
  }
#endif

  lock->count--;
  if (lock->count == 0) {
    lock->thread = 0;
  }

  kernel_unlock(&lock->lock);
}


void 
_kernel_newlib_lock_init(unsigned* lock)
{
  *lock = 0;
}


void 
_kernel_newlib_lock_close(unsigned* lock) 
{
  panic("_kernel_newlib_lock_close:  not implemented");
}


unsigned 
_kernel_newlib_lock_acquire(unsigned* lock) 
{
  kernel_spinLock(lock);

  return 0;
}


unsigned 
_kernel_newlib_lock_try_acquire(unsigned* lock) 
{
  panic("_kernel_newlib_lock_try_acquire: not implemented");
  return 0;
}

 
void _kernel_newlib_lock_release(unsigned* lock) 
{
  *lock = 0;
}
