#undef _KERNEL_BUILD
#include <glob.h>
#include <sys/dirent.h>
#include <sys/syslimits.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "bft.h"
#include "kernel.h"
#include "simulator.h"
#include "console.h"
#include "thread.h"
#include "argv.h"
#include "window.h"
#include "gfx.h"
#include "file.h"
#include "shell.h"

#define SUCCESS 0
#define FAIL -1

static window_h _stress_newWindow(char* title, int xOff, int yOff)
{
  unsigned w = shell_windowWidth, h = shell_windowHeight;
  setbuf(stdout, NULL);
  window_h window = window_create(title, (w+2)*xOff, (h+2)*yOff, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  return window;

}

static void
_stress_checkRam(char* ptr, unsigned len, int pid)
{
  for (unsigned i = 0; i < len; i++) {
    if (ptr[i] != pid) {
      printf("ramcheck failed at %x", i);
	kernel_threadDie(1);
    }
  }
}

static void
_stress_tickleRam(char* ptr, unsigned len, int pid)
{

  if (pid != getpid()) {
    printf("pid doesn't match!\n");
    kernel_threadDie(0);
  }
  
  memset(ptr, pid, len);
  
  kernel_threadBlocked();

  _stress_checkRam(ptr, len, pid);
}

static char* 
_stress_testMalloc(int divisor, int pid)
{
    int length = rand()/divisor;
    char* ptr = malloc(length);
    
    printf("malloc'd  %05x bytes -> %x\n", length, (unsigned)ptr);

    kernel_threadBlocked();

    _stress_tickleRam(ptr, length, pid);

    kernel_threadBlocked();

    int length2 = rand()/divisor;

    ptr = realloc(ptr, length2);

    printf("realloc'd %05x bytes -> %x\n", length2, (unsigned)ptr);

    if (length > length2) {
      _stress_checkRam(ptr, length2, pid);
    } else {
      _stress_checkRam(ptr, length, pid);
    }

    _stress_tickleRam(ptr, length2, pid);

    kernel_threadBlocked();

    return ptr;

}

static int 
_stress_malloc(int argc, char** argv)
{
  _stress_newWindow(argv[0], atoi(argv[1]),  atoi(argv[2]));  
  int pid = getpid();
  const int maxAlloc = 0x100000;
  int divisor = RAND_MAX/maxAlloc;
  const int num = 32;


  for(;;) {
    char* ptr[num];
    for (int i = 0; i < num; i++) {
       ptr[i] = _stress_testMalloc(divisor, pid);
    }
    for (int i = 0; i < num; i++) {
      free(ptr[i]);
      printf("free'd    %05x \n", (unsigned)ptr[i]);
    }
  }

  return SUCCESS;
}


static int 
_stress_rcopyToRand(char* src)
{

  struct stat buf;
  char dest[PATH_MAX];
  sprintf(dest, "/usr/local/home/%d", rand());

  if (stat(src, &buf) == 0) {
    if (S_ISDIR(buf.st_mode)) {
      DIR* dirp = opendir(src);
      struct dirent* dp;
      
      do {
	if ((dp = readdir(dirp)) != NULL) {
	  char sbuffer[PATH_MAX];
	  sprintf(sbuffer, "%s/%s", src, dp->d_name);
	  printf("rcopy %s\n", sbuffer);
	  if (_stress_rcopyToRand(sbuffer) == FAIL) {
	    fprintf(stderr, "_stress_rcopyToRand: failed to _stress_rcopyToRand\n");
	    return FAIL;
	  }
	}
      } while (dp != NULL);	
      if (closedir(dirp) == FAIL) {
	// closedir not supported on BITOS yet
	//fprintf(stderr, "_stress_rcopyToRand: failed to closedir\n");
	//	return FAIL;
      }
    } else {
      printf("copy from %s to %s\n", src, dest);
      if (shell_copy(src, dest) == FAIL) {
	    fprintf(stderr, "_stress_rcopyToRand: failed to shell_copy\n");
	return FAIL;
      }
      if (unlink(dest) == FAIL) {
	fprintf(stderr, "_stress_rcopyToRand: failed to unlink\n");
	return FAIL;
      }
    }
  }
  return SUCCESS;
}


static int 
_stress_filesystem(int argc, char** argv)
{
  _stress_newWindow(argv[0], atoi(argv[1]),  atoi(argv[2]));
  _stress_rcopyToRand("/");
  printf("Success!\n");
  //  kernel_threadDie(0);
  for (;;) {
    kernel_threadBlocked();
  }
  return SUCCESS;
}
				      
   

int
shell_stress(int argc, char** argv)
{
  char* cmds[] = {
    "malloc 0 0", 
    "malloc 1 0", 
    "filesystem 0 1", 
    "filesystem 1 1",
    0};
   
  for (unsigned i = 0; cmds[i] != 0; i++) {
    KERNEL_MODE();
    char** _argv = argv_build(cmds[i]);
    USER_MODE();
    
    if (strcmp(_argv[0], "malloc") == 0) {
	kernel_threadSpawn(&_stress_malloc, _argv, 0); 
    } else {
      kernel_threadSpawn(&_stress_filesystem, _argv, 0); 
    }
  }
  
  
  return SUCCESS;
}
