#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

static int aborted = 0;

char * (*breadline_completion_generator) (const char *text, int state) = 0;

char *command_generator PARAMS((const char *, int));
static char **readline_completion PARAMS((const char *, int, int));

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

char* breadline(char* prompt, char * (*generator) (const char *text, int state))
{
  aborted = 0;
  breadline_completion_generator = generator;
  char* line = readline(prompt);
  if (line) {
    add_history(line);
  }
  return line;
}

void breadline_init()
{
  rl_attempted_completion_function = readline_completion;
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

static char **
readline_completion (text, start, end)
     const char *text;
     int start, end;
{
  char **matches;

  rl_completion_suppress_append = 1;

  matches = (char **)NULL;

  if (breadline_completion_generator != 0) {
    matches = rl_completion_matches (text, breadline_completion_generator);
  }

  return (matches);
}
