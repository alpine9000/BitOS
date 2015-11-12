#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

/* dummy lock routines for single-threaded aps */

 
#include <_ansi.h>

typedef struct {
  unsigned lock;
  unsigned thread;
  unsigned count;
} _bitos_recursive_lock_t;

typedef unsigned _LOCK_T;
typedef _bitos_recursive_lock_t _LOCK_RECURSIVE_T;


void _bitos_lock_init(_LOCK_T *lock);
void _bitos_lock_close(_LOCK_T *lock);
unsigned _bitos_lock_acquire(_LOCK_T *lock);
unsigned _bitos_lock_try_acquire(_LOCK_T *lock); 
void _bitos_lock_release(_LOCK_T *lock);

void _bitos_lock_init_recursive(_LOCK_RECURSIVE_T* lock);
void _bitos_lock_close_recursive(_LOCK_RECURSIVE_T* lock);
unsigned _bitos_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock);
unsigned _bitos_lock_try_acquire_recursive(_LOCK_RECURSIVE_T *lock);
void _bitos_lock_release_recursive(_LOCK_RECURSIVE_T *lock);

#define __LOCK_INIT(_class,lock)             _class _LOCK_T lock = 0;
#define __LOCK_INIT_RECURSIVE(_class,_lock)  _class _LOCK_RECURSIVE_T _lock = {0,0,0};
#define __lock_init(lock)                    _bitos_lock_init(&lock)
#define __lock_init_recursive(lock)          _bitos_lock_init_recursive(&lock)
#define __lock_close(lock)                   _bitos_lock_close(&lock) 
#define __lock_close_recursive(lock)         _bitos_lock_close_recursive(&lock)
#define __lock_acquire(lock)                 _bitos_lock_acquire(&lock) 
#define __lock_acquire_recursive(lock)       _bitos_lock_acquire_recursive(&lock)
#define __lock_try_acquire(lock)             _bitos_lock_try_acquire(&lock) 
#define __lock_try_acquire_recursive(lock)   _bitos_lock_try_acquire_recursive(&lock)
#define __lock_release(lock)                 _bitos_lock_release(&lock) 
#define __lock_release_recursive(lock)       _bitos_lock_release_recursive(&lock)

#endif /* __SYS_LOCK_H__ */
