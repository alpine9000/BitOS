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
#include <libgen.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>
#include "bft.h"
#include "kernel.h"
#include "console.h"
#include "thread.h"
#include "argv.h"
#include "window.h"
#include "gfx.h"
#include "file.h"
#include "shell.h"
#include "runtest.h"

#define print(x) printf(x);fflush(stdout)
#define flushc(x) putchar(x); fflush(stdout)


unsigned shell_windowWidth = ((gfx_fontWidth+gfx_spaceWidth)*80);
unsigned shell_windowHeight = (gfx_fontHeight*24);
static unsigned shell_fileBlockSize = 1024*1024;

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
_mkdir(int argc, char** argv);

static int
runShell(int argc, char** argv);

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

int 
shell_execBuiltin(int argc, char** argv);

static int 
shell_time(int argc, char** argv);

static int
shell_kill(int argc, char** argv);

static int
shell_catchSignal(int argc, char** argv);

static int
shell_spawn(int argc, char** argv);

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
  {"mkdir", 0, _mkdir},
  {"rwolf", 1, rwolf},
  {"stress", 0, shell_stress},
  {"diff", 0, diff},
  {"rt", 0,runtest},
#ifdef _INCLUDE_BUILDER
  {"bcc", 0, bcc},
  {"b", 0, build},
  {"c", 0, build2},
#endif
  {"test", 0, shell_test},
  {"shell", 1, runShell},
  {"version", 0, version},
  {"kernel", 0, kernel},
  {"touch", 0, touch},
  {"time", 0,shell_time},
  {"kill", 0, shell_kill},
  {"catch", 0, shell_catchSignal},
  {"spawn", 0, shell_spawn}
};

static unsigned numBuiltins = sizeof(builtins)/sizeof(builtin_t);

int _shell_complete = 0;

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

const char *
shell_getFilenameExt(const char *filename) 
{
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}


int 
shell_copy(char* s, char* dest_filename)
{
  char d[PATH_MAX];

  struct stat buf;
  if (stat(dest_filename, &buf) == 0 && S_ISDIR(buf.st_mode)) {
    int len = strlen(dest_filename);
    if (len > 1 && dest_filename[len-1] == '/') {
      dest_filename[len-1] = 0;
    }
    sprintf(d, "%s/%s", dest_filename, basename(s));
  } else {
    strncpy(d, dest_filename, PATH_MAX);
  }

  int src = open(s, O_RDONLY);

  if (src != -1) {
    int dest = open(d, O_WRONLY|O_CREAT);
    if (dest != -1) {
      int bytes;
      char *buffer = malloc(shell_fileBlockSize);
      do {
	bytes = read(src, buffer, shell_fileBlockSize);
	if (bytes) {
	  write(dest, buffer, bytes);
	}
      } while (bytes > 0);
      
      free(buffer);
      close(dest);   
    } else {
      fprintf(stderr, "cp: %s: %s\n", d, strerror(errno));
      return -1;
    }
    close(src);
    return 0;
  } else {
    fprintf(stderr, "cp: %s: %s\n", s, strerror(errno));
    return -1;
  }
}

char** 
shell_argvDup(int argc, char** argv, int skip)
{

  if (argv != 0) {
    char **vector = malloc((argc+1-skip)*sizeof(char*));
    int i;
    for (i = 0;i  <  argc-skip; i++) {
      vector[i] = malloc(strlen(argv[i+skip])+1);
      strcpy(vector[i], argv[i+skip]);
    }
    vector[i] = 0;
    return vector;
  }

  return 0;
}

int
shell_execBuiltinFromArgv(int argc, char** argv, int argvSkip)
{
  if (!(argc > argvSkip)) { 
    return 1;
  }

  int argc2;
  char** argv2;
  char* cmd = argv_reconstruct(argv);
  shell_globArgv(cmd, &argc2, &argv2);
  free(cmd);
  char** argv3 = shell_argvDup(argc2, argv2, argvSkip);
  shell_execBuiltin(argc2-argvSkip, argv3);
  argv_free(argv2);
  argv_free(argv3);
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

static int 
rcopy(char* src, char* dest)
{
  struct stat buf;
  if (stat(src, &buf) == 0) {
    if (S_ISDIR(buf.st_mode)) {
      printf("%s is a dir: mkdir %s\n", src, dest);
      mkdir(dest, 0);
      DIR* dirp = opendir(src);
      struct dirent* dp;
      
      do {
	if ((dp = readdir(dirp)) != NULL) {
	  char sbuffer[PATH_MAX];
	  char dbuffer[PATH_MAX];
	  sprintf(sbuffer, "%s/%s", src, dp->d_name);
	  sprintf(dbuffer, "%s/%s", dest, dp->d_name);
	  printf("rcopy %s %s\n", sbuffer, dbuffer);
	  rcopy(sbuffer, dbuffer);
	}
      } while (dp != NULL);	
      closedir(dirp);
    } else {
      printf("copy from %s to %s\n", src, dest);
      shell_copy(src, dest);
    }
  }
  return 0;
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
    return rcopy(s, d);
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

unsigned 
getSR()
{
  __asm__ volatile ("stc sr,r0\n"
	  :
	  :
          :"r0", "memory");
 register unsigned r0 __asm__("r0");
 return r0;
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

  KERNEL_MODE(); // This only has an effect when running on the kernel side (executing this code directly from init

  char* cleanCommand = "make clean -C ";
  char* cleanCommandPtr = malloc(strlen(cleanCommand) + pathLen+1);
  sprintf(cleanCommandPtr, "%s%s", cleanCommand, argv[0]);

  char* elfCommand = "make elf -C ";
  char* elfCommandPtr = malloc(strlen(elfCommand) + pathLen+1);
  sprintf(elfCommandPtr, "%s%s", elfCommand, argv[0]);

  USER_MODE();// This only has an effect when running on the kernel side (executing this code directly from init

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

  rcopy("/usr/local/src/BitOS", "/usr/local/src/BitOS.2");

  char cmd1[1024];
  char cmd2[1024];


  snprintf(cmd1, 1024, "/usr/local/src/BitOS 0 %s", argv[1]);
  snprintf(cmd2, 1024, "/usr/local/src/BitOS.2 1 %s", argv[1]);


  KERNEL_MODE(); // This only has an effect when running on the kernel side (executing this code directly from init
  char** argv1 = argv_build(cmd1);
  char** argv2 = argv_build(cmd2);
  USER_MODE(); // This only has an effect when running on the kernel side (executing this code directly from init


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

static struct timeval* 
shell_timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;
  
  /* Return 1 if result is negative. */
  return result;
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

static void 
lsdir(char* path, int argc, char** argv)
{
  struct stat statBuffer;
  int len = 0;
  DIR* dirp = opendir(path);

  if (dirp) {
    struct dirent* dp;
    
    do {
      if ((dp = readdir(dirp)) != NULL) {
	char buffer[80];
	if (stat(dp->d_name, &statBuffer) == 0) {
	  int l = sprintf(buffer, "%ld", statBuffer.st_size);
	  if (l > len) {
	    len = l;
	  }
	} 
      }
    } while (dp != NULL);

    closedir(dirp);
    dirp = opendir(path);
    
    do {
      if ((dp = readdir(dirp)) != NULL) {
	if (stat(dp->d_name, &statBuffer) == 0) {
	  time_t t = statBuffer.st_mtime;
	  struct tm lt;
	  localtime_r(&t, &lt);
	  char timbuf[80];
	  strftime(timbuf, sizeof(timbuf), "%c", &lt);
	  printf("%*ld %s %s\n", len+1, statBuffer.st_size, timbuf, dp->d_name);
	} else {
	  printf("%s - stat failed\n", dp->d_name);
	}
      }
    } while (dp != NULL);

    closedir(dirp);
  } else {
    printf("%s: %s: No such file or directory\n", argv[0], path);
  }
}

static int
listPath(char* path, char* cwd, int argc, char** argv)
{
  struct stat statBuffer;

  if (stat(path, &statBuffer) != 0) {
    fprintf(stderr, "%s: %s: %s\n", argv[0], path, strerror(errno));
    return 1;
  }
  
  if (statBuffer.st_mode & S_IFDIR) {
    chdir(path);
    lsdir(path, argc, argv);
    chdir(cwd);
  } else {
    time_t t = statBuffer.st_mtime;
    struct tm lt;
    localtime_r(&t, &lt);
    char timbuf[80];
    strftime(timbuf, sizeof(timbuf), "%c", &lt);
    printf("%ld %s %s\n", statBuffer.st_size, timbuf, path);
  }

  return 0;
}

int 
shell_filesAreIdentical(char* p1, char* p2)
{
  struct stat statBuffer;
  int length1 = -1, length2 = -1;
  if (stat(p1, &statBuffer) == 0) {
    length1 = statBuffer.st_size;
  }
  if (stat(p2, &statBuffer) == 0) {
    length2 = statBuffer.st_size;
  }
  if (length1 != length2) {
    return 0;
  }

  int src = open(p1, O_RDONLY);
  int dest = open(p2, O_RDONLY);
  char *buffer1 = malloc(shell_fileBlockSize);
  char *buffer2 = malloc(shell_fileBlockSize);
  int result = 1;
  
  if (src != -1 && dest != -1) {
    int bytes1, bytes2;

    do {
      bytes1 = read(src, buffer1, shell_fileBlockSize);
      bytes2 = read(dest, buffer2, shell_fileBlockSize);
      if (bytes1 != bytes2 || memcmp(buffer1, buffer2, bytes1) != 0) {
	  result = 0;
	  break;
      }
    } while (bytes1 > 0 && bytes2 > 0);
  }
  
  close(src);
  close(dest);
  free(buffer1);
  free(buffer2);

  return result;
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
shell_spawn(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s command\n", argv[0]);
  }

  thread_h tid = thread_spawn(argv[1]);

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
    int signo = atoi(argv[1]);
    printf("Sending %d to %d\n", signo, pid);
    if (kill(pid, signo) != 0) {
      printf("%s: failed\n", argv[0]);
    }
  } else {
    fprintf(stderr, "usage: %s tid [signal number]\n", argv[0]);
  }

  return 0;
}

void shell_signalHandler(int signo)
{
  printf("bsh: caught signal %d\n", signo);
}

int
shell_catchSignal(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s signalNumber\n", argv[0]);
    return -1;
  }

  int signo = atoi(argv[1]);
  printf("bsh: catching signal %d...\n", signo);
  if (signal(signo, shell_signalHandler) == SIG_ERR) {
    fprintf(stderr, "%s: error: failed to register handler\n", argv[0]);
    return -1;
  }
  
  for(;;) {
    kernel_threadBlocked();
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
      listPath(argv[i], cwd, argc, argv);
    }
  }

  if (!file) {
    getcwd(path, PATH_MAX);
    getcwd(cwd, PATH_MAX);
    listPath(path, cwd, argc, argv);
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
readchar()
{
  if (_console_char_avail()) {
    return _console_read_char();
  } 

  return -1;
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

typedef struct {
  int argc;
  char** argv;
} argv_t;

void
shell_globArgv(char* command, int* out_argc, char*** out_argv)
{
  char** argv = argv_build(command);
  int argc = argv_argc(argv);

  if (argc > 1) {
    glob_t g;
    g.gl_offs = 1;
    for (int i = 1; i < argc; i++) {
      if (i == 1) {
	glob(argv[i], GLOB_NOCHECK | GLOB_DOOFFS, NULL, &g);
      } else {
	glob(argv[i], GLOB_NOCHECK | GLOB_DOOFFS | GLOB_APPEND, NULL, &g);
      }
    }
    
    g.gl_pathv[0] = argv[0];
    
    *out_argv = shell_argvDup(g.gl_pathc + g.gl_offs, g.gl_pathv, 0);
    *out_argc = g.gl_pathc + g.gl_offs;

    argv_free(argv);
  } else {
    *out_argv = argv;
    *out_argc = argc;
  }
}


void 
shell_exec(char* cmd)
{
  
  int background = 0, len = strlen(cmd);

  for (int i = len-1; i > 0; i--) {
    if (cmd[i] == '&') {
      cmd[i] = 0;
      background = 1;
      break;
    }
  }


  char** argv;
  int argc;

  KERNEL_MODE(); // This only has an effect when running on the kernel side (executing this code directly from init
  shell_globArgv(cmd, &argc, &argv);
  USER_MODE(); // This only has an effect when running on the kernel side (executing this code directly from init

  if (argc > 0) {
    for (unsigned i = 0; i < numBuiltins; i++) {
      if (strcmp(argv[0], builtins[i].name) == 0) {
	if (builtins[i].spawn) {
	  kernel_threadSpawn(builtins[i].function, argv, 0);
	  argv_free(argv);
	  return;
	} else {
	  builtins[i].function(argv_argc(argv), argv);
	  argv_free(argv);
	}
	return;
      } 
    }
  }

  if (argc > 0) {
    if (background) {
      thread_spawn(cmd);
    } else {
      thread_load(cmd);
    }
  }

  argv_free(argv);
}



void 
shell()
{
  while (!_shell_complete) {
    char buf[255];
    print(">");	
    for (int i = 0;;) {
      int c  = readchar();
      if (c >= 0) {
	switch (c) {
	case 8:
	  if (i > 0) {
	    i--;
	    console_backspace();
	    console_deleteAtCursor();
	  }
	  break;
	case 13:
	  printf("\n");
	  buf[i] = 0;
	  if (strcmp(buf, "exit") == 0) {
	    return;
	  }
	  if (i > 0) {
	    shell_exec(buf);
	  }
	  print(">");
	  i = 0;
	  break;
	default:
	  flushc(c);
	  buf[i] = c;
	  i++;
	}
	
      }
      kernel_threadBlocked();
    }
  } 
}

static int
runShell(int argc, char** argv)
{
  unsigned w = 320, h = 200;
  window_h window = window_create("Shell", 20, 20, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  shell(); 
  window_close(window);
  kernel_threadDie(0);
  return 0;
}
