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
#include "message.h"

#include "shell.h" 
#include "runtest.h"
#include "commands.h"

static int
_mv(int argc, char** argv);

static int
_cd(int argc, char** argv);

static int
_cp(int argc, char** argv);

static int
_pwd(int argc, char** argv);

static int
_ls(int argc, char** argv);

static int 
_diff(int argc, char** argv);

static int
_rm(int argc, char** argv);

static int
_cat(int argc, char** argv);

static int
_touch(int argc, char** argv);

static int
_mkdir(int argc, char** argv);

static int
_sh(int argc, char** argv);

static int
_version(int argc, char** argv);

static int
_rwolf(int argc, char** argv);

static int
_kernel(int argc, char** argv);

static int
_kernel_stats(int argc, char** argv);

static int
_malloc_stats(int argc, char** argv);

static int 
_time(int argc, char** argv);

static int
_kill(int argc, char** argv);

static int
_spawn(int argc, char** argv);

static int
_echo(int argc, char** argv);

static int
_mecho(int argc, char** argv);


static int
_help(int argc, char** argv);


typedef void(*arg_function)(int,char**);

typedef struct {
  char *name;
  unsigned spawn;
  int(*function)(int, char**);
  char* help;
  char* usage;
} builtin_t;

static builtin_t builtins[] = {
  {"ps", 0, _kernel_stats, "Display process status", 0},
  {"free", 0, _malloc_stats, "Display some fairly meaningless memory stats", 0},
  {"cat", 0, _cat, "print a file", 0},
  {"cp", 0, _cp, "copy files", 0},
  {"mv", 0, _mv, "move files", 0},
  {"ls", 0, _ls, "list directory contents", "[-l] [file ...]"},
  {"rm", 0, _rm, "remove files", "[-f] [file...]"},
  {"pwd", 0, _pwd, "print the current working directory", 0},
  {"cd", 0, _cd, "change the current working directory", 0},
  {"sh", 0, _sh, "execute a shell", "[-d] -c command"},
  {"mkdir", 0, _mkdir, "make new directories", 0},
  {"rwolf", 1, _rwolf, "run a raycasting demo", 0},
  {"stress", 0, shell_stress, "run a stress test", 0},
  {"diff", 0, _diff, "compare files", 0},
  {"rt", 0, runtest_rt, "run tests", "[-r] [testfiles]"},
  {"test", 0, commands_test, "run a torture test", 0},
  {"version", 0, _version, "print the BitOS version", 0},
  {"kernel", 0, _kernel, "load a new kernel", 0},
  {"touch", 0, _touch, "change the file access and modification times", 0},
  {"time", 0,_time, "time command execution", 0},
  {"kill", 0, _kill, "terminate or message a thread", "[-message id] [-d data] tid"},
  {"spawn", 0, _spawn, "spawn a new command", 0},
  {"echo", 0, _echo, "print arguments", 0},
  {"mecho", 0, _mecho, "echo any messages received back to the sender", 0},
  {"help", 0, _help, "print help", "[command]"}
};

static unsigned numBuiltins = sizeof(builtins)/sizeof(builtin_t);


char* stripDash(char* argument)
{
  char* p = argument;
  for (; *p != 0 && *p == '-'; p++);
  return p;
}


static void usage(char* command)
{
  for (unsigned i = 0; i < numBuiltins; i++) {
    if (strcmp(command, builtins[i].name) == 0) {
      if (builtins[i].usage) {
	printf("usage: %s ", command);
	printf(builtins[i].usage);
	printf("\n");
      }
      break;
    }
  }
}


static void
printHelpSummary()
{
  printf("Built in commands:\n\n");
    int longestString = 0;
  for (unsigned i = 0; i < numBuiltins; i++) {
    int len = strlen(builtins[i].name);
    if (len > longestString) {
      longestString = len;
    }
  }
  
  char formatString[255];
  sprintf(formatString, " %%-%ds", longestString+4);
  
  for (unsigned i = 0; i < numBuiltins; i++) {
    printf(formatString, builtins[i].name);
    if (i % 3 == 2) {
      printf("\n");
    }
  }
  printf("\n");
}


static void
printHelp(const char* command)
{
  for (unsigned i = 0; i < numBuiltins; i++) {
    if (strcmp(command, builtins[i].name) == 0) {
      printf("%s - %s\n", builtins[i].name, builtins[i].help);
      usage(builtins[i].name);
      break;
    }
  }
}


static int
_help(int argc, char** argv)
{ 
  if (argc == 1) {
    printHelpSummary();
    usage(argv[0]);
  } else if (argc == 2) {
    printHelp(argv[1]);
  } else {
    usage(argv[0]);
  }

  return 0;
}


static int
_version(int argc, char** argv)
{
  printf("BitOS %s\n", _bft->kernel_version());
  return 0;
}

static int
_rwolf(int argc, char** argv)
{
  return _bft->wolf(argc, argv);
}


static int
_mv(int argc, char** argv)
{
  if (argc != 3) {
    printf("usage: %ssrc dest\n", argv[0]);
    return 1;
  }

  return rename(argv[1], argv[2]);
}


static int
_cp(int argc, char** argv)
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
_mkdir(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s path\n", argv[0]);
    return 1;
  }

  return mkdir(argv[1], 0);
}


static int
_runTestBuild(int argc, char** argv)
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
    if (thread_run(cleanCommandPtr) != 0 ||
	thread_run(elfCommandPtr) != 0) {
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
commands_test(int argc, char** argv)
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

  thread_h t1 = kernel_threadSpawn(&_runTestBuild, argv1, 0); 
  thread_h t2 = kernel_threadSpawn(&_runTestBuild, argv2, 0);

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
_pwd(int argc, char** argv)
{
  char buffer[PATH_MAX];
  printf("%s\n", getcwd(buffer, PATH_MAX));
  return 0;
}


static int 
_cd(int argc, char** argv)
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
_time(int argc, char** argv)
{
  if (argc == 1) {
    printf("usage: %s command\n", argv[0]);
    return -1;
  }  

  char** newArgv = shell_argvDup(argc, argv, 1);

  char* command = argv_reconstruct(newArgv);

  struct timeval start, end, diff;
  
  gettimeofday(&start,0);

  int success = shell_threadLoad(command) == 0;
  
  gettimeofday(&end,0);

  free(command);
  argv_free(newArgv);
  
  shell_timeval_subtract(&diff, &end, &start);
  
  printf("completed [%s] in %ld.%03ld\n", success ? "success" : "failed", diff.tv_sec, (diff.tv_usec/1000));

  return success;
}


static int 
_diff(int argc, char** argv)
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
_echo(int argc, char** argv)
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
_spawn(int argc, char** argv)
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
_kill(int argc, char** argv)
{
  int signo = SIGINT;
  int data = 0;
  int tid;
  char c;

  opterr = 0;

  while ((c = getopt(argc, argv, "d:1::2::3::4::5::6::7::8::9::")) != -1) {
    switch (c) {
    case 'd':
      if (sscanf(optarg, "%d", &data) != 1) {
	fprintf(stderr, "%s: invalid data -- %s\n", argv[0], optarg);
	usage(argv[0]);
	return -1;
      }
      break;
    default:
      if ((char)optopt >= '1' && (char)optopt <= '9') {
	char* opt = argv[optind-1];
	if (*opt != '-'|| sscanf((opt+1), "%d", &signo) != 1) {
	  fprintf(stderr, "%s: invalid option -- %s\n", argv[0], argv[optind-1]);
	  usage(argv[0]);
	  return -1;
	}
      } else {
	  fprintf(stderr, "%s: invalid option -- %s\n", argv[0], argv[optind-1]);
	  usage(argv[0]);
	  return -1;
      }
    }
  }
   
  if (optind >= argc) {
    usage(argv[0]);
    return -1;
  }

  if (sscanf(argv[optind], "%d", &tid) != 1) {
    fprintf(stderr, "%s: invalid tid -- %s\n", argv[0], optarg);
    usage(argv[0]);
    return -1;
  } 

  printf("Sending message id %d to %d with data %d\n", signo, tid, data);    
  if (message_send((thread_h)tid, signo, (void*)data) != 0) {
    fprintf(stderr, "%s: failed to send message\n", argv[0]);
    return -1;
  }  

  return 0;
}


static int
_ls(int argc, char** argv)
{
  char path[PATH_MAX];
  int c;
  int longFlag = 0;
  
  while ((c = getopt(argc, argv, "l")) != -1) {  
    switch (c) {
    case 'l':
      longFlag = 1;
      break;
    default:
      usage(argv[0]);
      return -1;
      break;
    }
  }


  if (optind < argc) {
    for (int i = optind; i < argc; i++) {
      shell_listPath(argv[i], argc, argv, longFlag);
    }
  } else {
    getcwd(path, PATH_MAX);
    shell_listPath(path, argc, argv, longFlag);
  }
  
  return 0;
}


static int 
_rm(int argc, char** argv)
{
  int force = 0;
  int c;
  
  while ((c = getopt(argc, argv, "f")) != -1) {  
    switch (c) {
    case 'f':
      force = 1;
      break;
    case '?':
      usage(argv[0]);
      return -1;
      break;
    }
  }

  if (optind >= argc) {
    usage(argv[0]);
    return -1;
  }
  
  for (int i = optind; i < argc; i++) {
    if (unlink(argv[i]) != 0) {
      if (!force) {
	printf("%s: failed to rm %s\n", argv[0], argv[i]);
	return 1;
      }
    }
  } 

  return 0;
} 


static int 
_cat(int argc, char** argv)
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


static int
_touch(int argc, char** argv)
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
_kernel(int argc, char** argv)
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
_kernel_stats(int argc, char** argv)
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
_malloc_stats(int argc, char** argv)
{
  malloc_stats();
  return 0;
}


static int
_sh(int argc, char** argv)
{
  signal(SIGINT, SIG_DFL); 
  int hasOptionC = 0;
  int dontQuit = 0;
  int c;

  while ((c = getopt(argc, argv, "cd")) != -1) {  
    switch (c) {
    case 'c':
      hasOptionC = 1;
      break;
    case 'd':
      dontQuit = 1;
      break;
    case '?':
      usage(argv[0]);
      return -1;
      break;
    }
  }

  if (!hasOptionC) {
    usage(argv[0]);
    return 1;
  }
 
  shell_execBuiltinFromArgv(argc, argv, optind);
  
  if (dontQuit) {
    for (;;) {
      kernel_threadBlocked();
    }
  }

  return 0;
}

static void 
_mecho_handler(int message, thread_h sender, void* data)
{
  printf("Got %d from %d with data %d\n", message, (unsigned)sender, (unsigned)data);
  message_send(sender, message, data);
}

static int 
_mecho(int argc, char** argv)
{
  message_handle(0, _mecho_handler);
  
  signal(9, SIG_DFL);

  for (;;) {
    kernel_threadBlocked();
  }
  return 0;
}


int 
commands_execBuiltin(int argc, char** argv)
{
  // reset gotopt globals
  optind = 0;
  opterr = 1;

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
commands_launchBuiltin(int argc, char** argv)
{
  // reset gotopt globals
  optind = 0;

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


void 
commands_usage(char* command)
{
  usage(command);
}
