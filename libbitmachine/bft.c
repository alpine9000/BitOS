#include "bft.h"
#include "syscall.h"

_bft_t* _bft = 0;

void 
bft_init()
{
  _bft = (_bft_t*)__trap34(SYS_bft);
}
