#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simulator.h"
#include "peripheral.h"
#include "kernel.h"
#include "argv.h"

#define lock() unsigned ___ints_disabled = kernel_disableInts()
#define unlock() kernel_enableInts(___ints_disabled)


void 
simulator_stop()
{
  peripheral.simulator.stop = 1;
  peripheral.simulator.yield = 1;
}

void 
simulator_yield()
{
  peripheral.simulator.yield = 1;
}

void 
simulator_startStopWatch()
{
  peripheral.simulator.startStopWatch = 1;
}

void 
simulator_stopStopWatch()
{
  peripheral.simulator.stopStopWatch = 1;
}

unsigned 
simulator_stopWatchElapsed()
{
  return peripheral.simulator.stopWatchElapsed;
}

unsigned 
simulator_timeSeconds()
{
  return peripheral.time.seconds;
}

unsigned 
simulator_timeMicroSeconds()
{
  return peripheral.time.useconds;
}


unsigned 
simulator_elapsedMilliSeconds()
{
  return peripheral.time.elapsedMilliSeconds;
}


int 
simulator_gtod(struct timeval *tv, void *tz)
{
  tv->tv_sec = simulator_timeSeconds();;
  tv->tv_usec = simulator_timeMicroSeconds();;
  return 0;
}

int 
simulator_printf(const char * format, ...)
{
#define BIG_NUMBER 32768
  static char buffer[BIG_NUMBER];
  va_list argList;
  va_start(argList, format);
  vsnprintf(buffer, BIG_NUMBER, format, argList);
  va_end(argList);

  kernel_memoryBarrier();
  peripheral.simulator.print = (unsigned)&buffer;

  return 1;
}

static void 
_simulator_argvDump(char** vector)
{
  int argc = 0;

  if (vector != 0) {
    for (; argc < 255 && vector[argc] != 0; argc++) {
      if (argc != 0) {
	simulator_printf(" ");
      }
      simulator_printf("%s", vector[argc]);
    }
  }
}


char** 
simulator_kernelArgv()
{
  lock();

  int length = peripheral.kernelCmdLength;
  char* cmd = malloc(length+1);
  cmd[length] = 0;
  for (int i = 0; i < length; ++i) {
    cmd[i] = (char)peripheral.kernelCmd;
  }
  char **argv = argv_build(cmd);

  simulator_printf("kernel command line: ");
  _simulator_argvDump(argv);
  simulator_printf("\n");
  
  unlock();

  return argv;
}

