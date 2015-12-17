#include <sys/syslimits.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include "kernel.h"
#include "peripheral.h"
#include "panic.h"
#include "argv.h"
#include "simulator.h"
#include "memory.h"
#include "console.h"

extern void* 
malloc(unsigned);

extern void 
free(void*ptr);

extern void 
_kernel_resume_asm(unsigned int *sp);

extern unsigned 
_kernel_atomic_lock_asm(void* ptr);


#define _thread_max 8
#define _thread_historyMax 20 

//#define _thread_stack_size_bytes  (0x2800000)
#define _thread_stack_size_bytes  (0x200000)
#define _thread_stack_size_words  (_thread_stack_size_bytes/4)
#define _KERNEL_STACK_CANARY 0xDEADBEEF

enum {
    _THREAD_DEAD = 0,
    _THREAD_RUNNING = 1,
    _THREAD_WAIT = 2,
    _THREAD_BLOCKED = 3,
};

static char* states[] = {
  "D", "R", "W", "B"
};

typedef struct {
  thread_h tid;
  unsigned state;
  unsigned *sp;
  window_h window;
  unsigned* thread_stack;
  unsigned* image;
  int argc;
  char** argv;
  unsigned imageSize;
  fds_t fd;
  char cwd[PATH_MAX];
} _thread_entry_t;

typedef struct {
  int exitStatus;
  thread_h tid;
  unsigned _stdout;
} _thread_history_t;

int errno;
static unsigned nextTid = 1;
static _thread_entry_t threadTable[_thread_max];
static _thread_history_t threadHistory[_thread_historyMax];
static unsigned int currentThread;
static unsigned int currentThreadSave;
static volatile unsigned kernel_threadMax = _thread_max;

static inline unsigned _kernel_disableInts();
static inline void _kernel_enableInts(unsigned);

#define _threadTable_lock() unsigned ___ints_disabled = _kernel_disableInts()
#define _threadTable_unlock() _kernel_enableInts(___ints_disabled)


unsigned 
kernel_enterKernelMode()
{
  unsigned ___ints_disabled = _kernel_disableInts();
  currentThreadSave = currentThread;
  currentThread = 0;
  return ___ints_disabled;
}


void
kernel_exitKernelMode(unsigned ___ints_disabled)
{
  currentThread = currentThreadSave;
  _kernel_enableInts(___ints_disabled);  
}


static void 
_kernel_scheduleFromBlocked()
{
  unsigned int i, previous = currentThread;

  for (i = (currentThread+1) % _thread_max; i != currentThread; i = (i+1) % _thread_max) {
    if (threadTable[i].state == _THREAD_WAIT) {
      break;
    }
  }

  currentThread = i; 
  
  if (currentThread == previous) {
    ktrace_simulator_yield();
    peripheral.simulator.yieldOnRTE = 1;
  } else {
    ktrace_current_thread();
  }

  threadTable[currentThread].state = _THREAD_RUNNING;
}


static void 
_kernel_schedule()
{
  unsigned int i;

  for (i = 0; i < _thread_max; ++i) {
    if (threadTable[i].state == _THREAD_BLOCKED) {
      threadTable[i].state = _THREAD_WAIT;
    }
  }

  
  for (i = (currentThread+1) % _thread_max; i != currentThread; i = (i+1) % _thread_max) {
    if (threadTable[i].state == _THREAD_WAIT) {
      break;
    }
  }
  
  currentThread = i; 
  ktrace_current_thread();

  threadTable[currentThread].state = _THREAD_RUNNING;
}


static thread_h 
_kernel_threadCreate(unsigned int threadIndex, unsigned* image, unsigned imageSize, void(* ptr)(int,char**),  char**argv, fds_t* fds)
{
  _thread_entry_t *entry = &threadTable[threadIndex];
  entry->tid = (thread_h)nextTid++;
  entry->argc = argv_argc(argv);
  entry->argv = argv;
  entry->image = image;
  entry->imageSize = imageSize;
  entry->state = _THREAD_WAIT;
  entry->window = 0;
  entry->cwd[0] = '/';
  entry->cwd[1] = 0;

  if (fds != 0) {
    entry->fd._stdin = fds->_stdin;
    entry->fd._stdout = fds->_stdout;
    entry->fd._stderr = fds->_stderr;
  } else {
    entry->fd._stdin = STDIN_FILENO;
    entry->fd._stdout = STDOUT_FILENO;
    entry->fd._stderr = STDERR_FILENO;
  }

  entry->thread_stack[(_thread_stack_size_words)-1] = 0x00; // SR - all ints enabled

  if (ptr != 0) {
    entry->thread_stack[(_thread_stack_size_words)-2] = ((unsigned int)ptr); // PC
    entry->thread_stack[(_thread_stack_size_words)-10] = (unsigned) entry->argc; // R4
    entry->thread_stack[(_thread_stack_size_words)-11] = (unsigned) argv; // R5
  }
  
  entry->sp = (unsigned int*)&entry->thread_stack[(_thread_stack_size_words)-38];

  return entry->tid;
}


static int 
_kernel_threadAllocate()
{
  unsigned int i;
  for (i = 0; i < _thread_max; i++) {
    if (threadTable[i].state == _THREAD_DEAD) {
      return i;
    }
  }

  return -1;
}


static int
_kernel_threadSetInfo(unsigned threadIndex, thread_info_t type, unsigned info)
{
  _thread_entry_t *entry = &threadTable[threadIndex];
  switch (type) {
  case KERNEL_THREAD_WINDOW:
    entry->window = (window_h)info;
    return 1;
  case KERNEL_CURRENT_WORKING_DIRECTORY:
    strncpy(threadTable[currentThread].cwd, (char*)info, PATH_MAX);
    return 1;
  }

  return 0;
}


static unsigned 
_kernel_isArgvAddress(char* address, char** vector)
{
  register char **scan;
  
  if (address == *vector) {
    return 1;
  }
  if (vector != NULL) {
    for (scan = vector; *scan != NULL; scan++) {
      if (*scan == address) {
	return 1;
      }
    }
  }
  return 0;
}


static inline unsigned 
_kernel_disableInts()
{
  register unsigned r0 __asm__("r0");

  __asm__ volatile (
	  "stc sr,r0\n"
          :::"r0", "memory");

  if ((r0 & 0xF0) == 0xF0) { // Already disabled
    return 0;
  }
  
  __asm__ volatile (
	  "or #0xF0,r0\n"
	  "ldc r0,sr"
	  :
	  :
          :"r0", "memory");

  return 1;
}


static inline void 
_kernel_enableInts(unsigned enable)
{
  if (enable) {

#ifdef _KERNEL_ASSERTS
    register unsigned r0 __asm__("r0");
  __asm__ volatile (
	  "stc sr,r0\n"
          :::"r0", "memory");

  if ((r0 & 0xF0) != 0xF0) { // Already enabled
    panic("_kernel_enableInts: already enabled");
  }
#endif

    __asm__ volatile ("stc sr,r0\n"
		      "and #0xFFFFFF0F,r0\n"
		      "ldc r0,sr"
		      :
		      :
		      :"r0", "memory");
  }
}


/* called from kernel_asm.S via simulated clock tick - level 15 interrupt (ints disabled) */
void 
_from_asm_kernel_tick(unsigned int *sp)
{
  threadTable[currentThread].sp = sp;
  threadTable[currentThread].state = _THREAD_WAIT;

  _kernel_schedule();

  if (threadTable[currentThread].thread_stack[0] != _KERNEL_STACK_CANARY) {
        panic("_from_asm_kernel_tick: dead canary");
   }

  _kernel_resume_asm(threadTable[currentThread].sp);
}


/* called from kernel_asm.S via INT_TRAPA36 - ints disabled before this is called */
void 
_from_asm_kernel_blocked(unsigned int *sp)
{
  threadTable[currentThread].sp = sp;
  ktrace_thread_blocked();
  threadTable[currentThread].state = _THREAD_BLOCKED;
  _kernel_scheduleFromBlocked();
  if (threadTable[currentThread].thread_stack[0] != _KERNEL_STACK_CANARY) {
    panic("_from_asm_kernel_blocked: dead canary");
  }
  _kernel_resume_asm(threadTable[currentThread].sp);
}


/* called from kernel_asm.S via INT_TRAPA37 - ints disabled before this is called */
void 
_from_asm_kernel_kill(int status, int context)
{
  _thread_entry_t *entry = &threadTable[currentThread];
  unsigned long _currentThreadSave = currentThread;
  currentThread = 0;

  unsigned i;
  for (i = 0; i < _thread_historyMax; i++) {
    if (threadHistory[i].tid == 0) {
      threadHistory[i].exitStatus = status;
      threadHistory[i].tid = entry->tid;
      threadHistory[i]._stdout = entry->fd._stdout;
      break;
    }
  }

  if (i == _thread_historyMax) {
    panic("_from_asm_kernel_kill: exceeded thread history limits");
  }    
    
  if (entry->argv != 0) {
      argv_free(entry->argv);
      entry->argv = 0;
  }

  if (entry->image != 0) {
    free(entry->image);
    entry->image = 0;
    // I think we might be able to call this for everyone now as only "client" allocations are tracked
    // We only want to do this if we are in "client" world, otherwise we will be free'ing shared crap
        memory_cleanupThread(entry->tid); // TODO: this is not the definitive way to say we are in client world [ better way will be to only save client mallocs' ]
  }
  
  //  memory_cleanupThread(entry->tid); 

  entry->state = _THREAD_DEAD;
  entry->tid = 0;
  memset(&entry->fd, 0, sizeof(entry->fd));

  currentThread = _currentThreadSave;

  _kernel_schedule();

  if (threadTable[currentThread].thread_stack[0] != _KERNEL_STACK_CANARY) {
    panic("_from_asm_kernel_kill:  dead canary");
  }
  
  _kernel_resume_asm(threadTable[currentThread].sp);
}


void 
kernel_init(void(*ptr)(int argc, char** argv))
{
  ktrace_reset();
  currentThread = 0;
  for (int i = 0; i < _thread_max; i++) {
    threadTable[i].thread_stack = malloc(_thread_stack_size_bytes);
    threadTable[i].thread_stack[0] = _KERNEL_STACK_CANARY;
  }
  _kernel_threadCreate(currentThread, 0, 0, ptr, argv_build("kernel"), 0);
  _kernel_resume_asm(threadTable[currentThread].sp);
}


static int 
_kernel_getIsThreadAlive(thread_h tid)
{
  _threadTable_lock();
  
  int result = 0;
  for (unsigned i = 0; i < _thread_max; i++) {
    if (threadTable[i].tid == tid) {
      result = threadTable[i].state != _THREAD_DEAD;    
      break;
    }
  }
  
  _threadTable_unlock();
  
  return result;
}


unsigned
kernel_threadGetIsActiveImage(void* ptr)
{
   for (int i = 0; i < _thread_max; i++) {
    _thread_entry_t *entry = &threadTable[i];     
      if (entry->image == ptr) {
	return 1;
      }
      if (_kernel_isArgvAddress(ptr, entry->argv)) {
	return 1;
      }
   }
   
   return 0;
}


unsigned
kernel_disableInts()
{
  return _kernel_disableInts();
}


void
kernel_enableInts(unsigned enable)
{
  _kernel_enableInts(enable);
}


int
kernel_threadSetInfo(thread_info_t type, unsigned info)
{
  _threadTable_lock();
  int result = _kernel_threadSetInfo(currentThread, type, info);
  _threadTable_unlock();
  return result;
}


thread_h
kernel_threadSpawn(void (*entry)(int, char**argv), char**argv, fds_t* fds)
{
  _threadTable_lock();
  int threadIndex = _kernel_threadAllocate();
  thread_h tid = INVALID_THREAD;
  if (threadIndex > 0) {
    tid = _kernel_threadCreate(threadIndex, 0, 0,  entry,  argv, fds);
    //_kernel_disableInts();
  }
  _threadTable_unlock();

  return tid;
}


thread_h
kernel_threadLoad(unsigned* image, unsigned imageSize, void (*entry)(int,char**),  char** argv, fds_t* fds, int clone_cwd)
{
  _threadTable_lock();
  int threadIndex = _kernel_threadAllocate();
  thread_h tid = INVALID_THREAD;
  if (threadIndex > 0) {
    tid = _kernel_threadCreate(threadIndex, image, imageSize, entry, argv, fds);
    if (clone_cwd) {
      strncpy(threadTable[threadIndex].cwd, threadTable[currentThread].cwd, PATH_MAX);      
    }
  }
  _threadTable_unlock();
  
  return tid;
}


void
kernel_threadDie(int status)
{
  __asm__ volatile("trapa #37":::"memory");
}


void
kernel_threadBlocked()
{
  register unsigned r1 __asm__("r1");
  
  __asm__ volatile (
		    "stc sr,r1\n"
		    :::"r1", "memory");
  
  if ((r1 & 0xF0) == 0xF0) { // Already disabled
    panic("kernel_threadBlocked: called with interupts disabled");
  }
  
  __asm__ volatile("trapa #36":::"memory");
}


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


window_h
kernel_threadGetWindow()
{
  _threadTable_lock();
  window_h result = threadTable[currentThread].window;
  _threadTable_unlock();
  return result;
}


thread_h
kernel_threadGetId()
{
  _threadTable_lock();
  thread_h tid = threadTable[currentThread].tid;
  _threadTable_unlock();
  return tid;
}


fds_t*
kernel_threadGetFds()
{
  _threadTable_lock();
  fds_t* fds = &threadTable[currentThread].fd;
  _threadTable_unlock();
  return fds;
}


char*
kernel_getcwd(char *buf, unsigned size)
{
  if (buf == 0) {
    buf = malloc(PATH_MAX);
    size = PATH_MAX;
  }

  strncpy(buf, threadTable[currentThread].cwd, size);
  return buf;
}


unsigned
kernel_times (struct tms *tp)
{
  /*  struct tms {
    clock_t tms_utime;
    clock_t tms_stime;
    clock_t tms_cutime;
    clock_t tms_cstime;
  };

  The elements of this structure are defined as follows:

     tms_utime   The CPU time charged for the execution of user instructions.

     tms_stime   The CPU time charged for execution by the system on behalf of the process.

     tms_cutime  The sum of the tms_utimes and tms_cutimes of the child processes.

     tms_cstime  The sum of the tms_stimes and tms_cstimes of the child processes.
     printf("_times_r called\n");*/

  
  
  tp->tms_utime = peripheral.time.elapsedMilliSeconds;
  tp->tms_stime = 1;
  tp->tms_cutime = 1;
  tp->tms_cstime = 1;

  return peripheral.time.elapsedMilliSeconds;
}


thread_h
kernel_threadGetIdForStdout(unsigned fd)
{
  _threadTable_lock();
  for (unsigned i = 0; i < _thread_max; i++) {
    if (threadTable[i].fd._stdout == (int)fd) {
      thread_h tid = threadTable[i].tid;
      _threadTable_unlock();
      return tid;
    }
  }

  for (unsigned i = 0; i < _thread_historyMax; i++) {
    if (threadHistory[i].tid  != 0 && threadHistory[i]._stdout == fd) {
      thread_h tid = threadHistory[i].tid;
      _threadTable_unlock();
      return tid;
    }
  }

  return INVALID_THREAD;
}


int
kernel_threadGetExitStatus(thread_h tid)
{
  _threadTable_lock();
  int exitStatus = -1;
  for (unsigned i = 0; i < _thread_historyMax; i++) {
    _thread_history_t *entry = &threadHistory[i];
    if (entry->tid == tid) {
      entry->tid = 0;
      exitStatus =  entry->exitStatus;
      break;
    }
  }
  _threadTable_unlock();
    
  return exitStatus;
}


void
kernel_stats()
{
  _threadTable_lock();
  for (int i = 0; i < _thread_max; i++) {
    _thread_entry_t *entry = &threadTable[i];
    //    if (entry->state != _THREAD_DEAD) {
      char* name = "";
      if (entry->state != _THREAD_DEAD) {
	if (entry->argc > 0) {
	  name = entry->argv[0];
	}
      }
      printf("%d: %s %d i: %X %X s: %X %X %X %s\n", (int)entry->tid,  states[entry->state], entry->fd._stdout, (unsigned)entry->image, (unsigned)(entry->image) + entry->imageSize, (unsigned)&entry->thread_stack[0], (unsigned)entry->sp, (unsigned)((char*)&entry->thread_stack[0])+_thread_stack_size_bytes, name);
      //    }
  }
  _threadTable_unlock();
}


void 
_exit(int c)
{
  panic("_exit: exit called in kernel_context");
  for(;;);
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
    if (lock->count == 0) { //? Never locked maybe ?
      // panic("_kernel_newlib_lock_release_recursive: lock->thread != tid (count = 0)");
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

int 
kernel_threadWait(thread_h tid)
{
  while (_kernel_getIsThreadAlive(tid)) {
    kernel_threadBlocked();
  }

  return kernel_threadGetExitStatus(tid);
}
