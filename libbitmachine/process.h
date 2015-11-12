#pragma once

#include <stdio.h>

#ifdef _KERNEL_BUILD

FILE* process_open(char* command);
int process_close(FILE* fp);

#endif

int process_spawn(char* command);
int process_load(char* commandLine);
int process_wait(unsigned pid);


