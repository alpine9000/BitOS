#pragma once

// These types are here becaue things like window.h include bft.h and bft.h needs these types.

typedef void* window_h;

typedef enum {
  KERNEL_THREAD_WINDOW,
  KERNEL_CURRENT_WORKING_DIRECTORY
} thread_info_t;

typedef enum {
  AUDIO_TYPE_SINE = 0,
  AUDIO_TYPE_SAWTOOTH = 1,
  AUDIO_TYPE_SQUARE = 2,
  AUDIO_TYPE_TRIANGLE = 3,
  AUDIO_TYPE_BUFFER = 4,
} audio_type_t;

typedef struct {
  int _stdin;
  int _stdout;
  int _stderr;
} fds_t;


typedef enum { WAITING_FOR_CONTROL = 0, WAITING_FOR_TYPE = 1, WAITING_FOR_ARG = 2} console_console_state_t;

typedef enum {
  CONSOLE_SET_ROW = 100, 
  CONSOLE_SET_COL, 
  CONSOLE_STANDOUT, 
  CONSOLE_CLEAR_TO_END_OF_LINE, 
  CONSOLE_RESET,
  CONSOLE_INSERT_AT_CURSOR,
  CONSOLE_DELETE_AT_CURSOR,
  CONSOLE_UP,
  CONSOLE_BACKSPACE,

} console_control_type_t;

typedef enum {
  CONSOLE_BEHAVIOUR_AUTO_WRAP = 1,
  CONSOLE_BEHAVIOUR_AUTO_SCROLL = 2,
  CONSOLE_BEHAVIOUR_STANDOUT = 4
} console_behaviour_t;

typedef struct {
  console_console_state_t state;
  console_control_type_t type;
  unsigned arg;
  unsigned behaviour;
} console_control_t;



#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */

