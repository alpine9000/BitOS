#pragma once

#ifdef _KERNEL_BUILD

#include <sys/dirent.h>
#include <sys/stat.h>

int      
file_write(int file, char *ptr, int len);

int      
file_open(char *name, int flags, int* _errno);

int      
file_read(int file, char *ptr, int len);

int      
file_close(int file);

int      
file_isatty(int file);

int      
file_fstat(int file, struct stat *st);

int      
file_stat(const char *path, struct stat *st, int* _errno);

int      
file_access(char* pathname, int mode, struct stat* st);

DIR*     
file_opendir(const char *dirname);

int      
file_mkdir(const char *path, mode_t mode);

int      
file_lseek(int file, int ptr, int dir);

int      
file_unlink(char* path);

int      
file_rename(const char* old, const char* new);

int      
file_chdir(char* path);

//void*          
//file_loadElf(unsigned fd, unsigned** image, unsigned *imageSize);
void (*file_loadElf(unsigned fd, unsigned** image, unsigned *imageSize))();

int            
file_pipe(int fds[2]);

struct dirent *
file_readdir(DIR *dirp);


void 
file_loadElfKernel(unsigned fd);

#else

#define file_loadElfKernel(x) _bft->file_loadElfKernel(x)

#endif

