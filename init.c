#include <stdlib.h>
#include <string.h>
#include "gfx.h"
#include "kernel.h"
#include "window.h"
#include "argv.h"
#include "thread.h"

#ifdef GITVERSION
static const char* version=GITVERSION;
#else
static const char* version="local";
#endif

extern void 
memory_cleanupThread(unsigned);

extern void 
shell();

int 
runShell(int argc, char** argv)
{
  unsigned w = 320, h = 200;
  window_h window = window_create("Shell", 0, 0, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  shell(); 
  window_close(window);
  kernel_threadDie(0);
  return 0;
}

int
go(int argc, char** argv)
{
  extern int shell_test(int argc, char** argv);
  if (argc == 2 && strncmp(argv[1], "test", 5) == 0) {
    kernel_threadSpawn(&shell_test, argv_build("test"), 0);
  } else {
    if (thread_spawn("bsh") == INVALID_THREAD) {
      kernel_threadSpawn(&runShell, argv_build("bsh"), 0); 
    }
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

