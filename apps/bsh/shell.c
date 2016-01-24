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
#include <fcntl.h>
#include <readline/readline.h>
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
#include "commands.h"

#define print(x) printf(x);fflush(stdout)
#define flushc(x) putchar(x); fflush(stdout)

unsigned shell_windowWidth = ((gfx_fontWidth+gfx_spaceWidth)*80);
unsigned shell_windowHeight = (gfx_fontHeight*24);
static unsigned shell_fileBlockSize = 1024*1024;
static thread_h shell_runningThreadId = INVALID_THREAD;


int _shell_complete = 0;


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
  commands_execBuiltin(argc2-argvSkip, argv3);
  argv_free(argv2);
  argv_free(argv3);
  return 0;

}


int 
shell_rcopy(char* src, char* dest)
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
	  shell_rcopy(sbuffer, dbuffer);
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


struct timeval* 
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


static void 
_lsdir(char* path, int argc, char** argv)
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

int
shell_listPath(char* path, char* cwd, int argc, char** argv)
{
  struct stat statBuffer;

  if (stat(path, &statBuffer) != 0) {
    fprintf(stderr, "%s: %s: %s\n", argv[0], path, strerror(errno));
    return 1;
  }
  
  if (statBuffer.st_mode & S_IFDIR) {
    chdir(path);
    _lsdir(path, argc, argv);
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

static void 
_shell_sigintHandler(int sig)
{
  if (shell_runningThreadId != INVALID_THREAD) {
    printf("Killing %d\n", (unsigned)shell_runningThreadId);
    kill((int)shell_runningThreadId, SIGINT);
    shell_runningThreadId = INVALID_THREAD;
  }
}

static int _shell_inputFd = -1;

void
shell_init()
{
  int fd[2];
  pipe(fd);
  _shell_inputFd = fd[0];
  int flags = fcntl(_shell_inputFd, F_GETFL, 0);
  fcntl(_shell_inputFd, F_SETFL, flags | O_NONBLOCK);
  signal(SIGINT, _shell_sigintHandler);
}

void
shell_cleanup()
{
  close(_shell_inputFd);
}

int
shell_getc()
{
  int result;
  char c;
  
  do {
    result = read(_shell_inputFd, &c, 1);
    if (result == -2) {
      if (_console_char_avail()) {
	int ch = _console_read_char();
	if (ch != -1) {
	  c = ch;
	  write(_shell_inputFd, &c, 1);
	}
      } else {
       	kernel_threadBlocked();
      }
    }
  } while (result == -2);

  return (int)c;
}


int 
shell_threadLoad(char* commandLine)
{ 
  FILE *fp = popen(commandLine, "r");

  int fd = fileno(fp);

  shell_runningThreadId = kernel_threadGetIdForStdout(fd);

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);


  for(;fp != 0;) {
    char c;
    int result;
    if ((result = read(fileno(fp), &c, 1)) <= 0) {
      if (result == -2) { //blocked pipe
	if (_console_char_avail()) {
	  int ch = _console_read_char();
	  if (ch != -1) {
	    char _c = ch;
	    write(_shell_inputFd, &_c, 1);
	  }	  
	}
	kernel_threadBlocked();
      } else {
	kernel_threadBlocked();
	break;
      }
    } else {
      printf("%c", c);
      fflush(stdout);
    }
  }

  if (fp) {
    return pclose(fp) == 0;
  }
  
  return 0;
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

  shell_globArgv(cmd, &argc, &argv);

  shell_runningThreadId = INVALID_THREAD;

  if (commands_launchBuiltin(argc, argv) != 0) {
    if (argc > 0) {
      if (background) {
	thread_spawn(cmd);
      } else {
	shell_threadLoad(cmd);
      }
    }
  }

  argv_free(argv);
}


int 
shell_isOption(char* argv, char option)
{
  return argv[0] == '-' && argv[1] == option;
}


int 
shell_isLongOption(char* argv, char* option)
{
  if (argv[0] == '-' && argv[1] == '-') {
    return strcmp(&argv[2], option) == 0;
  }

  return 0;
}

int 
shell_hasOption(int argc, char** argv, char option)
{
  for (int i = 1; i < argc; i++) {
    if (shell_isOption(argv[i], option)) {
      return 1;
    }
  }

  return 0;
}

int 
shell_hasLongOption(int argc, char** argv, char* option)
{
  for (int i = 1; i < argc; i++) {
    if (shell_isLongOption(argv[i], option)) {
      return 1;
    }
  }

  return 0;
}
