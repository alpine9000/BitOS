/* Signal TODO:
   o - (1) rwolf (2) bsh (3 - in new bsh) kill 2 (4) kill 3 -> bad argv free
*/

#include <signal.h>
#include "kernel.h"


int
signal_registerHandler(kernel_signal_handler_t* handler, int sig, kernel_signal_handler_t func)
{
  kernel_signal_handler_t old_func;
  thread_h tid = kernel_threadGetId();

  if (sig < 0 || sig >= kernel_signalMax) {
    return -1;
  }
  
  kernel_signal_handler_t* handlers = kernel_threadGetSignalHandler(tid);
  old_func = handlers[sig];

  handlers[sig] = func;

  *handler = old_func;

  return 0;
}

#define _signal_lock() unsigned ___ints_disabled = kernel_disableInts()
#define _signal_unlock() kernel_enableInts(___ints_disabled)

int 
signal_fire(thread_h tid, int sig)
{
  _signal_lock();
  
  kernel_signal_handler_t func;
  kernel_signal_handler_t* handlers = kernel_threadGetSignalHandler(tid);

  if (handlers == NULL || sig < 0 || sig >= kernel_signalMax) {
    _signal_unlock();
    return -1;
  }

  func = handlers[sig];

  if (func == SIG_DFL) {
    kernel_threadKill(tid);
    _signal_unlock();
    return 0;
  } else if (func == SIG_IGN) {
    _signal_unlock();
    return 0;
  } else {
    if (tid == kernel_threadGetId()) {
      _signal_unlock();
      func(sig);
    } else {
      kernel_threadQueueSignalHandler(tid, func, sig);
      _signal_unlock();
    }
    return 0;
  }
}
