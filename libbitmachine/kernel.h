#pragma once

#include <sys/lock.h>
#include "types.h"

struct tms;

#ifdef _KERNEL_BUILD

#define kernel_setThreadWindow(w) kernel_setThreadInfo(KERNEL_THREAD_WINDOW, (unsigned)w)

unsigned 
kernel_times (struct tms *tp);

char* 
kernel_getcwd(char *buf, unsigned size);

void 
kernel_stats();

extern void 
_kernel_resume_asm(unsigned int *sp);

extern unsigned 
_kernel_atomic_lock_asm(void* ptr);

unsigned 
kernel_enterKernelMode();

void     
kernel_exitKernelMode(unsigned ___ints_disabled);

void     
kernel_enableInts(unsigned enable);

unsigned 
kernel_disableInts();

void     
kernel_init(void(*ptr)(int argc, char** argv));

thread_h    
kernel_spawn(void (*entry)(int argc, char**argv),  char**argv, fds_t* fds);

thread_h    
kernel_load( unsigned* image, unsigned imageSize, void (*entry)(int,char**), char** argv, fds_t* fds, int clone_cwd);

void    
kernel_die(int status);

void     
kernel_spinLock(void* ptr);

void     
kernel_unlock(void* ptr);

void     
kernel_threadBlocked();

int      
kernel_setThreadInfo(thread_info_t type, unsigned info);

window_h      
kernel_getThreadWindow();

thread_h    
kernel_getTid();

fds_t*   
kernel_getFds();

int      
kernel_getExitStatus(thread_h tid);

thread_h    
kernel_getTidForStdout(unsigned fd);

unsigned 
kernel_getIsActiveImage(void* ptr);

int      
kernel_getIsThreadAlive(thread_h tid);

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


//#define KTRACE

#ifdef KTRACE
#define ktrace_current_thread() (peripheral.simulator.trace = currentThread)
#define ktrace_thread_blocked() (peripheral.simulator.trace = currentThread|0x100)
#define ktrace_simulator_yield() (peripheral.simulator.trace =0x1000)
#define ktrace_reset() (peripheral.simulator.traceReset = 1)
#else
#define ktrace_current_thread() 
#define ktrace_thread_blocked() 
#define ktrace_simulator_yield()
#define ktrace_reset()

#endif

#else //_KERNEL_BUILD

#define kernel_stats() _bft->kernel_stats()
#define malloc_stats() _bft->malloc_stats()
#define kernel_threadBlocked() _bft->kernel_threadBlocked()
#define kernel_setThreadWindow(i) _bft->kernel_setThreadInfo(KERNEL_THREAD_WINDOW, (unsigned)i)
#define kernel_getThreadWindow() _bft->kernel_getThreadWindow()
#define kernel_die(x) _bft->kernel_die(x)
#define kernel_spawn(x,v,f) _bft->kernel_spawn(x,v,f)
#define kernel_getFds(x) _bft->kernel_getFds(x)

#endif
