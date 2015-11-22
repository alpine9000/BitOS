#pragma once
#include "bft.h"
#include <stdio.h>

#ifdef _KERNEL_BUILD

FILE* 
process_open(char* command);

int 
process_close(FILE* fp);

int 
process_spawn(char* command);

int 
process_load(char* commandLine);

int 
process_wait(unsigned pid);

#else

#define process_spawn(command) _bft->process_spawn(command)
#define process_load(commandLine) _bft->process_load(commandLine)
#define process_wait(pid) _bft->process_wait(pid)

#endif




