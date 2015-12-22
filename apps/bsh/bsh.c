#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "bft.h"
#include "kernel.h"
#include "window.h"
#include "gfx.h"
#include "console.h"

extern void shell();
extern int shell_exec(char*);
extern int _shell_complete;

int main(int argc, char**argv)
{
#define OPEN_WINDOW
 
#ifdef OPEN_WINDOW
  unsigned w = (gfx_fontWidth+gfx_spaceWidth)*80, h = gfx_fontHeight*24;
  window_h window = window_create("bsh", 20, 20, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
#endif

  fds_t* fds = kernel_threadGetFds();

  if (fds->_stdout != STDOUT_FILENO) {
    close(fds->_stdout);
    fds->_stdout = STDOUT_FILENO;
  }
  
  using_history();
  stifle_history(20);
  read_history("~/bsh_history");

  chdir("~/");
  
  rl_terminal_name = "vt100";


  setbuf(stdout, NULL);

  char* line = 0;
  while (!_shell_complete) {
    if (line != 0) {
      free(line);
    }
    console_clearBehaviour(CONSOLE_BEHAVIOUR_AUTO_WRAP);
    line = readline(">");
    console_setBehaviour(CONSOLE_BEHAVIOUR_AUTO_WRAP);
    if (line[0]) {
      add_history(line);
      if (strcmp(line, "exit") == 0) {
	break;
      }
      shell_exec(line);
      write_history("~/bsh_history");
    }
    kernel_threadBlocked();
  }

#ifdef OPEN_WINDOW
  window_close(window);
#endif

  return 0;
}


