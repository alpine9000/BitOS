#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include "kernel.h"
#include "bft.h"

int quit = 0;

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
}

int main(void)
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
    printf("\ncan't catch SIGINT\n");

  printf("raise(SIGINT);\n");

  kill(getpid(), SIGINT);

  volatile int i = 0;
  while (quit == 0) {
    kernel_threadBlocked();
    if ((i++ % 100) == 0) {
      printf("hello %d\n", i);
    }
  }

  printf("Exiting...\n");

  return 0;
}
