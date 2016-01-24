#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include "bft.h"
#include "window.h"
#include "gfx.h"
#include "kernel.h"
#include "thread.h"
#include "argv.h"
#include "file.h"

#include "shell.h" 
#include "runtest.h"

static int
mv(int argc, char** argv);

static int
cd(int argc, char** argv);

static int
cp(int argc, char** argv);

static int
pwd(int argc, char** argv);

static int
ls(int argc, char** argv);

static int 
diff(int argc, char** argv);

static int
rm(int argc, char** argv);

static int
cat(int argc, char** argv);

static int
touch(int argc, char** argv);

static int
shell_mkdir(int argc, char** argv);

static int
sh(int argc, char** argv);

static int
version(int argc, char** argv);

static int
rwolf(int argc, char** argv);

static int
kernel(int argc, char** argv);

static int
shell_kernel_stats(int argc, char** argv);

static int
shell_malloc_stats(int argc, char** argv);

static int 
shell_time(int argc, char** argv);

static int
shell_kill(int argc, char** argv);

static int
shell_spawn(int argc, char** argv);

static int
shell_echo(int argc, char** argv);

typedef void(*arg_function)(int,char**);


typedef struct {
  char *name;
  unsigned spawn;
  int(*function)(int, char**);
} builtin_t;

static builtin_t builtins[] = {
  {"ps", 0, shell_kernel_stats},
  {"free", 0, shell_malloc_stats},
  {"cat", 0, cat},
  {"cp", 0, cp},
  {"mv", 0, mv},
  {"ls", 0, ls},
  {"rm", 0, rm},
  {"pwd", 0, pwd},
  {"cd", 0, cd},
  {"sh", 0, sh},
  {"mkdir", 0, shell_mkdir},
  {"rwolf", 1, rwolf},
  {"stress", 0, shell_stress},
  {"diff", 0, diff},
  {"rt", 0,runtest},
  {"test", 0, shell_test},
  {"version", 0, version},
  {"kernel", 0, kernel},
  {"touch", 0, touch},
  {"time", 0,shell_time},
  {"kill", 0, shell_kill},
  {"spawn", 0, shell_spawn},
  {"echo", 0, shell_echo}
};

static unsigned numBuiltins = sizeof(builtins)/sizeof(builtin_t);

static int
version(int argc, char** argv)
{
  printf("BitOS %s\n", _bft->kernel_version());
  return 0;
}

static int
rwolf(int argc, char** argv)
{
  return _bft->wolf(argc, argv);
}


static int
mv(int argc, char** argv)
{
  if (argc != 3) {
    printf("usage: %ssrc dest\n", argv[0]);
    return 1;
  }

  return rename(argv[1], argv[2]);
}


static int
cp(int argc, char** argv)
{
  if (!(argc == 3 || argc == 4)) {
    printf("usage: %s [-r] src dest\n", argv[0]);
    return 1;
  }

  int argvIndex = 1;
  int recursive = 0;
  if (argc == 4) {
    if (strcmp(argv[argvIndex++], "-r") == 0) {
      recursive = 1;
    }
  }

  char* s = argv[argvIndex++];
  char* d = argv[argvIndex++];
  if (!recursive) {
    return shell_copy(s, d);
  } else {
    return shell_rcopy(s, d);
  }
}


static int
shell_mkdir(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s path\n", argv[0]);
    return 1;
  }

  return mkdir(argv[1], 0);
}


static int
runTestBuild(int argc, char** argv)
{
  unsigned w = shell_windowWidth, h = shell_windowHeight;
  setbuf(stdout, NULL);
  int offset = atoi(argv[1]);
  window_h window = window_create("BitOS", (w+2)*offset, 0, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  int exitStatus = 0;
  int pathLen = strlen(argv[0]);
  int numLoops = atoi(argv[2]);

  char* cleanCommand = "make clean -C ";
  char* cleanCommandPtr = malloc(strlen(cleanCommand) + pathLen+1);
  sprintf(cleanCommandPtr, "%s%s", cleanCommand, argv[0]);

  char* elfCommand = "make elf -C ";
  char* elfCommandPtr = malloc(strlen(elfCommand) + pathLen+1);
  sprintf(elfCommandPtr, "%s%s", elfCommand, argv[0]);

  for (int i = 0; i < numLoops; i++) {
    if (!thread_load(cleanCommandPtr) ||
	!thread_load(elfCommandPtr)) {
      exitStatus = 1;
      break;
    }
  }

  if (exitStatus == 0) {
    printf("Success!\n");
  } else {
    printf(":-(\n");
  }

  kernel_threadDie(exitStatus);

  return 0;
}


int
shell_test(int argc, char** argv)
{
   unsigned w = shell_windowWidth, h = shell_windowHeight;

  setbuf(stdout, NULL);

  window_h window = window_create("Torture", 0, h+window_titleBarHeight+2, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);

  shell_rcopy("/usr/local/src/BitOS", "/usr/local/src/BitOS.2");

  char cmd1[1024];
  char cmd2[1024];


  snprintf(cmd1, 1024, "/usr/local/src/BitOS 0 %s", argv[1]);
  snprintf(cmd2, 1024, "/usr/local/src/BitOS.2 1 %s", argv[1]);

  char** argv1 = argv_build(cmd1);
  char** argv2 = argv_build(cmd2);

  thread_h t1 = kernel_threadSpawn(&runTestBuild, argv1, 0); 
  thread_h t2 = kernel_threadSpawn(&runTestBuild, argv2, 0);

  int result1 = -1;
  int result2 = -1;
  
  do {
    if (result1 == -1) {
      result1 = kernel_threadGetExitStatus(t1);
    }

    if (result2 == -1) {
      result2 = kernel_threadGetExitStatus(t2);
    }

    kernel_threadBlocked();
  } while(result1 == -1 || result2 == -1);


  printf("Results = %s\n", result1 == 0 && result2 == 0 ? "Success" : "Failed");
  
  kernel_threadDie(0);
  
  return 0;
}


static int
pwd(int argc, char** argv)
{
  char buffer[PATH_MAX];
  printf("%s\n", getcwd(buffer, PATH_MAX));
  return 0;
}


static int 
cd(int argc, char** argv)
{
  if (argc == 1) {
    return chdir("~");
  }

  if (argc != 2) {
    printf("usage: %s path\n", argv[0]);
    return 1;
  }

  return chdir(argv[1]);
}


static int 
shell_time(int argc, char** argv)
{
  if (argc == 1) {
    printf("usage: %s command\n", argv[0]);
    return -1;
  }  

  char** newArgv = shell_argvDup(argc, argv, 1);

  char* command = argv_reconstruct(newArgv);

  struct timeval start, end, diff;
  
  gettimeofday(&start,0);

  int success = thread_load(command);
  
  gettimeofday(&end,0);

  free(command);
  argv_free(newArgv);
  
  shell_timeval_subtract(&diff, &end, &start);
  
  printf("completed [%s] in %ld.%03ld\n", success ? "success" : "failed", diff.tv_sec, (diff.tv_usec/1000));

  return success;
}


static int 
diff(int argc, char** argv)
{
  if (argc != 3) {
    fprintf(stderr, "usage: %s file1 file2\n", argv[0]);
    return 1;
  }

  if (!shell_filesAreIdentical(argv[1], argv[2])) {
    printf("Files %s and %s differ\n", argv[1], argv[2]);
  }
  
  return 0;
}


static int
shell_echo(int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    printf("%s ", argv[i]);
  }

  if (argc > 1) {
    printf("\n");
  }

  return 0;
}


static int
shell_spawn(int argc, char** argv)
{
  if (argc == 1) {
    fprintf(stderr, "usage: %s command\n", argv[0]);
    return 0;
  }

  char** argv2 = shell_argvDup(argc, argv, 1);
  char* command = argv_reconstruct(argv2);
  thread_h tid = thread_spawn(command);
  free(command);
  argv_free(argv2);

  if (tid == INVALID_THREAD) {
    return -1;
  }

  setbuf(stdout, NULL);  
  printf("%d\n", (unsigned)tid);

  return 0;
}


static int
shell_kill(int argc, char** argv)
{
  if (argc == 2) {
    unsigned pid = atoi(argv[1]);
    printf("Sending %d to %d\n", SIGINT, pid);
    if (kill(pid, SIGINT) != 0) {
      printf("%s: failed\n", argv[0]);
    }

  } else if (argc == 3) {
    unsigned pid = atoi(argv[1]);
    int signo = atoi(argv[2]);
    printf("Sending %d to %d\n", signo, pid);
    if (kill(pid, signo) != 0) {
      printf("%s: failed\n", argv[0]);
    }
  } else {
    fprintf(stderr, "usage: %s tid [signal number]\n", argv[0]);
  }

  return 0;
}


static int
ls(int argc, char** argv)
{
  char cwd[PATH_MAX];
  char path[PATH_MAX];
  
  int file = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      file = 1;
      shell_listPath(argv[i], cwd, argc, argv);
    }
  }

  if (!file) {
    getcwd(path, PATH_MAX);
    getcwd(cwd, PATH_MAX);
    shell_listPath(path, cwd, argc, argv);
  }
  
  return 0;
}


static int 
rm(int argc, char** argv)
{
  if (argc == 1) {
    printf("usage: %s [path...]\n", argv[0]);
    return 1;
  }

  int force = 0;
  
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (unlink(argv[i]) != 0) {
	if (!force) {
	  printf("%s: failed to rm %s\n", argv[0], argv[i]);
	  return 1;
	}
      }
    } else {
      if (argv[i][1] == 'f') {
	force = 1;
      }
    }
  }  
  return 0;
} 


static int 
cat(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s file\n", argv[0]);
    return 1;
  }
  
  FILE *fp = fopen(argv[1], "r");
  
  if (fp == 0) {
    printf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
    return 1;
  }
  
  char buffer[255];
  for(;;) {
    int len = fread(&buffer, 1, sizeof(buffer)-1, fp);
    if (len > 0) {
      buffer[len] = 0;
      printf(buffer);
    } else {
      break;
    }
  } 

  fclose(fp);

  return 0;
}


int
touch(int argc, char** argv)
{
  if (argc == 1) {
    printf("usage: %s [path...]\n", argv[0]);
    return 1;
  }

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      int fd = open(argv[i], O_RDWR|O_APPEND);     
      if (fd < 0) {
	fd = open(argv[i], O_WRONLY|O_CREAT);
	if (fd < 0) {
	  printf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
	  return 1;
	}
      }
  
      close(fd);
    }
  }

  return 0; 
}


static int 
kernel(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s file\n", argv[0]);
    return 1;
  }
  
  FILE *fp = fopen(argv[1], "r");
  
  if (fp == 0) {
    printf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
    return 1;
  }
  
  file_loadElfKernel(fileno(fp));
  return 0;
}


static int
shell_kernel_stats(int argc, char** argv)
{  
  char buffer[PATH_MAX];
  thread_status_t** stats = kernel_threadGetStats();

  char* lookup[] = {
    "DEAD",
    "RUNNING",
    "WAIT",
    "BLOCKED"
  };

  int tidColumnWidth = 3;
  int stateColumnWidth = 5;
  
  for (int i = 0; stats[i] != 0; i++) {
    snprintf(buffer, PATH_MAX, "%d", (unsigned) stats[i]->tid);
    int len = strlen(buffer);
    if (len > tidColumnWidth) {
      tidColumnWidth = len;
    }
    len = strlen(lookup[stats[i]->state]);
    if (len > stateColumnWidth) {
      stateColumnWidth = len;
    }
  }
  

  sprintf(buffer, "%%%ds %%-%ds %%s\n", tidColumnWidth, stateColumnWidth);
     
  printf(buffer, "TID", "STATE", "CMD");

  sprintf(buffer, "%%%dd %%-%ds %%s\n", tidColumnWidth, stateColumnWidth);

  for (int i = 0; stats[i] != 0; i++) {
    printf(buffer, (unsigned)stats[i]->tid, lookup[stats[i]->state], stats[i]->name);
  }


  kernel_threadFreeStats(stats);
  return 0;

}


static int 
shell_malloc_stats(int argc, char** argv)
{
  malloc_stats();
  return 0;
}


static int
sh(int argc, char** argv)
{
  if (!(argc > 2)) { // /bin/sh -c rm blah blah2
    printf("%s: usage: %s -C command\n", argv[0], argv[0]);
    return 1;
  }
  
  int dontQuit = 0;
  if (strcmp(argv[1], "-dontquit") == 0) {
    dontQuit = 1;
  }

  shell_execBuiltinFromArgv(argc, argv, 2); // sh -C

  if (dontQuit) {
    for (;;) {
      kernel_threadBlocked();
    }
  }

  return 0;
}


int 
shell_execBuiltin(int argc, char** argv)
{
  int retval = -2;
  if (argc > 0) {
    for (unsigned i = 0; i < numBuiltins; i++) {
      if (strcmp(argv[0], builtins[i].name) == 0) {
	if (!builtins[i].spawn) {
	  retval = builtins[i].function(argv_argc(argv), argv);
	}
	return retval;
      } 
    }
  }

  return retval;
}

int 
shell_launchBuiltin(int argc, char** argv)
{
  if (argc > 0) {
    for (unsigned i = 0; i < numBuiltins; i++) {
      if (strcmp(argv[0], builtins[i].name) == 0) {
	if (builtins[i].spawn) {
	  kernel_threadSpawn(builtins[i].function, argv, 0);
	  return 0;
	} else {
	  builtins[i].function(argv_argc(argv), argv);
	}
	return 0;
      } 
    }
  }

  return -1;
}
