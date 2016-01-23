include ../makefiles/optimize.mk
WARNINGS = -pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
ASSERTS=-D_KERNEL_ASSERTS
CFLAGS=$(ASSERTS) -D_KERNEL_BUILD $(OPTIMIZE) $(WARNINGS) -I. -m2e -falign-jumps
LIB_OBJS=argv.o audio.o bft_kernel.o console.o cpp.o file.o gfx.o glcdfont.o  interrupts.o kernel.o kernel_asm.o kernel_client_asm.o lock.o  malloc.o memory.o panic.o signal.o simulator.o syscall.o syscalls.o  termios.o thread.o window.o 
ASM_OBJS=start.o
CLIENT_OBJS=argv.o bft.o client.o kernel_client_asm.o panic.o syscalls.o
CRT_OBJS=crt1.o crti.o crtbegin.o crtend.o crtn.o

include ../makefiles/base.mk
include ../makefiles/libc.mk

all: libbitmachine.a libc-bitos.a libc-kernel.a $(CRT_OBJS)

libbitmachine.a: $(LIB_OBJS) $(ASM_OBJS)
	$(AR) rcs libbitmachine.a $(LIB_OBJS) $(ASM_OBJS)

libc-bitos.a: $(CLIENT_OBJS)
	$(AR) rcs libc-bitos.a $(CLIENT_OBJS) $(LIBC_OBJS) 

libc-kernel.a: libc-bitos.a
	$(AR) rcs libc-kernel.a $(LIBC_OBJS) 

touch:
	touch $(LIB_OBJS) $(ASM_OBJS) $(CLIENT_OBJS) $(CRT_OBJS)

clean:
	-rm -f  libc-bitos.a libc-kernel.a libbitmachine.a $(CRT_OBJS) $(LIB_OBJS) $(CLIENT_OBJS) $(ASM_OBJS)  $(LIB_OBJS:.o=.d) $(CRT_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d)  *~

-include $(LIBOBJ:.o=.d) $(CRT_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d)