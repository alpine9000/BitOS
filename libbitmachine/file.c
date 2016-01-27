#include <errno.h>
#include <unistd.h>
#include <sys/syslimits.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "console.h"
#include "peripheral.h"
#include "file.h"
#include "panic.h"
#include "kernel.h"
#include "types.h"
#include "simulator.h"
#include "memory.h"

#define _file_lock() unsigned ___ints_disabled = kernel_disableInts()
#define _file_unlock() kernel_enableInts(___ints_disabled)

static char *
file_realpath(const char *path, char *resolved_path) 
{
  char buffer[PATH_MAX];

  buffer[0] = 0;

  if (path[0] == '.' && path[1] == 0) {
    getcwd(resolved_path, PATH_MAX);
    return resolved_path;
  } 
  
  if (path[0] == '~') {
    strcat(buffer, "/usr/local/home");
    strcat(buffer, &path[1]);
  } else {
      
    if (path[0] != '/') {
      getcwd(buffer, PATH_MAX);
      if (buffer[strlen(buffer)-1] != '/') {
	strcat(buffer, "/");
      }
    }
      
    strcat(buffer, path);
  }
    
  int d = 0;
  for (int s = 0; buffer[s] != 0; s++) {
    if (buffer[s] == '/') {
      if (s > 0 && buffer[s-1] == '/') {
	continue;
      }
    }
    if (buffer[s] == '.' && buffer[s+1] == '.') {
      unsigned slashCount = 0;
      while (d > 0 && slashCount < 2) {
	if (resolved_path[--d] == '/') {
	  slashCount++;
	}
      }
      s+=2;
    } else if (buffer[s] == '.' && buffer[s+1] == '/') {
      s++;
      while (buffer[s] != 0 && buffer[s] == '/') {
	s++;
      }

    }

    resolved_path[d++] = buffer[s];
  }

  if (d >= 3 && resolved_path[d-1] == '.' && resolved_path[d-2] == '/') {
    d--;
  }

  resolved_path[d] = 0;
      
  return resolved_path;  
}

int
file_chdir(char* path)
{
  char _path[PATH_MAX];
  file_realpath(path, _path);
  kernel_threadSetInfo(KERNEL_CURRENT_WORKING_DIRECTORY, (unsigned)_path);
  return 0;
}

static int
file_loaded(int fd)
{
  _file_lock();
  peripheral.file.fd = fd; 
  volatile int status = peripheral.file.status;
  _file_unlock();
  return status;
}


int
file_open(char *name, int flags, int* _errno)
{
  char path[PATH_MAX];
  file_realpath(name, path);

  if (strncmp(name, "/dev/urandom", 12) == 0) {
    *_errno = ENOENT;
    return -1;
  }

  _file_lock();
  volatile int fd = (volatile int)peripheral.file.fd;
  peripheral.file.fd = fd;
  peripheral.file.filename = (unsigned)path;
  peripheral.file.flags = (unsigned)flags;
  _file_unlock();

  unsigned loaded;
  for (;;) {
    loaded = file_loaded(fd);
    if (loaded == 0) {
      kernel_threadBlocked();
    } else {
      break;
    }
  }

  if (loaded == 1) {
    *_errno = 0;
    return fd;
  } else {
    *_errno = ENOENT;
    return -1;
  }
}


static int 
_file_read_stdin (int file, char *ptr, int len)
{
    int i;
  
    while (!_console_char_avail()) {
      kernel_threadBlocked();
    }
    
    for (i = 0; i < len; ++i) {
      if (_console_char_avail()) {
	int val = _console_read_char();
	*ptr++ = (char)val;
      } else {
	break;
      }
    }

    return i;
}

static int 
_file_read(int file, char *ptr, int len)
{
  if (file == STDIN_FILENO) {
    return _file_read_stdin(file, ptr, len);
  } else {
    _file_lock();
    peripheral.file.fd = file;
    peripheral.file.address = ptr;
    peripheral.file.readLength = len;
    int val = peripheral.file.doRead;
    if (val == -2) { // Blocked pipe
      _file_unlock();
      return val;
    }
    _file_unlock();

    // If successful, the number of bytes actually read is returned.  
    // Upon read-ing end-of-file, zero is returned.  
    // Otherwise, a -1 is returned and the global variable errno is set to indicate the error.

    return val;
  }
}

int 
file_read (int fd, char *ptr, int len)
{
  int val;
  do {
    val =  _file_read(fd, ptr, len);
    if (val == -2) {
      if (file_getOptions(fd) & O_NONBLOCK) {
	return val;
      } else {
       kernel_threadBlocked();
      }
    }
  } while (val == -2); // Blocked pipe
  return val;
}

static int 
_file_write_stdout ( int file, char *ptr, int len)
{
  int i = 0;
  for (i = 0; i < len; i++) {
    _console_write_char(*ptr);
    ptr++;
  }
  return len;
}

int 
file_write ( int file, char *ptr, int len)
{
  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    return _file_write_stdout(file, ptr, len);
  } else {
    _file_lock();
    peripheral.file.fd = file;
    peripheral.file.address = ptr;
    peripheral.file.writeLength = len;
    _file_unlock();
    return len;
  }
}

void 
__file_close(int file)
{
  _file_lock();
  peripheral.file.fd = file;
  peripheral.file.close = 1;
  _file_unlock();
}

static int 
_file_closeStatus(int fd)
{
  _file_lock();
  peripheral.file.fd = fd;
  volatile int status =  (int) peripheral.file.closeStatus;
  _file_unlock();
  return status;
}

static int 
_file_close(int file)
{
  if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
    fds_t* fds = kernel_threadGetFds();
    if (file == STDOUT_FILENO && fds->_stdout != STDOUT_FILENO) {
      close(fds->_stdout);
    }
    if (file == STDIN_FILENO && fds->_stdin != STDIN_FILENO) {
      close(fds->_stdin);
    }
    return 0;
  }
  __file_close(file);
  return 0;
}



int 
file_close(int file)
{
  if (_file_close(file) == -1) {
    return -1;
  }

  int status;

  for (;;) {
    status = _file_closeStatus(file);
    if (status == 1) {
      kernel_threadBlocked();
    } else {
      break;
    }
  }

  return status;
}

int 
file_isatty(int file)
{
  if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
    return 1;
  } else {
    return 0;
  }
}


int 
file_fstat (int file, struct stat *st)
{
  if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
    st->st_mode = S_IFCHR;
    return 0;
  } else {
    _file_lock();
    volatile int result;
    peripheral.file.fd = file;
    if (peripheral.file.status == 1) {
      st->st_size = peripheral.file.size;
      result = 0;
    } else {
      result = -1;
    }
    _file_unlock();
    return result;
  }
}

int _file_creat (const char *path,int mode) {
  panic("_file_creat - not implemented");
  return -1;
}


int
file_lseek (int file, int ptr, int dir) 
{
  _file_lock();
  peripheral.file.fd = file;
  peripheral.file.seekDirection = dir;
  peripheral.file.seek  = ptr;
  volatile int position = peripheral.file.position;
  _file_unlock();
  return position;
}


/*int _file_stat (int file, struct stat *st) {
  panic("_file_stat - not implemented");
  return -1;
  }*/


static int 
_file_stat(const char* pathname, int mode, struct stat *st) {
  char path[PATH_MAX];
  file_realpath(pathname, path);

  _file_lock();
  volatile int sd = peripheral.file.stat;
  peripheral.file.statStruct = (unsigned)st;
  peripheral.file.statPath = (unsigned)path;
  _file_unlock();
  return sd;

}

static int 
_file_statStatus(int sd)
{
  _file_lock();
  peripheral.file.stat = sd;
  volatile int status = peripheral.file.statStatus;
  _file_unlock();
  return status;
}

int 
file_stat(const char *path, struct stat *st, int* _errno)
{
  int sd = _file_stat(path, 0, st);    
  int a;
  for (;;) {
    a = _file_statStatus(sd);
    if (a == -1) {
      *_errno = ENOENT;
      return a;
    } else if (a == 0) {
      *_errno = 0;
      return a;
    }
    kernel_threadBlocked();
  }
}


static int 
_file_opendir(DIR *dirp, const char* filename)
{
  char path[PATH_MAX];

  file_realpath(filename, path);

  simulator_printf("filename = %s, path = %s\n", filename, path);

  _file_lock();
  peripheral.file.dir.opendirStruct = (unsigned)dirp;
  peripheral.file.dir.opendir = (unsigned)path;
  _file_unlock();
  return 0;
}


static int 
_file_opendirStatus(DIR *dirp)
{
  _file_lock();
  peripheral.file.dir.opendirStruct = (unsigned)dirp;
  volatile int status = peripheral.file.dir.opendir;
  _file_unlock();
  return status;
}

DIR *
file_opendir(const char *dirname)
{
  unsigned loaded;
  // Don't use kernel mode malloc for this
  DIR* p = memory_malloc(sizeof(struct dirent));

  _file_opendir(p, dirname);
  
  for (;;) {
    loaded = _file_opendirStatus(p);
    if (loaded == 0) {
      kernel_threadBlocked();
    } else {
      break;
    }
  }

  if (loaded == 1) {
    return p;
  } else {
    return 0;
  }
}

static int 
_file_readdir(struct dirent *dirp)
{
  _file_lock();
  peripheral.file.dir.readdir = (unsigned)dirp;
  volatile int status = peripheral.file.dir.readdir;
  _file_unlock();
  return status;
}

struct dirent *
file_readdir(DIR *dirp)
{
  if (_file_readdir((struct dirent*)dirp) == 0) {
    return (struct dirent*)dirp;
  } else {
    return 0;
  }
}

static int 
_file_mkdir(const char* path)
{
  char _path[PATH_MAX];
  file_realpath(path, _path);

  _file_lock();
  peripheral.file.dir.mkdir = (unsigned)_path;
  volatile int status = peripheral.file.dir.mkdir;
  _file_unlock();
  return status;
}

int
file_mkdir(const char *path, mode_t mode)
{
  for (;;) {
    int a = _file_mkdir(path);
    if (a == -1 || a == 0) {
      return a;
    }
    kernel_threadBlocked();
  }
}


static int 
_file_unlinkStatus(void* unlinkp)
{
  _file_lock();
  peripheral.file.unlink.descriptor = (unsigned)unlinkp;
  volatile int status = (int) peripheral.file.unlink.status;
  _file_unlock();
  return status;
}


int
file_unlink(char* path)
{
  char _path[PATH_MAX];

  file_realpath(path, _path);

  _file_lock();
  void* unlinkp = (void*) peripheral.file.unlink.descriptor;
  peripheral.file.unlink.path = (unsigned)_path;
  _file_unlock();

  int status;

  for (;;) {
    status = _file_unlinkStatus(unlinkp);
    if (status == 1) {
      kernel_threadBlocked();
    } else {
      break;
    }
  }

  return status;
}

static void* 
_file_rename(const char* old, const char* new)
{
  char src[PATH_MAX];
  char dest[PATH_MAX];

  file_realpath(old, src);
  file_realpath(new, dest);

  _file_lock();
  void* renamep = (void*) peripheral.file.rename.rename;
  peripheral.file.rename.oldPathname = (unsigned)src;
  peripheral.file.rename.newPathname = (unsigned)dest;
  _file_unlock();
  return renamep;

}

static int 
_file_renameStatus(void* renamep)
{
  _file_lock();
  peripheral.file.rename.rename = (unsigned)renamep;
  volatile int status = (int) peripheral.file.rename.status;
  _file_unlock();
  return status;
}

int
file_rename(const char *old, const char *new)
{
  void* renamep  =  _file_rename(old, new);
  int status;

  for (;;) {
    status = _file_renameStatus(renamep);
    if (status == 1) {
      kernel_threadBlocked();
    } else {
      break;
    }
  }

  return status;
}

int
file_pipe(int fds[2])
{
  int fd = open("/dev/pipe", O_CREAT);
  if (fd >= 0) {
    fds[0] = fds[1] = fd;
    return 0;
  } else {
    return fd;
  }
}


int (*file_loadElf(unsigned fd, unsigned** image, unsigned *imageSize))(int, char**x)
{
  _file_lock();
  
  peripheral.file.fd = fd;
  peripheral.file.elf.load = 1;
  KERNEL_MODE();
  *image = malloc(peripheral.file.elf.size);
  USER_MODE();
  *imageSize = peripheral.file.elf.size;
  peripheral.file.elf.relocate = (unsigned)*image;
  //  void* entry = (void(*)())peripheral.file.elf.entry;
  int(*entry)(int,char**) = (int(*)(int,char**))peripheral.file.elf.entry;
  
  _file_unlock();
 
 return entry;
}

void
file_loadElfKernel(unsigned fd)
{
  _file_lock();
  
  peripheral.file.fd = fd;
  peripheral.file.elf.kernelLoad = 1;
  
  _file_unlock();
}

int
file_setOptions(int fd, unsigned options)
{
  _file_lock();
  
  peripheral.file.fd = fd;
  peripheral.file.options = options;
  
  _file_unlock();

  return 0;
}

int
file_getOptions(int fd)
{
  _file_lock();
  
  peripheral.file.fd = fd;
  unsigned options = peripheral.file.options;
  
  _file_unlock();

  return options;
}
