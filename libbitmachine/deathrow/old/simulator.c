#include "simulator.h"
#include "peripheral.h"

void simulator_stop()
{
  peripheral.simulator.stop = 1;
  peripheral.simulator.yield = 1;
}

void simulator_yield()
{
  peripheral.simulator.yield = 1;
}

void simulator_startStopWatch()
{
  peripheral.simulator.startStopWatch = 1;
}

void simulator_stopStopWatch()
{
  peripheral.simulator.stopStopWatch = 1;
}

unsigned long simulator_stopWatchElapsed()
{
  return peripheral.simulator.stopWatchElapsed;
}

unsigned long simulator_timeSeconds()
{
  return peripheral.time.seconds;
}

unsigned long simulator_timeMicroSeconds()
{
  return peripheral.time.useconds;
}


unsigned long simulator_elapsedMilliSeconds()
{
  return peripheral.time.elapsedMilliSeconds;
}
