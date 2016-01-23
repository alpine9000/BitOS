#pragma once

#include <sys/lock.h>

void 
_kernel_newlib_lock_init_recursive(_LOCK_RECURSIVE_T* lock);

void 
_kernel_newlib_lock_close_recursive(_LOCK_RECURSIVE_T* lock);

unsigned 
_kernel_newlib_lock_acquire_recursive(_LOCK_RECURSIVE_T* lock);

unsigned 
_kernel_newlib_lock_try_acquire_recursive(_LOCK_RECURSIVE_T* lock);

void 
_kernel_newlib_lock_release_recursive(_LOCK_RECURSIVE_T* lock);


void 
_kernel_newlib_lock_init(unsigned* lock);

void 
_kernel_newlib_lock_close(unsigned* lock);

unsigned 
_kernel_newlib_lock_acquire(unsigned* lock);

unsigned 
_kernel_newlib_lock_try_acquire(unsigned* lock);

void 
_kernel_newlib_lock_release(unsigned* lock);


