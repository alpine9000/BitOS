#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

static int aborted = 0;

static int quit (int a, int b)
{
  rl_done = 1;
  rl_line_buffer[0] = 0;
  aborted = 1;
  return 0;
}

static int nop (int a, int b)
{
  return 0;
}

int breadline_aborted()
{
  return aborted;
}

char* breadline(char* prompt)
{
  aborted = 0;

  char* line = readline(prompt);
  if (line) {
    add_history(line);
  }
  return line;
}

void breadline_init()
{
  using_history();
  stifle_history(20);
  read_history("~/bemacs_history");
  rl_bind_key(7, quit);
  rl_bind_key(12, nop);
}

void breadline_exit()
{
  write_history("~/bemacs_history");
}
