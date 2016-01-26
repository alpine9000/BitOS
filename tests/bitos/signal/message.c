/* { dg-do run } */
/* { dg-options "-I../../../libbitmachine" } */

#include "thread.h"
#include "kernel.h"
#include "message.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int numDelayLoops = 100;
static void
delay()
{
  for (int i = 0; i < numDelayLoops; i++) {
    kernel_threadBlocked();
  }
}

int sentData = 0xCAFEBABE;
int gotMessage = 0;
int rxedData = -1;

static void 
message_handler(int message, thread_h sender, void* data)
{
  gotMessage = 1;
  rxedData = (int)data;
}

int
main(int argc, char** argv)
{
  thread_h tid = thread_spawn("bsh mecho");
  
  if (tid == INVALID_THREAD) {
    fprintf(stderr, "Failed to spawn thread\n");
    abort();
  }

  delay();

  message_handle(35, message_handler);

  message_send(tid, 35, (void*)sentData);

  for (int i = 0; i < 1000; i++) {
    if (gotMessage) {
      break;
    }
    kernel_threadBlocked();
  }

  if (!gotMessage) {
    fprintf(stderr, "Failed to get echo'd message\n");
    kill((int)tid, SIGKILL);
    abort();
  }

  if (rxedData != sentData) {
    fprintf(stderr, "Failed to get data\n");
    kill((int)tid, SIGKILL);
    abort();
  }

  kill((int)tid, SIGKILL);

  return 0;

}
