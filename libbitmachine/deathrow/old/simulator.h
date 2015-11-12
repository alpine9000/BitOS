#ifndef __SIMULATOR_H
#define __SIMULATOR_H

void simulator_startStopWatch();
void simulator_stopStopWatch();
unsigned long simulator_stopWatchElapsed();
void simulator_yield();
void simulator_stop();
unsigned long simulator_timeSeconds();
unsigned long simulator_timeMicroSeconds();

unsigned long simulator_elapsedMilliSeconds();

#endif //__SIMULATOR_H
