#include <stdlib.h>
#include <string.h>
#include "gfx.h"
#include "kernel.h"
#include "window.h"
#include "thread.h"
#include "argv.h"

#ifdef GITVERSION
static const char* version=GITVERSION;
#else
static const char* version="local";
#endif

int
go(int argc, char** argv)
{
  if (strcmp(argv[0], "kernel") == 0) {
    thread_spawn("/web/BitFS/bsh");
  } else {
    char* command = argv_reconstruct(argv);
    thread_spawn(command);
  }
  window_loop();
  return 0;
}

int
main()
{
  gfx_init();
  window_init();
  kernel_init(&go, version); 
  for(;;);
}

