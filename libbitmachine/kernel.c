#include <stddef.h>
#include <sys/syslimits.h> // PATH_MAX
#include <string.h>        // strcpy,strlcpy
#include "kernel.h"
#include "peripheral.h"
#include "panic.h"
#include "argv.h"
#include "simulator.h"
#include "memory.h"
#include "window.h"
#include "threadcontext.h"

extern void 
_kernel_resume_asm(unsigned int *sp);


#define _thread_max 16
#define _thread_historyMax 32
#define _message_max 64

#define _thread_stack_size_bytes  (0x100000)
//#define _thread_stack_size_bytes  (0x10000)
#define _thread_stack_size_words  (_thread_stack_size_bytes/4)
#define _KERNEL_STACK_CANARY 0xDEADBEEF

static volatile unsigned _kernel_thread_stack_size_bytes = _thread_stack_size_bytes;

const char* _version;

typedef struct {
  thread_h tid;
  unsigned state; // TODO: thread_state_t
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

typedef struct {
  message_handler_t handlers[_message_max];
} _thread_message_handler_t;


int errno;
static unsigned nextTid = 1;
static _thread_entry_t threadTable[_thread_max];
static _thread_history_t threadHistory[_thread_historyMax];
static _thread_message_handler_t _threadMessageHandlers[_thread_max];
static unsigned int currentThread;
static unsigned int currentThreadSave;
const int kernel_messageMax = _message_max;

// This is only accessed from the simulator
static volatile unsigned kernel_threadMax = _thread_max;

static inline unsigned _kernel_disableInts();
static inline void _kernel_enableInts(unsigned);
static inline unsigned _kernel_getSR();

#define _threadTable_lock() unsigned ___ints_disabled = _kernel_disableInts()
#define _threadTable_unlock() _kernel_enableInts(___ints_disabled)

#define JMP_kernel_resume_asm(sp)   __asm__ volatile(\
		   "mov.l %0,r4\n"	\
		   "mov.l kernel_resume_asm%=,r0\n"\
		   "jmp @r0\n"\
		   "nop\n"\
		   ".align 4\n"\
		   "kernel_resume_asm%=:\n"\
		   ".long __kernel_resume_asm"\
		   ::"m"(sp):"r0","r4");	      


unsigned 
kernel_enterKernelMode()
{
  unsigned modeFlag = _kernel_disableInts() << 1 | (currentThread != 0);
  currentThreadSave = currentThread;
  currentThread = 0;
  kernel_memoryBarrier();
  return modeFlag;
}


void
kernel_exitKernelMode(unsigned modeFlag)
{
  unsigned exitMode = modeFlag & 1;
  unsigned intsDisabled = modeFlag & 2;
  if (exitMode) {
    currentThread = currentThreadSave;
    kernel_memoryBarrier();
  }
  _kernel_enableInts(intsDisabled);  
}


static inline unsigned
_kernel_getSR()
{
  volatile unsigned sr;
  __asm__ volatile ("stc sr,r0\n"
		    "mov.l r0,%0" 
		    :"=m"(sr)
		    :
		    :"r0", "memory");
  return sr;
}


static unsigned 
_kernel_scheduleFromBlocked()
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  unsigned int i, previous = currentThread;

  for (i = (currentThread+1) % _thread_max; i != currentThread; i = (i+1) % _thread_max) {
    if (threadTable[i].state == _THREAD_WAIT) {
      break;
    }
  }

  if (i == previous) {
    peripheral.simulator.yieldOnRTE = 1;
  }

  threadTable[i].state = _THREAD_RUNNING;
  return i;
}


static unsigned 
_kernel_schedule()
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

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
  
  threadTable[i].state = _THREAD_RUNNING;
  return i;
}


static void *
_kernel_memset(void *v,
	       int c,
	       size_t length)
{
  char* p = v;
  for (unsigned i = 0; i < length; i++) {
    *p++ = c;
  }
  return v;
}


static char** 
_kernel_argvDup(char** argv)
{
  int argc = argv_argc(argv);

  if (argv != 0) {
    char **vector = memory_kmalloc((argc+1)*sizeof(char*));
    int i;
    for (i = 0;i  <  argc; i++) {
      vector[i] = memory_kmalloc(strlen(argv[i])+1);
      strcpy(vector[i], argv[i]);
    }
    vector[i] = 0;
    return vector;
  }

  return 0;
}


void 
_kernel_argvFree (char **vector)
{
  if (vector != NULL) {
    for (char** scan = vector; *scan != NULL; scan++) {
      memory_free(*scan);
    }
    memory_free(vector);
  }
}


static thread_h 
_kernel_threadCreate(unsigned int threadIndex, unsigned* image, unsigned imageSize, int(* ptr)(int,char**),  char**argv, fds_t* fds)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  _thread_entry_t *entry = &threadTable[threadIndex];
  entry->tid = (thread_h)nextTid++;
  entry->argc = argv_argc(argv);
  entry->argv = _kernel_argvDup(argv);
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

  entry->sp = (unsigned int*)&entry->thread_stack[(_thread_stack_size_words)-STACK_OFFSET_BASE];

  entry->sp[STACK_OFFSET_SR] =  0x00; // SR - all ints enabled

  if (ptr != 0) {
    entry->sp[STACK_OFFSET_PC] = ((unsigned int)ptr); // PC
    entry->sp[STACK_OFFSET_R4] = (unsigned) entry->argc; // R4
    entry->sp[STACK_OFFSET_R5] = (unsigned) argv; // R5
  }

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
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  _thread_entry_t *entry = &threadTable[threadIndex];
  switch (type) {
  case KERNEL_THREAD_WINDOW:
    entry->window = (window_h)info;
    return 1;
  case KERNEL_CURRENT_WORKING_DIRECTORY:
    strlcpy(threadTable[threadIndex].cwd, (char*)info, PATH_MAX);
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
  if ((_kernel_getSR() & 0xF0) == 0xF0) { // Already disabled
    KERNEL_ASSERT_INTERRUPTS_DISABLED();
    return 0;
  }
  
  __asm__ volatile (
	  "stc sr,r0\n"
	  "or #0xF0,r0\n"
	  "ldc r0,sr"
	  :
	  :
          :"r0", "memory");

  KERNEL_ASSERT_INTERRUPTS_DISABLED();
  return 1;
}


static inline void 
_kernel_enableInts(unsigned enable)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED()

  if (enable) {

    __asm__ volatile ("stc sr,r0\n"
		      "and #0xFFFFFF0F,r0\n"
		      "ldc r0,sr"
		      :
		      :
		      :"r0", "memory");

    KERNEL_ASSERT_INTERRUPTS_ENABLED();
  }
}


void 
kernel_assertKernelMode(unsigned pr)
{
#ifdef _KERNEL_ASSERTS
  if (currentThread != 0) {    
    panic("kernel_assertKernelMode: not in kernel mode");
  }
#endif
}

static inline void
_kernel_checkStack()
{
#ifdef _KERNEL_ASSERTS
  if (threadTable[currentThread].thread_stack[0] != _KERNEL_STACK_CANARY) {
    panic("kernel assesrtion failed: dead canary");
  }

  if (threadTable[currentThread].sp < &threadTable[currentThread].thread_stack[0] ||
      threadTable[currentThread].sp >= &threadTable[currentThread].thread_stack[_thread_stack_size_words]) {
    panic("kernel assesrtion failed: invalid stack pointer");
  }
#endif
}


/* called from kernel_asm.S via simulated clock tick - level 15 interrupt (ints disabled) */
void 
_from_asm_kernel_tick(unsigned int *sp)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  threadTable[currentThread].sp = sp;
  threadTable[currentThread].state = _THREAD_WAIT;

  _kernel_checkStack();
 
  currentThread = _kernel_schedule();
  kernel_memoryBarrier();

  JMP_kernel_resume_asm(threadTable[currentThread].sp);
}


/* called from kernel_asm.S via INT_TRAPA36 - ints disabled before this is called */
void 
_from_asm_kernel_blocked(unsigned int *sp)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  threadTable[currentThread].sp = sp;
  threadTable[currentThread].state = _THREAD_BLOCKED;

  _kernel_checkStack();

  currentThread = _kernel_scheduleFromBlocked();
  kernel_memoryBarrier();

  JMP_kernel_resume_asm(threadTable[currentThread].sp);
}


static void 
_kernel_threadKill(int status, int saveHistory)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  _thread_entry_t *entry = &threadTable[currentThread];

  //  unsigned long _currentThreadSave = currentThread;
  //  kernel_memoryBarrier();
  
  if (saveHistory) {
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
  }
    
  if (entry->argv != 0) {
    _kernel_argvFree(entry->argv);
    entry->argv = 0;
  }

  if (entry->image != 0) {
    memory_free(entry->image);
    entry->image = 0;
    memory_cleanupThread(entry->tid); 
  }
  

  entry->state = _THREAD_DEAD;
  entry->tid = 0;
  _kernel_memset(&entry->fd, 0, sizeof(entry->fd));
  _kernel_memset(&_threadMessageHandlers[currentThread].handlers, 0, sizeof(_threadMessageHandlers[currentThread].handlers));


  //  currentThread = _currentThreadSave;
  //  kernel_memoryBarrier();

  _kernel_checkStack();
}


/* called from kernel_asm.S via INT_TRAPA37 - ints disabled before this is called */
void 
_from_asm_kernel_kill(int status, int context)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  _kernel_threadKill(status, 1);
  currentThread = _kernel_schedule();
  kernel_memoryBarrier();
  JMP_kernel_resume_asm(threadTable[currentThread].sp);
}


static inline int 
_kernel_threadGetIndex(thread_h tid)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  for (unsigned i = 0; i < _thread_max; i++) {
    if (threadTable[i].tid == tid) {
      return i;
    }
  }

  return -1;
}


/* called from kernel_asm.S via INT_TRAPA38 - ints disabled before this is called */
void
_from_asm_kernel_kill_thread(unsigned* sp, thread_h tid)
{
  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  threadTable[currentThread].sp = sp;
  threadTable[currentThread].state = _THREAD_BLOCKED;

  int threadIndex = _kernel_threadGetIndex(tid);

  if (threadIndex == -1) {
    panic("_from_asm_kernel_kill_thread: thread not found");
  } else {
    currentThread = threadIndex;
    kernel_memoryBarrier();
  }

  fds_t* fds = &threadTable[currentThread].fd;

  extern void __file_close(int file);

  if (fds->_stdin != STDIN_FILENO) {
    __file_close(fds->_stdin);
  }
  if (fds->_stdout != STDOUT_FILENO) {
    __file_close(fds->_stdout);
  }
  if (fds->_stderr != STDERR_FILENO) {
    __file_close(fds->_stderr);
  }

  window_cleanup(tid);

  _kernel_threadKill(1, 0);

  currentThread = _kernel_schedule();
  kernel_memoryBarrier();
  JMP_kernel_resume_asm(threadTable[currentThread].sp);
}


const char* 
kernel_version()
{
  return _version;
}


void
kernel_init(int (*ptr)(int argc, char** argv), const char* version)
{
  _version = version;
  currentThread = 0;
  for (int i = 0; i < _thread_max; i++) {
    threadTable[i].thread_stack = memory_kmalloc(_thread_stack_size_bytes);
    threadTable[i].thread_stack[0] = _KERNEL_STACK_CANARY;
  }

  char** argv = simulator_kernelArgv();

  _kernel_threadCreate(currentThread, 0, 0, ptr, argv, 0);
  JMP_kernel_resume_asm(threadTable[currentThread].sp);
}


static int 
_kernel_getIsThreadAlive(thread_h tid)
{
  _threadTable_lock();

  KERNEL_ASSERT_INTERRUPTS_DISABLED();

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
kernel_threadSpawn(int (*entry)(int, char**argv), char**argv, fds_t* fds)
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
kernel_threadLoad(unsigned* image, unsigned imageSize, int (*entry)(int,char**),  char** argv, fds_t* fds, int clone_cwd)
{
  _threadTable_lock();
  int threadIndex = _kernel_threadAllocate();
  thread_h tid = INVALID_THREAD;
  if (threadIndex > 0) {
    tid = _kernel_threadCreate(threadIndex, image, imageSize, entry, argv, fds);
    if (clone_cwd) {
      strlcpy(threadTable[threadIndex].cwd, threadTable[currentThread].cwd, PATH_MAX);      
    }
  }
  _threadTable_unlock();
  
  return tid;
}


void
kernel_threadDie(int status)
{
  __asm__ volatile("trapa #37":::"memory");
  for(;;);
}


void
kernel_threadBlocked()
{
  KERNEL_ASSERT_INTERRUPTS_ENABLED();
  
  __asm__ volatile("trapa #36":::"memory");
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


int
kernel_threadGetStdoutForId(thread_h tid)
{
  _threadTable_lock();

  for (unsigned i = 0; i < _thread_max; i++) {
    if (threadTable[i].tid == tid) {
      int _stdout = threadTable[i].fd._stdout;
      _threadTable_unlock();
      return _stdout;
    }
  }

  for (unsigned i = 0; i < _thread_historyMax; i++) {
    if (threadHistory[i].tid == tid) {
      int _stdout = threadHistory[i]._stdout;
      _threadTable_unlock();
      return _stdout;
    }
  }

  _threadTable_unlock();
  return -1;
}


char*
kernel_getcwd(char *buf, unsigned size)
{
  if (buf == 0) {
    buf = memory_malloc(PATH_MAX);
    size = PATH_MAX;
  }

  strlcpy(buf, threadTable[currentThread].cwd, size);
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
  */

  
  
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

  _threadTable_unlock();

  return INVALID_THREAD;
}


static int
_kernel_threadGetExitStatus(thread_h tid, int* status)
{
  _threadTable_lock();

  KERNEL_ASSERT_INTERRUPTS_DISABLED();

  int success = 0;

  for (unsigned i = 0; i < _thread_historyMax; i++) {
    _thread_history_t *entry = &threadHistory[i];
    if (entry->tid == tid) {
      entry->tid = 0;
      *status =  entry->exitStatus;
      success = 1;
      break;
    }
  }
  _threadTable_unlock();
    
  return success;
}


int 
kernel_threadGetExitStatus(thread_h tid)
{
  int status = -1, success, count = 0;

  do {
    success = _kernel_threadGetExitStatus(tid, &status);
    if (!success) {
      kernel_threadBlocked();
    }
  } while (!success && count++ < 10); // TODO: bemacs exit doesn't work

  return status;
}


thread_status_t**
kernel_threadGetStats()
{
  _threadTable_lock();

  unsigned numThreads = 0;
  for (int i = 0; i < _thread_max; i++) {
    _thread_entry_t *entry = &threadTable[i];
    if (entry->state != _THREAD_DEAD) {
      numThreads++;
    }
  }


  thread_status_t** status = memory_kmalloc((sizeof(thread_status_t*)*(numThreads+1)));
  status[numThreads] = 0;

  for (int i = 0, y = 0; i < _thread_max; i++) {
    _thread_entry_t *entry = &threadTable[i];
    if (entry->state != _THREAD_DEAD) {
      int argvLen = strlen(entry->argv[0]);
      status[y] = memory_kmalloc(sizeof(thread_status_t));
      status[y]->state = entry->state;
      status[y]->tid = entry->tid;
      status[y]->name = memory_kmalloc(argvLen+1);
      strlcpy(status[y]->name, entry->argv[0], argvLen+1);
      y++;
    }
  }

  _threadTable_unlock();

  return status;
}


void
kernel_threadFreeStats(thread_status_t** stats)
{
  if (stats) {
    for (int i = 0; stats[i] != 0; i++) {
      memory_free(stats[i]->name);
      memory_free(stats[i]);
    }
    memory_free(stats);
  }
}


void 
_exit(int c)
{
  panic("_exit: exit called in kernel_context");
  for(;;);
}


int 
kernel_threadWait(thread_h tid)
{
  KERNEL_ASSERT_INTERRUPTS_ENABLED();

  while (_kernel_getIsThreadAlive(tid)) {
    KERNEL_ASSERT_INTERRUPTS_ENABLED();
    kernel_threadBlocked();
    KERNEL_ASSERT_INTERRUPTS_ENABLED();
  }


  KERNEL_ASSERT_INTERRUPTS_ENABLED();

  return kernel_threadGetExitStatus(tid);
}


message_handler_t*
kernel_threadGetMessageHandler(thread_h tid)
{
  _threadTable_lock();
  
  for (int i = 0; i < _thread_max; i++) {
    _thread_entry_t *entry = &threadTable[i];
    if (entry->tid == tid) {
      _threadTable_unlock();
      return _threadMessageHandlers[i].handlers;
    }
  }

  _threadTable_unlock();
  return NULL;
}

void
kernel_threadKill(thread_h tid)
{
  __asm__ volatile ("mov.l %0,r5" 
		    :
		    :"m"(tid)
		    :"r5");
  __asm__ volatile("trapa #38":::"memory");
}


static  void
_kernel_threadExecMessageHandler(message_handler_t handler, int id, thread_h sender, unsigned* sp, void* data)
{
  handler(id, sender, data);
  JMP_kernel_resume_asm(sp);  
  
}


void
kernel_threadQueueMessageHandler(thread_h tid, message_handler_t handler, int id, thread_h sender, void* data)
{
  _threadTable_lock();

  int threadIndex = _kernel_threadGetIndex(tid);

  if (threadIndex == -1) {
    _threadTable_unlock();
    return;
  }

  unsigned *sp = threadTable[threadIndex].sp;
  unsigned *save = sp;
  void (*fp)(message_handler_t, int, thread_h, unsigned*, void*) = _kernel_threadExecMessageHandler; 

  *(--sp) = (unsigned)data;       // Arg5 goes on the stack
  unsigned *stack = sp;
  

  *(--stack) = sp[STACK_OFFSET_SR]; 
  *(--stack) = (unsigned)fp; 
  *(--stack) = sp[STACK_OFFSET_PC]; 
  *(--stack) = sp[STACK_OFFSET_MH]; 
  *(--stack) = sp[STACK_OFFSET_ML]; 
  *(--stack) = sp[STACK_OFFSET_R0]; 
  *(--stack) = sp[STACK_OFFSET_R1]; 
  *(--stack) = sp[STACK_OFFSET_R2]; 
  *(--stack) = sp[STACK_OFFSET_R3]; 
  *(--stack) = (unsigned)handler; // R4 = Arg1
  *(--stack) = id;                // R5 = Arg2
  *(--stack) = (unsigned)sender;  // R6 = Arg3
  *(--stack) = (unsigned)save;    // R7 = Arg4
  *(--stack) = sp[STACK_OFFSET_R8]; 
  *(--stack) = sp[STACK_OFFSET_R9]; 
  *(--stack) = sp[STACK_OFFSET_R10]; 
  *(--stack) = sp[STACK_OFFSET_R11]; 
  *(--stack) = sp[STACK_OFFSET_R12]; 
  *(--stack) = sp[STACK_OFFSET_R13]; 
  *(--stack) = sp[STACK_OFFSET_R14]; 
  *(--stack) = sp[STACK_OFFSET_FPSCR]; 
  *(--stack) = sp[STACK_OFFSET_FPUL]; 
  *(--stack) = sp[STACK_OFFSET_FR0]; 
  *(--stack) = sp[STACK_OFFSET_FR1]; 
  *(--stack) = sp[STACK_OFFSET_FR2]; 
  *(--stack) = sp[STACK_OFFSET_FR3]; 
  *(--stack) = sp[STACK_OFFSET_FR4]; 
  *(--stack) = sp[STACK_OFFSET_FR5]; 
  *(--stack) = sp[STACK_OFFSET_FR6]; 
  *(--stack) = sp[STACK_OFFSET_FR7]; 
  *(--stack) = sp[STACK_OFFSET_FR8]; 
  *(--stack) = sp[STACK_OFFSET_FR9]; 
  *(--stack) = sp[STACK_OFFSET_FR10]; 
  *(--stack) = sp[STACK_OFFSET_FR11]; 
  *(--stack) = sp[STACK_OFFSET_FR12]; 
  *(--stack) = sp[STACK_OFFSET_FR13]; 
  *(--stack) = sp[STACK_OFFSET_FR14]; 
  *(--stack) = sp[STACK_OFFSET_FR15]; 

  threadTable[threadIndex].sp = stack;

  _threadTable_unlock();

}
