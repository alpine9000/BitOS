#ifndef __FILE_H
#define __FILE_H

#include <sys/stat.h>

int _file_write ( int file, char *ptr, int len);
int _file_open (char *name, int flags);
int _file_read (int file, char *ptr, int len);
int _file_close(int file);
int _file_isatty(int file);
int _file_fstat (int file, struct stat *st);
int _file_unlink(char* file);

#endif
