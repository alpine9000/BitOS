#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "thread.h"
#include "kernel.h"
#include "bft.h"

/* { dg-do run } */
/* { dg-options "-I../../../libbitmachine" } */


void
createFile(char* filename)
{
  
  int fd = open(filename, O_WRONLY|O_CREAT);
  
  if (fd == 0) {
    fprintf(stderr, "Failed to create %s\n", filename);
    kernel_threadBlocked();
    abort();
  }

  if (close(fd) != 0) {
    fprintf(stderr, "Failed to create (close) %s\n", filename);
      kernel_threadBlocked();
    abort();
  }

  struct stat buf;
  if (stat(filename, &buf) != 0) {
    fprintf(stderr, "Failed to create (stat) %s\n", filename);
  kernel_threadBlocked();
    abort();
  }    
}

void 
fileDoesNotExist(char* filename)
{
  struct stat buf;
  if (stat(filename, &buf) == 0) {
    fprintf(stderr, "Failed remove file  %s\n", filename);
    kernel_threadBlocked();
    abort();
  }    
}


int 
main(int argc, char** argv)
{

  createFile("__test_1");
  createFile("__test_2");
  createFile("__test_3");

  if (!thread_load("/bin/sh -c rm __test_*")) {
    fprintf(stderr, "Failed to load sh\n");
    kernel_threadBlocked();
    abort();
  }

  fileDoesNotExist("__test_1");
  fileDoesNotExist("__test_2");
  fileDoesNotExist("__test_3");

  kernel_threadBlocked();

}
