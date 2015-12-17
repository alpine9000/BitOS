#pragma once

#include <stdio.h>
#include "types.h"

#ifdef _KERNEL_BUILD

FILE* 
thread_open(char* command);

int 
thread_close(FILE* fp);

#endif

int 
thread_spawn(char* command);

int 
thread_load(char* commandLine);



