ELF_PATH =/usr/local/sh-elf/bin/
LIB_PATH=/usr/local/sh-elf/
CC=$(ELF_PATH)sh-elf-gcc
CPP=$(ELF_PATH)sh-elf-g++
AR=$(ELF_PATH)sh-elf-ar
LD=$(ELF_PATH)sh-elf-ld
STRIP=$(ELF_PATH)sh-elf-strip -g

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.s
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) -o $@ $<

%.S: %.c
	$(CC) -S $(CFLAGS) $*.c -o $*.s

