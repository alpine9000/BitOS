#pragma once

#ifdef _KERNEL_ASSERTS

#define KERNEL_ASSERT_KERNEL_MODE() volatile unsigned __pr; \
  __asm__ volatile ("sts pr,r0\n"    \
		    "mov.l r0,%0"    \
		    :"=m"(__pr)      \
		    :                \
		    :"r0", "memory");\
  kernel_assertKernelMode(__pr);

#define KERNEL_ASSERT_INTERRUPTS_DISABLED() {  volatile unsigned sr;\
  __asm__ volatile ("stc sr,r0\n"     \
		    "mov.l r0,%0"     \
		    :"=m"(sr)         \
		    :                 \
		    :"r0", "memory"); \
  if ((sr & 0xF0) != 0xF0) {  panic(" kernel assesrtion failed: interrupts not disabled");} }


#define KERNEL_ASSERT_INTERRUPTS_ENABLED() {  volatile unsigned sr;\
  __asm__ volatile ("stc sr,r0\n"     \
		    "mov.l r0,%0"     \
		    :"=m"(sr)         \
		    :                 \
		    :"r0", "memory"); \
  if ((sr & 0xF0) != 0x00) {  panic(" kernel assesrtion failed: interrupts not enabled");} }

#else 
#define KERNEL_ASSERT_KERNEL_MODE()
#define KERNEL_ASSERT_INTERRUPTS_DISABLED()
#define KERNEL_ASSERT_INTERRUPTS_ENABLED()

#endif
