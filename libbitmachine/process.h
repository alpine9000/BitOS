#pragma once

#include <stdio.h>
#include "types.h"

#ifdef _KERNEL_BUILD

FILE* 
thread_open(char* command);

int 
thread_close(FILE* fp);

#endif

thread_h
thread_spawn(char* command);

thread_h 
thread_spawnFileDescriptors(char* command, int in, int out, int err);

int 
thread_load(char* commandLine);



