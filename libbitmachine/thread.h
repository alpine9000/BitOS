#pragma once

#include <stdio.h>
#include "types.h"

#ifdef _KERNEL_BUILD

FILE* 
thread_fopen(char* command);

int 
thread_fclose(FILE* fp);

#endif

thread_h
thread_spawn(char* command);

thread_h 
thread_spawnFileDescriptors(char* command, int in, int out, int err);

int 
thread_run(char* commandLine);

int
thread_open(char* command);

int
thread_close(int fd);
