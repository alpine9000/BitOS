/* { dg-do run } */
/* { dg-options "-I../../../libbitmachine" } */

#include "thread.h"
#include "kernel.h"
#include "bft.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int 
delay()
{
  for (int i = 0; i < 100; i++) {
    kernel_threadBlocked();
  }
}

int
is_thread_running(thread_h tid)
{
  thread_status_t** stats = kernel_threadGetStats();
  int running = 0;

  for (int i = 0; stats[i] != 0; i++) {
    if (stats[i]->tid == tid && stats[i]->state != _THREAD_DEAD) {
      running = 1;
    }
  }

  kernel_threadFreeStats(stats);
  return running;
}

void kill_thread(thread_h tid, int signal)
{
  if (kill((int)tid, signal) != 0) {
    fprintf(stderr, "Failed to send %d\n", signal);
    abort();
  }

  delay();

  if (is_thread_running(tid)) {
    fprintf(stderr, "Failed to kill thread\n");
    abort();
  }
}

thread_h read_thread_handle(int fd)
{
  char buffer[255];
  int i;
  for (i = 0; i < 255; i++) {
    char c;
    if (read(fd, &c, 1) == 1) {
      if (c == '\n') {
	buffer[i] = 0;
	break;
      } else {
	buffer[i] = c;
      }
    } else {
      buffer[i] = 0;
      break;
    }
  }

  if (i == 255) {
    buffer[254] = 0;
  }

  int tid;
  if (sscanf(buffer, "%d", &tid) == 1) {
    return (thread_h)tid;
  } else {
    return INVALID_THREAD;
  }
}



void
simple_launch_and_kill()
{
  thread_h tid = thread_spawn("bsh");
  
  if (tid == INVALID_THREAD) {
    fprintf(stderr, "Failed to spawn thread\n");
    abort();
  }

  delay();

  kill_thread(tid, SIGINT);
}

void
catching()
{
  char buffer[1024];
  snprintf(buffer, 1024, "bsh catch %d", SIGINT);
  thread_h tid = thread_spawn(buffer);
  
  if (tid == INVALID_THREAD) {
    fprintf(stderr, "Failed to spawn thread\n");
    abort();
  }

  delay();

  if (kill((int)tid, SIGINT) != 0) {
    fprintf(stderr, "Failed to send SIGINT\n");
    abort();
  }

  delay();

  if (!is_thread_running(tid)) {
    fprintf(stderr, "Thread did not catch signal\n");
    abort();
  }

  delay();

  kill_thread(tid, SIGKILL);
}

void
killing_parent_doesnt_kill_child()
{
  FILE* fp = popen("/bin/sh -dontquit spawn bsh", "r");

  if (fp == NULL) {
    fprintf(stderr, "Failed to popen\n");
    abort();
  }

  int fd = fileno(fp);

  thread_h child = read_thread_handle(fd);
  if (child == INVALID_THREAD) {
    fprintf(stderr, "Failed to read child tid\n");
    abort();
  }

  thread_h parent = kernel_threadGetIdForStdout(fd);

  if (!is_thread_running(child)) {
    fprintf(stderr, "child thread %d is not running\n", child);
    abort();
  }

  if (!is_thread_running(parent)) {
    fprintf(stderr, "parent thread %d is not running\n", parent);
    abort();
  }

  kill_thread(parent, SIGINT);

  if (!is_thread_running(child)) {
    fprintf(stderr, "child thread %d is not running after killing parent\n",child);
    abort();
  }

  kill_thread(child, SIGINT);
}

void
killing_child_doesnt_kill_parent()
{
  FILE* fp = popen("/bin/sh -dontquit spawn bsh", "r");

  if (fp == NULL) {
    fprintf(stderr, "Failed to popen\n");
    abort();
  }

  int fd = fileno(fp);

  thread_h child = read_thread_handle(fd);
  if (child == INVALID_THREAD) {
    fprintf(stderr, "Failed to read child tid\n");
    abort();
  }

  thread_h parent = kernel_threadGetIdForStdout(fd);

  if (!is_thread_running(child)) {
    fprintf(stderr, "child thread %d is not running\n", child);
    abort();
  }

  if (!is_thread_running(parent)) {
    fprintf(stderr, "parent thread %d is not running\n", parent);
    abort();
  }

  kill_thread(child, SIGINT);
  
  if (!is_thread_running(parent)) {
    fprintf(stderr, "parent thread %d is not running after killing child\n", parent);
    abort();
  }

  kill_thread(parent, SIGINT);

}

int main(int argc, char** argv)
{
  simple_launch_and_kill();
  catching();
  killing_parent_doesnt_kill_child();
  
  return 0;
}
