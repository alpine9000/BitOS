
#include <sys/unistd.h>
#include "console.h"
#include "error.h"
#include "peripheral.h"
#include "simulator.h"
#include "file.h"
#include "panic.h"

static int __fd = 5;

int _file_open (char *name, int flags)
{
  int fd = ++__fd;
  peripheral.file.fd = fd;
  peripheral.file.filename = (unsigned long)name;
  unsigned long status;

  for (;;) {
    status = peripheral.file.status;
    if (status == 0) {
      simulator_yield();
    } else {
      break;
    }
  }

  if (status == 1) {
    return fd;
  } else {
    return -1;
  }
}

int _file_read_stdin (int file, char *ptr, int len)
{
    int i;
  
    while (!_console_char_avail());
    
    for (i = 0; i < len; ++i) {
      if (_console_char_avail()) {
	int val = _console_read_char();
	*ptr++ = (char)val;
      } else {
	break;
      }
    }

    return i;
}

int _file_read (int file, char *ptr, int len)
{
  if (file == STDIN_FILENO) {
    return _file_read_stdin(file, ptr, len);
  } else {
    peripheral.file.fd = file;
    int i;
    for (i = 0; i < len; ++i) {
      int val = peripheral.file.read;
      if (val >= 0) {
	*ptr++ = (char)val;
      } else {
	break;
      }
    }

    return i;
  }
}

int _file_write_stdout ( int file, char *ptr, int len)
{
  int i = 0;
  for (i = 0; i < len; i++) {
    _console_write_char(*ptr);
    ptr++;
  }
  return len;
}

int _file_write ( int file, char *ptr, int len)
{

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    return _file_write_stdout(file, ptr, len);
  } else {
    panic();
    return -1;
  }
}

int _file_close(int file)
{
  peripheral.file.fd = file;
  peripheral.file.close = 1;
  return 0;
}

int _file_isatty(int file)
{
  if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
    return 1;
  } else {
    return 0;
  }
}


int _file_fstat (int file, struct stat *st)
{
  if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
    st->st_mode = S_IFCHR;
    return 0;
  } else {
    return -1;
  }
}

int _file_unlink(char* file)
{
  return -1;
}


int _file_creat (const char *path,int mode) {
  return -1;
}


int _file_lseek (int file, int ptr, int dir) {
  return -1;
}


int _file_stat (int file, struct stat *st) {
  return -1;
}
