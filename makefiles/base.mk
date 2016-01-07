ELF_PATH =/usr/local/sh-elf/bin/
LIB_PATH=/usr/local/sh-elf/
CC=$(ELF_PATH)sh-elf-gcc
CPP=$(ELF_PATH)sh-elf-g++
AR=$(ELF_PATH)sh-elf-ar
LD=$(ELF_PATH)sh-elf-ld
STRIP=$(ELF_PATH)sh-elf-strip -g

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	@#$(CC) -S $< $(CFLAGS)
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.s
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) -o $@ $<
	$(CC) -MM $(CPPFLAGS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp


%.S: %.c
	$(CC) -S $(CFLAGS) $*.c -o $*.s


.SECONDARY:
%.rgba: %.png
	convert $*.png $*.rgba

.SECONDARY:
%.c: %.rgba
	./bin/bin2c $*.rgba $*.c

.SECONDARY:
%.c: %.wav
	./bin2c $*.wav $*.c
