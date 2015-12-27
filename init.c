#include <stdlib.h>
#include "gfx.h"
#include "kernel.h"
#include "window.h"
#include "argv.h"
#include "process.h"

extern char ** gitversion;
#ifdef GITVERSION
static char* version=GITVERSION;
#else
static char* version="local";
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
  if (thread_spawn("bsh") == INVALID_THREAD) {
    kernel_threadSpawn(&runShell, argv_build("bsh"), 0); 
  }
  window_loop();
  return 0;
}

int
main()
{
  gitversion = &version;
  gfx_init();
  window_init();
  kernel_init(&go); 
  for(;;);
}

