#include "bft.h"
#include "kernel.h"
#include "peripheral.h"

extern void start(int argc, char**argv) 
__asm__("start");
extern int main(int argc, char**argv);
extern void exit(int);

static void _init_ctors()

{
  static int initialized = 0;
  if (!initialized) {
      typedef void (*pfunc) ();
      extern pfunc __ctors[];
      extern pfunc __ctors_end[];
      volatile pfunc *p;
      
      initialized = 1;
      for (p = __ctors_end; p > __ctors; ) {
        (*--p) ();
      }      
  }
}

void free(void*);

void start(int argc, char** argv)
{
  bft_init();
  _init_ctors();
  exit(main(argc, argv));
}

void _exit(int status) 
{
  // newlib does seem to close stdout if it hasn't been used, so we do this in the case stdout is an empty pipe that will have a blocked listener waiting on it
  extern int close(int);
  close(STDOUT_FILENO);
  kernel_die(status);
  for(;;);
}


void *__dso_handle = 0; 
