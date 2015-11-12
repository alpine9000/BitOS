#ifndef __PANIC_H
#define __PANIC_H

#define panic() __asm__("trapa #35")

#endif //__PANIC_H
