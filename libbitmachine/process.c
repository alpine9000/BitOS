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
  void(*entry)(int, char**);
  char** argv;
  unsigned *image;
  unsigned imageSize;
} _process_info_t;

static int _process_load(char* command, _process_info_t* info)
{
  char buffer[PATH_MAX];
  char** argv = argv_build((char*)command);
  char* name = argv[0];
  int fd;
  struct stat s;

  if (name[0] != '/') {
    snprintf(buffer, PATH_MAX, "/usr/local/bin/%s", name);
  } else {
    strncpy(buffer, name, PATH_MAX);
  }

  fd = open(buffer, O_RDONLY);

  if (fstat(fd, &s) < 0) {
    buffer[0] = 0;
    snprintf(buffer, PATH_MAX, "/dev/id/%s/%s", BIN, name);
    fd = open(buffer, O_RDONLY);
    if (fstat(fd, &s) < 0) {
      printf("fstat failed\n");
      return 0;
    }
  }

  info->entry = file_loadElf(fd, &info->image, &info->imageSize);

  close(fd);

  //  printf("address = %0X\n", (unsigned)info->image);
  //  printf("elfSize = %d\n", info->imageSize);
  //  printf("Entry point = %0X\n", (unsigned)info->entry);

  info->argv = argv;

  return 1;
}

FILE* process_open(char* command)
{
  _process_info_t info;
  if (_process_load(command, &info)) {
    FILE* fp = fopen("/dev/pipe", "rw");  
    fds_t fds = {STDIN_FILENO, fileno(fp), STDERR_FILENO};
    
    kernel_load(info.image, info.imageSize, info.entry, info.argv, &fds, 1);
    
    return fp;
  } 

  return 0;
}

int process_close(FILE* stream)
{
  unsigned fd = fileno(stream);
  int pid = kernel_getPidForStdout(fd);
  int status = kernel_getExitStatus(pid);

  fclose(stream);

  return status;
}

int process_spawn(char* command)
{
  _process_info_t info;
  if (_process_load(command, &info)) {
    return kernel_load(info.image, info.imageSize, info.entry, info.argv, 0, 1);
  }
  return -1;
}


int process_load(char* commandLine)
{ 
  FILE *fp = popen(commandLine, "r");

  for(;fp != 0;) {
    char c;
    if (read(fileno(fp), &c, 1) <= 0) {
      thread_blocked();
      break;
    }
    printf("%c", c);
    fflush(stdout);
  }

  if (fp) {
    return pclose(fp) == 0;
  }
  
  return 0;
}

int process_wait(unsigned pid)
{
  while (kernel_getIsThreadAlive(pid)) {
    thread_blocked();
  }

  return kernel_getExitStatus(pid);
}

