IMG_OBJ=media/eagle.o media/redbrick.o media/purplestone.o media/greystone.o media/bluestone.o media/mossy.o media/wood.o media/colorstone.o
OBJ_FILES=wolf.o $(IMG_OBJ) $(SOUND_OBJ)
CFLAGS=-O3  -Wall -Werror -std=gnu99 -m2e -I../../libbitmachine #-I/usr/local/sh-elf/include/freetype2/
CPPFLAGS =-O3  -Wall -Werror  -Wextra -Wno-unused-parameter -Wno-char-subscripts  -D_POSIX_C_SOURCE -D_XOPEN_SOURCE -m2e -funit-at-a-time -falign-jumps  -I../../libbitmachine
ELF_PATH =/usr/local/sh-elf/bin/
LIB_PATH=/usr/local/sh-elf/
CC=$(ELF_PATH)sh-elf-gcc
CPP=$(ELF_PATH)sh-elf-g++
LIBS=-lm  #-lfreetype 
LDFLAGS=-L../../libbitmachine -L/usr/local/sh-elf/lib
ELF_FILE=wolf


all: $(ELF_FILE)

%.o: %.s
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@
	$(CC) -MM $(CPPFLAGS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

.SECONDARY:
%.rgba: %.png
	convert $*.png $*.rgba

.SECONDARY:
%.c: %.rgba
	../../bin2c $*.rgba $*.c

.SECONDARY:
%.c: %.wav
	../../bin2c $*.wav $*.c


.SECONDARY:
$(ELF_FILE): $(OBJ_FILES)
	$(CC) -m2e $(LDFLAGS) -Wl,-Map -Wl,a.map -pie -o $(ELF_FILE) $(OBJ_FILES) $(LIBS) 
	cp $(ELF_FILE) ~/Google\ Drive/BitFS/bin/

clean:
	-rm -f *.o $(ELF_FILE)

-include $(OBJ_FILES:%.o=%.d) 