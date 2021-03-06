#pragma once

#include "types.h"
#include "bft.h"

struct tms;

#ifdef _KERNEL_BUILD

#include "kernel_asserts.h"

#define kernel_memoryBarrier() do { asm volatile ("" : : : "memory"); } while (0)
#define kernel_threadSetWindow(w) kernel_threadSetInfo(KERNEL_THREAD_WINDOW, (unsigned)w)


unsigned 
kernel_times (struct tms *tp);

char* 
kernel_getcwd(char *buf, unsigned size);

thread_status_t**
kernel_threadGetStats();

void
kernel_threadFreeStats(thread_status_t** stats);

void     
kernel_enableInts(unsigned enable);

unsigned 
kernel_disableInts();

void     
kernel_init(int (*ptr)(int argc, char** argv), const char* version);

thread_h    
kernel_threadSpawn(int (*entry)(int argc, char**argv),  char**argv, fds_t* fds);

thread_h    
kernel_threadLoad( unsigned* image, unsigned imageSize, int (*entry)(int,char**), char** argv, fds_t* fds, int clone_cwd);

void     
kernel_threadBlocked();

int 
kernel_threadWait(thread_h pid);

void    
kernel_threadDie(int status)  __attribute__ ((noreturn));

int      
kernel_threadSetInfo(thread_info_t type, unsigned info);

window_h      
kernel_threadGetWindow();

thread_h    
kernel_threadGetId();

fds_t*   
kernel_threadGetFds();

thread_h    
kernel_threadGetIdForStdout(unsigned fd);

unsigned 
kernel_threadGetIsActiveImage(void* ptr);

void
kernel_threadKill(thread_h tid);

void
kernel_threadQueueMessageHandler(thread_h tid, message_handler_t handler, int id, thread_h sender, void* data);

void 
kernel_assertKernelMode(unsigned pr);

void     
kernel_spinLock(void* ptr);

void     
kernel_unlock(void* ptr);


message_handler_t*
kernel_threadGetMessageHandler(thread_h tid);

const char*
kernel_version();


#define KERNEL_MODE() unsigned __kernelMode = kernel_enterKernelMode();
#define USER_MODE() kernel_exitKernelMode(__kernelMode);

#else //_KERNEL_BUILD

#define kernel_threadGetStats() _bft->kernel_threadGetStatus()
#define kernel_threadFreeStats(x) _bft->kernel_threadFreeStats(x)
#define malloc_stats() _bft->malloc_stats()
#define kernel_threadBlocked() _bft->kernel_threadBlocked()
#define kernel_threadSetWindow(i) _bft->kernel_threadSetInfo(KERNEL_THREAD_WINDOW, (unsigned)i)
#define kernel_threadGetWindow() _bft->kernel_threadGetWindow()
#define kernel_threadDie(x) _bft->kernel_threadDie(x)
#define kernel_threadSpawn(x,v,f) _bft->kernel_threadSpawn(x,v,f)
#define kernel_threadGetFds(x) _bft->kernel_threadGetFds(x)

#endif



extern const int kernel_messageMax;

unsigned 
kernel_enterKernelMode();

void     
kernel_exitKernelMode(unsigned ___ints_disabled);

thread_h    
kernel_threadGetIdForStdout(unsigned fd);

int
kernel_threadGetStdoutForId(thread_h tid);

int      
kernel_threadGetExitStatus(thread_h tid);
