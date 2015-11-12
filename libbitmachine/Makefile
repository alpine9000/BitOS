ELF_PATH =/usr/local/sh-elf/bin/
CC=$(ELF_PATH)sh-elf-gcc
AR=$(ELF_PATH)sh-elf-ar
LD=$(ELF_PATH)sh-elf-ld

#-std=gnu99
CFLAGS=-D_KERNEL_ASSERTS -D_KERNEL_BUILD -O3 -Wfatal-errors -Werror -Wall -Wextra -Wno-unused-parameter -I. -m2e -funit-at-a-time -falign-jumps #-fdata-sections -ffunction-sections -fno-function-cse 
LIBOBJ=cpp.o glcdfont.o gfx.o console.o audio.o simulator.o interrupts.o file.o syscall.o kernel_asm.o kernel_client_asm.o kernel.o window.o memory.o syscalls.o bft_kernel.o malloc.o  panic.o argv.o process.o shell.o build.o
ASMOBJ=start.o
CLIENTOBJ=bft.o kernel_client_asm.o syscalls.o panic.o client.o termios.o shell.o argv.o build.o
CRT_OBJS=crt1.o crti.o crtbegin.o crtend.o crtn.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
	#$(CC) -S $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

%.o: %.S
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.s
	$(CC) -c -o $@ $< $(CFLAGS)

all: libbitmachine.a libc-bitos.a libc-kernel.a $(CRT_OBJS)

libbitmachine.a: $(LIBOBJ) $(ASMOBJ)
	$(AR) rcs libbitmachine.a $(LIBOBJ) $(ASMOBJ)

libc-bitos.a: $(CLIENTOBJ)
	$(AR) rcs libc-bitos.a $(CLIENTOBJ) libc/*.o

libc-kernel.a: libc-bitos.a
	$(AR) rcs libc-kernel.a libc/*.o
	cp libc-kernel.a ..

clean:
	-rm -f libc-bitos.a libc-kernel.a libbitmachine.a $(CRT_OBJS) $(LIBOBJ) $(CLIENTOBJ) $(ASMOBJ) $(LIBOBJ:.o=.d) $(CRT_OBJS:.o=.d) $(CLIENTOBJ:.o=.d) *~

-include $(LIBOBJ:.o=.d) $(CRT_OBJS:.o=.d) $(CLIENTOBJ:.o=.d)