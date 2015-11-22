#pragma once

#include <sys/time.h>
#include <sys/times.h>

#ifdef _KERNEL_BUILD

void 
simulator_startStopWatch();

void 
simulator_stopStopWatch();

unsigned 
simulator_stopWatchElapsed();

void 
simulator_yield();

void 
simulator_stop();

unsigned 
simulator_timeSeconds();

unsigned 
simulator_timeMicroSeconds();

unsigned 
simulator_elapsedMilliSeconds();

int 
simulator_gtod(struct timeval *tv, void *tz);

int 
simulator_printf(const char *format, ...);

#endif
