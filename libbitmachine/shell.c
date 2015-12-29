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
#include "process.h"
#include "argv.h"
#include "window.h"
#include "gfx.h"
#include "file.h"

#define print(x) printf(x);fflush(stdout)
#define flushc(x) putchar(x); fflush(stdout)

void shell_exec(char* cmd);

#define shellWindowWidth  ((gfx_fontWidth+gfx_spaceWidth)*80)
#define shellWindowHeight ( gfx_fontHeight*24)

extern int
wolf();

extern int 
build(int argc, char** argv);

extern int 
build2(int argc, char** argv);

extern int
bcc(int argc, char** argv);

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
test(int argc, char** argv);

static int
version(int argc, char** argv);

static int
kernel(int argc, char** argv);

static int
shell_kernel_stats(int argc, char** argv);

static int
shell_malloc_stats(int argc, char** argv);

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
  {"mkdir", 0, _mkdir},
  {"bcc", 0, bcc},
  {"rwolf", 1, wolf},
  {"b", 0, build},
  {"c", 0, build2},
  {"shell", 1, runShell},
  {"test", 0, test},
  {"version", 0, version},
  {"kernel", 0, kernel},
  {"touch", 0, touch}
};

static unsigned numBuiltins = sizeof(builtins)/sizeof(builtin_t);

char **gitversion = 0;

int _shell_complete = 0;

static int
version(int argc, char** argv)
{
  printf("BitOS version %s\n", *gitversion);
  return 0;
}

static char *
basename(char *path)
{
  char *base = strrchr(path, '/');
  return base ? base+1 : path;
}

static int 
copy(char* s, char* dest_filename)
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
      char *buffer = malloc(1024*100);
      do {
	bytes = read(src, buffer, 1024*100);
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
      copy(src, dest);
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
    return copy(s, d);
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


static void 
testBuild(char* cmd, int(*builder)(int, char**), unsigned x)
{
  static int numTestBuilds = 2;
  unsigned w = shellWindowWidth, h = shellWindowHeight;
  setbuf(stdout, NULL); 
  window_h window = window_create(cmd, x, 0, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  char** argv = argv_build(cmd);
  int argc = argv_argc(argv);
  for (int i = 0; i < numTestBuilds; i++) {
    builder(argc, argv);
  }

  printf("Success!!!\n");

  for (;;) {
    kernel_threadBlocked();
  }
}


static int 
runTestBuild1(int argc, char** argv)
{
  testBuild("b -f", build, 0);
  return 0;
}

static int
runTestBuild2(int argc, char** argv)
{
  testBuild("c -f", build2, shellWindowWidth);
  return 0;
}


static int
test(int argc, char** argv)
{
  kernel_threadSpawn(&runTestBuild1, 0, 0);
  kernel_threadSpawn(&runTestBuild2, 0, 0); 
  do {
    kernel_threadBlocked();
  } while(1);
  
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

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (unlink(argv[i]) != 0) {
	printf("%s: failed to rm %s\n", argv[0], argv[i]);
	return 1;
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
	printf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
	return 1;
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
  kernel_stats();
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

static 
char** 
shell_argvDup(int argc, char** argv)
{

  if (argv != 0) {
    char **vector = malloc((argc+1)*sizeof(char*));
    int i;
    for (i = 0;i  <  argc; i++) {
      vector[i] = malloc(strlen(argv[i]+1));
      strcpy(vector[i], argv[i]);
    }
    vector[i] = 0;
    return vector;
  }

  return 0;
}

static 
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
    
    *out_argv = shell_argvDup(g.gl_pathc + g.gl_offs, g.gl_pathv);
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

  shell_globArgv(cmd, &argc, &argv);

  if (argc > 0) {
    for (unsigned i = 0; i < numBuiltins; i++) {
      if (strcmp(argv[0], builtins[i].name) == 0) {
	if (builtins[i].spawn) {
	  kernel_threadSpawn(builtins[i].function, argv, 0);
	  // argv free'd by kernel on thread exit
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
