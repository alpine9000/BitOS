include ../makefiles/optimize.mk
WARNINGS = -pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
#-D_INCLUDE_BUILDER
#ASSERTS=-D_KERNEL_ASSERTS
CFLAGS=$(ASSERTS) -D_KERNEL_BUILD $(OPTIMIZE) $(WARNINGS) -I. -m2e -falign-jumps
LIBOBJ=cpp.o glcdfont.o gfx.o console.o audio.o simulator.o interrupts.o file.o syscall.o kernel_asm.o kernel_client_asm.o kernel.o window.o memory.o syscalls.o bft_kernel.o malloc.o  panic.o argv.o thread.o shell.o stress.o termios.o
ASMOBJ=start.o
CLIENTOBJ=bft.o kernel_client_asm.o syscalls.o panic.o client.o shell.o stress.o argv.o
CRT_OBJS=crt1.o crti.o crtbegin.o crtend.o crtn.o

include ../makefiles/base.mk
include ../makefiles/libc.mk

all: libbitmachine.a libc-bitos.a libc-kernel.a $(CRT_OBJS)

libbitmachine.a: $(LIBOBJ) $(ASMOBJ)
	$(AR) rcs libbitmachine.a $(LIBOBJ) $(ASMOBJ)

libc-bitos.a: $(CLIENTOBJ)
	$(AR) rcs libc-bitos.a $(CLIENTOBJ) $(LIBC_OBJS) 

libc-kernel.a: libc-bitos.a
	$(AR) rcs libc-kernel.a $(LIBC_OBJS) 

touch:
	touch $(LIBOBJ) $(ASMOBJ) $(CLIENTOBJ) $(CRT_OBJS)

clean:
	-rm -f libc-bitos.a libc-kernel.a libbitmachine.a $(CRT_OBJS) $(LIBOBJ) $(CLIENTOBJ) $(ASMOBJ) $(LIBOBJ:.o=.d) $(CRT_OBJS:.o=.d) $(CLIENTOBJ:.o=.d) *~

-include $(LIBOBJ:.o=.d) $(CRT_OBJS:.o=.d) $(CLIENTOBJ:.o=.d)