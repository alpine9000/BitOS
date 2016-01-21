typedef void (*fp) (void);
extern void stack (void);
extern void reset (void);
extern void _syscall(void);
extern void _panic(void);
extern void _illegalSlot(void);
extern void _kernel_tick_asm(void);
extern void _kernel_blocked_asm(void);
extern void _kernel_kill_asm(void);
extern void _kernel_kill_thread_asm(void);

typedef struct vt {
  unsigned long reset;
  unsigned long stack;
  unsigned long INT_Manual_Reset_PC;
  unsigned long INT_Manual_Reset_SP;
  unsigned long INT_Illegal_code;
  unsigned long reserved0;
  unsigned long INT_Illegal_slot;
  unsigned long reserved1;
  unsigned long reserved2;
  unsigned long INT_CPU_Address;
  unsigned long INT_DMAC_Address;
  unsigned long INT_NMI;
  unsigned long INT_User_Break;
  unsigned long INT_FPU;
  unsigned long INT_HUDI;
  unsigned long reserved3[17];
  unsigned long INT_TRAPA32;
  unsigned long INT_TRAPA33;
  unsigned long INT_TRAPA34;
  unsigned long INT_TRAPA35;
  unsigned long INT_TRAPA36;
  unsigned long INT_TRAPA37;
  unsigned long INT_TRAPA38;
  unsigned long INT_TRAPA39;
  unsigned long INT_TRAPA40;
  unsigned long INT_TRAPA41;
  unsigned long INT_TRAPA42;
  unsigned long INT_TRAPA43;
  unsigned long INT_TRAPA44;
  unsigned long INT_TRAPA45;
  unsigned long INT_TRAPA46;
  unsigned long INT_TRAPA47;
  unsigned long INT_TRAPA48;
  unsigned long INT_TRAPA49;
  unsigned long INT_TRAPA50;
  unsigned long INT_TRAPA51;
  unsigned long INT_TRAPA52;
  unsigned long INT_TRAPA53;
  unsigned long INT_TRAPA54;
  unsigned long INT_TRAPA55;
  unsigned long INT_TRAPA56;
  unsigned long INT_TRAPA57;
  unsigned long INT_TRAPA58;
  unsigned long INT_TRAPA59;
  unsigned long INT_TRAPA60;
  unsigned long INT_TRAPA61;
  unsigned long INT_TRAPA62;
  unsigned long INT_TRAPA63;

  unsigned char data[3048];
} vt_t;

volatile  const vt_t v __attribute__ ((section (".vector"))) = {
  (unsigned long)reset,
  (unsigned long)stack, 
  (unsigned long)_kernel_tick_asm,// INT_Manual_Reset_PC
  (unsigned long)0,// INT_Manual_Reset_SP
  (unsigned long)0,// INT_Illegal_code
  (unsigned long)0,// reserved0
  (unsigned long)_illegalSlot,// INT_Illegal_slot
  (unsigned long)0,// reserved1
  (unsigned long)0,// reserved2
  (unsigned long)0,// INT_CPU_Address
  (unsigned long)0,// INT_DMAC_Address
  (unsigned long)0,// INT_NMI
  (unsigned long)0,// INT_User_Break
  (unsigned long)0,// INT_FPU
  (unsigned long)0,// INT_HUDI
  {0}, // reserved
  (unsigned long)0,// INT_TRAPA32
  (unsigned long)0,// INT_TRAPA33
  (unsigned long)_syscall,                // INT_TRAPA34
  (unsigned long)_panic,                  // INT_TRAPA35
  (unsigned long)_kernel_blocked_asm,     // INT_TRAPA36
  (unsigned long)_kernel_kill_asm,        // INT_TRAPA37
  (unsigned long)_kernel_kill_thread_asm, // INT_TRAPA38
  (unsigned long)0,// INT_TRAPA39
  (unsigned long)0,// INT_TRAPA40
  (unsigned long)0,// INT_TRAPA41
  (unsigned long)0,// INT_TRAPA42
  (unsigned long)0,// INT_TRAPA43
  (unsigned long)0,// INT_TRAPA44
  (unsigned long)0,// INT_TRAPA45
  (unsigned long)0,// INT_TRAPA46
  (unsigned long)0,// INT_TRAPA47
  (unsigned long)0,// INT_TRAPA48
  (unsigned long)0,// INT_TRAPA49
  (unsigned long)0,// INT_TRAPA50
  (unsigned long)0,// INT_TRAPA51
  (unsigned long)0,// INT_TRAPA52
  (unsigned long)0,// INT_TRAPA53
  (unsigned long)0,// INT_TRAPA54
  (unsigned long)0,// INT_TRAPA55
  (unsigned long)0,// INT_TRAPA56
  (unsigned long)0,// INT_TRAPA57
  (unsigned long)0,// INT_TRAPA58
  (unsigned long)0,// INT_TRAPA59
  (unsigned long)0,// INT_TRAPA60
  (unsigned long)0,// INT_TRAPA61
  (unsigned long)0,// INT_TRAPA62
  (unsigned long)0,// INT_TRAPA63
  {0}
};
