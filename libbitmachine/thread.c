#include <stdio.h>
#include <sys/types.h>
#include <sys/syslimits.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "syscall.h"
#include "window.h"
#include "gfx.h"
#include "console.h"
#include "kernel.h"
#include "simulator.h"
#include "peripheral.h"
#include "argv.h"
#include "memory.h"
#include "file.h"

#define BIN "0B-PSYTJK8InmOUtrX3p2eG9zUU0"

typedef struct {
  int(*entry)(int, char**);
  char** argv;
  unsigned *image;
  unsigned imageSize;
} _thread_info_t;

static int 
_thread_load(char* command, _thread_info_t* info)
{
  char buffer[PATH_MAX];
  KERNEL_MODE();
  char** argv = argv_build((char*)command);
  USER_MODE();
  char* name = argv[0];
  int fd;
  struct stat s;

  simulator_printf("_thread_load: %s\n", command);

  if (name[0] != '/') {
    snprintf(buffer, PATH_MAX, "/usr/local/bin/%s", name);
  } else {
    strlcpy(buffer, name, PATH_MAX);
  }

  fd = open(buffer, O_RDONLY);

  if (fstat(fd, &s) < 0) {
    buffer[0] = 0;
    snprintf(buffer, PATH_MAX, "/usr/local/sh-elf/bin/%s", name);
    fd = open(buffer, O_RDONLY);
  }

  if (fstat(fd, &s) < 0) {
    buffer[0] = 0;
    snprintf(buffer, PATH_MAX, "/dev/id/%s/%s", BIN, name);
    fd = open(buffer, O_RDONLY);
  }

  if (fstat(fd, &s) < 0) {
    printf("fstat failed (%s)\n", command);
    return 0;
  }

  info->entry = file_loadElf(fd, &info->image, &info->imageSize);

  close(fd);

  //  printf("address = %0X\n", (unsigned)info->image);
  //  printf("elfSize = %d\n", info->imageSize);
  //  printf("Entry point = %0X\n", (unsigned)info->entry);

  info->argv = argv;

  return 1;
}


int
thread_open(char* command)
{
  _thread_info_t info;

  if (_thread_load(command, &info)) {

    KERNEL_MODE();
    int fd = open("/dev/pipe", O_RDWR);  
    USER_MODE();

    fds_t fds = {STDIN_FILENO, fd, STDERR_FILENO};
    
    kernel_threadLoad(info.image, info.imageSize, info.entry, info.argv, &fds, 1);
    
    return fd;
  } 
  
  return -1;
}


int 
thread_close(int fd)
{
  thread_h tid = kernel_threadGetIdForStdout(fd);
  int status = kernel_threadGetExitStatus(tid);

  close(fd);

  return status;
}



thread_h 
thread_spawn(char* command)
{
  _thread_info_t info;
  if (_thread_load(command, &info)) {
    return kernel_threadLoad(info.image, info.imageSize, info.entry, info.argv, 0, 1);
  }
  return INVALID_THREAD;
}

thread_h 
thread_spawnFileDescriptors(char* command, int in, int out, int err)
{
  _thread_info_t info;
  if (_thread_load(command, &info)) {
    fds_t fds = {in, out, err};
    return kernel_threadLoad(info.image, info.imageSize, info.entry, info.argv, &fds, 1);
  }
  return INVALID_THREAD;
}

int thread_run(char* commandLine)
{ 
  int fd = thread_open(commandLine);

  if (fd > -1) {
    for(;;) {
      char c;
      if (read(fd, &c, 1) <= 0) {
	kernel_threadBlocked();
	break;
      }
      printf("%c", c);
      fflush(stdout);
    }
    
    return thread_close(fd);
  }
  
  return -1;
}


