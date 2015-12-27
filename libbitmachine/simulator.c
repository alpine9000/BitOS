#include <stdarg.h>
#include <stdio.h>
#include "simulator.h"
#include "peripheral.h"


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

  peripheral.simulator.print = (unsigned)&buffer;

  return 1;
}
