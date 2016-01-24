#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <libgen.h>
#include <signal.h>
#include "bft.h"
#include "kernel.h"
#include "window.h"
#include "gfx.h"
#include "console.h"
#include "argv.h"
#include "shell.h"

extern void 
shell();

extern int 
shell_execBuiltin(int argc, char** argv);

extern int _shell_complete;

static int
execBuiltin(int argc, char** argv)
{
  int argc2;
  char** argv2;
  char* cmd = argv_reconstruct(argv);
  shell_globArgv(cmd, &argc2, &argv2);
  int retval = shell_execBuiltin(argc2, argv2);
  argv_free(argv2);
  free(cmd);
  return retval;
}


int 
main(int argc, char**argv)
{
  shell_init();
  
  if (argc > 0) {
    char *base = basename(argv[0]);
    if (strcmp(base, "bsh") != 0) {
      strcpy(argv[0], base);
      int retval = execBuiltin(argc, argv);
      return retval;
    }
  }

  unsigned w = (gfx_fontWidth+gfx_spaceWidth)*80, h = gfx_fontHeight*24;
  window_h window = window_create("bsh", 20, 20, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);

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
  rl_catch_signals = 0;
  rl_getc_function = shell_getc;

  setbuf(stdout, NULL);

  if (argc > 1) {
    shell_execBuiltinFromArgv(argc, argv, 1);
  }

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

  shell_cleanup();
  window_close(window);

  return 0;
}


