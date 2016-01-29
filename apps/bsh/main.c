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
#include "commands.h"

extern int _shell_complete;


static char **
readline_completion(const char *text, int start, int end)
{
  char **matches = (char**)NULL;

  if (start == 0) {
    matches = rl_completion_matches(text, commands_generator);
  }

  return (matches);
}

int 
main(int argc, char**argv)
{ 
  char *base = basename(argv[0]);
  if (strcmp(base, "bsh") != 0) {
    strcpy(argv[0], base);
    return shell_execBuiltinFromArgv(argc, argv, 0);
  }

  shell_init();

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
  rl_attempted_completion_function = readline_completion;


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


