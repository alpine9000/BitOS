#include <stdlib.h>
#include "gfx.h"
#include "kernel.h"
#include "window.h"
#include "argv.h"

extern char ** gitversion;
static char* version=GITVERSION;

extern void 
memory_cleanupThread(unsigned);

extern void 
shell();

void 
runShell()
{
  unsigned w = 320, h = 200;
  window_h window = window_create("Shell", 0, 0, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  shell(); 
  window_close(window);
  kernel_threadDie(0);
}

void 
go()
{
  kernel_threadSpawn(&runShell, argv_build("shell"), 0); 
  window_loop();
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

