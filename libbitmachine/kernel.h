#pragma once

#include <sys/lock.h>
#include "types.h"

struct tms;

#ifdef _KERNEL_BUILD

#define lock(x)  kernel_spinLock(x)
#define unlock(x)  kernel_unlock(x)
#define spawn(x, v, f) kernel_spawn(x, v, f)
#define die(x) kernel_die(x)
#define thread_blocked() kernel_threadBlocked()
#define thread_window(w) kernel_setThreadInfo(KERNEL_THREAD_WINDOW, (unsigned)w)
#define thread_getWindow() kernel_getThreadWindow()

unsigned kernel_times (struct tms *tp);
char* kernel_getcwd(char *buf, unsigned size);

void kernel_stats();
extern void _kernel_resume_asm(unsigned int *sp);
extern unsigned _kernel_atomic_lock_asm(void* ptr);


void     kernel_enableInts(unsigned enable);
unsigned kernel_disableInts();
void     kernel_init(void(*ptr)(int argc, char** argv));
int      kernel_spawn(void (*entry)(int argc, char**argv),  char**argv, fds_t* fds);
int      kernel_load( unsigned* image, unsigned imageSize, void (*entry)(int,char**), char** argv, fds_t* fds, int clone_cwd);
void     kernel_die(int status);
void     kernel_spinLock(void* ptr);
void     kernel_unlock(void* ptr);
void     kernel_threadBlocked();

int      kernel_setThreadInfo(thread_info_t type, unsigned info);

window_h      kernel_getThreadWindow();
unsigned kernel_getPid();
fds_t*   kernel_getFds();
int      kernel_getExitStatus(unsigned pid);
int      kernel_getPidForStdout(unsigned fd);
unsigned kernel_getIsActiveImage(void* ptr);
int      kernel_getIsThreadAlive(unsigned pid);

void _kernel_newlib_lock_init_recursive(_LOCK_RECURSIVE_T* lock);
void _kernel_newlib_lock_close_recursive(_LOCK_RECURSIVE_T* lock);
unsigned _kernel_newlib_lock_acquire_recursive(_LOCK_RECURSIVE_T* lock);
unsigned _kernel_newlib_lock_try_acquire_recursive(_LOCK_RECURSIVE_T* lock);
void _kernel_newlib_lock_release_recursive(_LOCK_RECURSIVE_T* lock);

void _kernel_newlib_lock_init(unsigned* lock);
void _kernel_newlib_lock_close(unsigned* lock);
unsigned _kernel_newlib_lock_acquire(unsigned* lock);
unsigned _kernel_newlib_lock_try_acquire(unsigned* lock);
void _kernel_newlib_lock_release(unsigned* lock);

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
#define thread_window(i) _bft->kernel_setThreadInfo(KERNEL_THREAD_WINDOW, (unsigned)i)
#define thread_getWindow() _bft->kernel_getThreadWindow()
#define thread_blocked() _bft->kernel_threadBlocked()
#define die(x) _bft->kernel_die(x)
#define spawn(x,v,f) _bft->kernel_spawn(x,v,f)
#define kernel_getFds(x) _bft->kernel_getFds(x)

#endif
