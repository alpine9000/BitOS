#include "panic.h"
#include "gfx.h"
#include "simulator.h"

void
panic(char* message) 
{
  __asm__("sts pr,r5");
  __asm__("mov #4,r1");
  __asm__("sub r1,r5");
  __asm__("trapa #35");
}

