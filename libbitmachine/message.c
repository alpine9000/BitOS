#include <signal.h>
#include "kernel.h"

int
message_getHandler(message_handler_t* handler, int id)
{
  thread_h tid = kernel_threadGetId();
  message_handler_t* handlers = kernel_threadGetMessageHandler(tid);

  if (id < 0 || id >= kernel_messageMax) {
    return -1;
  }
  *handler = handlers[id];
  return 0;
}

int
message_handle(int id, message_handler_t func)
{
  thread_h tid = kernel_threadGetId();

  if (id < 0 || id >= kernel_messageMax) {
    return -1;
  }
  
  message_handler_t* handlers = kernel_threadGetMessageHandler(tid);

  if (id == 0) {
    for (int i = 0; i < kernel_messageMax; i++) {
      handlers[i] = func;
    }
  } else {
    handlers[id] = func;
  }

  return 0;
}

#define _message_lock() unsigned ___ints_disabled = kernel_disableInts()
#define _message_unlock() kernel_enableInts(___ints_disabled)

int 
message_send(thread_h tid, int id, void* data)
{
  _message_lock();
  
  message_handler_t func;
  message_handler_t* handlers = kernel_threadGetMessageHandler(tid);

  if (handlers == NULL || id < 0 || id >= kernel_messageMax) {
    _message_unlock();
    return -1;
  }

  func = handlers[id];
  thread_h currentThreadId = kernel_threadGetId();

  if (func == (message_handler_t)SIG_DFL) {
    kernel_threadKill(tid);
    _message_unlock();
    return 0;
  } else if (func == (message_handler_t)SIG_IGN) {
    _message_unlock();
    return 0;
  } else {
    if (tid == currentThreadId) {
      _message_unlock();
      func(id, currentThreadId, data);
    } else {
      kernel_threadQueueMessageHandler(tid, func, id, currentThreadId, data);
      _message_unlock();
    }
    return 0;
  }
}
