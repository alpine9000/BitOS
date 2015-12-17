#undef _KERNEL_BUILD
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


extern void 
wolf();

extern void 
build(int argc, char** argv);

extern void 
build2(int argc, char** argv);

extern void 
bcc(int argc, char** argv);

static void
mv(int argc, char** argv);

static void
cd(int argc, char** argv);

static void
cp(int argc, char** argv);

static void
pwd(int argc, char** argv);

static void
ls(int argc, char** argv);

static void
rm(int argc, char** argv);

static void
cat(int argc, char** argv);

static void
_mkdir(int argc, char** argv);

static void
runShell();

static void
test(int argc, char** argv);

static void
version(int argc, char** argv);

static void
panic(int argc, char** argv);

static void
kernel(int argc, char**argv);

static void
shell_kernel_stats();

static void
shell_malloc_stats();

typedef void(*arg_function)(int,char**);

typedef struct {
  char *name;
  unsigned spawn;
  void(*function)();
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
  {"panic", 0, panic},
  {"version", 0, version},
  {"kernel", 0, kernel}
};

static unsigned numBuiltins = sizeof(builtins)/sizeof(builtin_t);

char **gitversion = 0;

static void 
version(int argc, char** argv)
{
  printf("BitOS version %s\n", *gitversion);
}

static char *
basename(char *path)
{
  char *base = strrchr(path, '/');
  return base ? base+1 : path;
}

static void 
panic(int argc, char** argv)
{
  unsigned lock;
  _bft->_bitos_lock_close(&lock);
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


static void 
mv(int argc, char** argv)
{
  if (argc != 3) {
    printf("usage: %ssrc dest\n", argv[0]);
    return;
  }

  rename(argv[1], argv[2]);
}

static void 
cp(int argc, char** argv)
{
  if (!(argc == 3 || argc == 4)) {
    printf("usage: %s [-r] src dest\n", argv[0]);
    return;
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
    copy(s, d);
  } else {
    rcopy(s, d);
  }
}

static void 
_mkdir(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s path\n", argv[0]);
    return;
  }

  mkdir(argv[1], 0);
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

extern unsigned 
kernel_disableInts();
extern  void 
kernel_enableInts(unsigned );


static void 
test(int argc, char** argv)
{
#if 0
  printf("SR = %x\n", getSR());

  unsigned disabled = kernel_disableInts();
  
  printf("kernel_disableInts()= %d\n", disabled);

  printf("SR = %x\n", getSR());

  printf("kernel_enableInts(false);\n");

  kernel_enableInts(0);

  printf("SR = %x\n", getSR());

  if (argc == 1) {
    printf("kernel_enableInts(true);\n");
    
    kernel_enableInts(1);

    printf("SR = %x\n", getSR());

  } else {
    printf("keeping ints disabled\n");
  }

  for(;;);
#endif
}


static void 
pwd(int argc, char** argv)
{
  char buffer[PATH_MAX];
  printf("%s\n", getcwd(buffer, PATH_MAX));
}


static void 
cd(int argc, char** argv)
{
  if (argc == 1) {
    chdir("~");
    return;
  }

  if (argc != 2) {
    printf("usage: %s path\n", argv[0]);
    return;
  }

  chdir(argv[1]);
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
 
static void 
ls(int argc, char** argv)
{
  char cwd[PATH_MAX];
  char path[PATH_MAX];
  struct stat statBuffer;

  if (argc > 2) {
    printf("usage: %s [path]\n", argv[0]);
    return;
  }

  if (argc == 1) {
    getcwd(path, PATH_MAX);
    getcwd(cwd, PATH_MAX);
  } else {
    strcpy(path, argv[1]);
  }

  if (stat(path, &statBuffer) != 0) {
    fprintf(stderr, "%s: %s: %s\n", argv[0], path, strerror(errno));
    return;
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

}


static void 
rm(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s [path]\n", argv[0]);
    return;
  }

  unlink(argv[1]);
} 

static void 
cat(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s file\n", argv[0]);
    return;
  }
  
  FILE *fp = fopen(argv[1], "r");
  
  if (fp == 0) {
    printf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
    return;
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
}

static void 
kernel(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s file\n", argv[0]);
    return;
  }
  
  FILE *fp = fopen(argv[1], "r");
  
  if (fp == 0) {
    printf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
    return;
  }
  
  file_loadElfKernel(fileno(fp));
}

static void 
shell_kernel_stats()
{
  kernel_stats();
}

static void 
shell_malloc_stats()
{
  malloc_stats();
}


static int 
readchar()
{
  if (_console_char_avail()) {
    return _console_read_char();
  } 

  return -1;
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

  char** argv = argv_build(cmd);
  int argc = argv_argc(argv);


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
  while (1) {
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

static void 
runShell()
{
  unsigned w = 320, h = 200;
  window_h window = window_create("Shell", 20, 20, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_threadSetWindow(window);
  shell(); 
  window_close(window);
  kernel_threadDie(0);
}
