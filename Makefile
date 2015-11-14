ELF_PATH =/usr/local/sh-elf/bin/
LIB_PATH=/usr/local/sh-elf/
CC=$(ELF_PATH)sh-elf-gcc
CPP=$(ELF_PATH)sh-elf-g++
AR=$(ELF_PATH)sh-elf-ar
LD=$(ELF_PATH)sh-elf-ld
STRIP=$(ELF_PATH)sh-elf-strip


CFLAGS=-g -O3 -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -D_KERNEL_BUILD  -I/usr/local/sh-elf/include -I. -I./libbitmachine -m2e  -funit-at-a-time -falign-jumps -fdelete-null-pointer-checks  #-fdata-sections -ffunction-sections -fno-function-cse 
CPPFLAGS = -D_KERNEL_BUILD -O3 -Wall -Werror -Wextra -Wno-unused-parameter -Wno-char-subscripts  -m2e -funit-at-a-time -falign-jumps  -I./libbitmachine #-D_POSIX_C_SOURCE -D_XOPEN_SOURCE 
LDFLAGS= -L$(LIB_PATH)sh-elf/lib/m2e/ -L/usr/local/sh-elf/lib/gcc/sh-elf/5.2.0/m2e/ -L/usr/local/sh-elf/lib

LIBS =  /usr/local/sh-elf/sh-elf/lib/m2e/crt0.o -L.  -L./libbitmachine  --start-group -lwolf -lbitmachine -lc-kernel -lm -lgcc --end-group

IMG_OBJ= media/martini.o
WOLF_MEDIA=apps/wolf/media/eagle.o apps/wolf/media/redbrick.o apps/wolf/media/purplestone.o apps/wolf/media/greystone.o apps/wolf/media/bluestone.o apps/wolf/media/mossy.o apps/wolf/media/wood.o apps/wolf/media/colorstone.o 
WOLF_APP_OBJS=apps/wolf/wolf.o
WOLF_OBJ=$(WOLF_MEDA) $(WOLF_APP_OBJS)
CORE_OBJS = vector.o $(SOUND_OBJ) $(IMG_OBJ) init.o 
OBJ_FILES = $(CORE_OBJS) $(WOLF_APP_OBJS)
ALL_OBJS = $(OBJ_FILES) $(WOLF_OBJ)

BIN_FILE = bin/bitos.bin
ELF_FILE = bin/bitos.elf

LIB_BITMACHINE=./libbitmachine
BSH=./apps/bsh
BEMACS=./apps/bemacs

all: $(LIB_BITMACHINE) $(BSH) $(BEMACS) $(ELF_FILE) 

elf: $(LIB_BITMACHINE) $(ELF_FILE) 

gdrive: all
	cp $(ELF_FILE) ~/Google\ Drive/BitFS/bin

web: all
	cp $(ELF_FILE) ~/Google\ Drive/Projects/BitMachine/Web/BitFS/

.PHONY: $(LIB_BITMACHINE)

$(LIB_BITMACHINE):
	make  -C $(LIB_BITMACHINE)

.PHONY: $(BSH)

$(BSH):
	make  -C $(BSH)
	make  -C $(BEMACS)

.SECONDARY:
%.rgba: %.png
	convert $*.png $*.rgba

.SECONDARY:
%.c: %.rgba
	./bin/bin2c $*.rgba $*.c

.SECONDARY:
%.c: %.wav
	./bin2c $*.wav $*.c

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@
	$(CC) -MM $(CPPFLAGS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp


%.S: %.c
	$(CC) -S $(CFLAGS) $*.c -o $*.s


%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

clean:
	-rm -f $(BIN_FILE) libwolf.a bitos.sym bitos.map *~ *.rgba y.* lex.yy.? y.tab.? $(ELF_FILE) $(ALL_OBJS) $(ALL_OBJS:%.o=%.d)
	#-rm -f $(IMG_OBJ:.o=.rgba) $(IMG_OBJ:.o=.c) $(SOUND_OBJ:.o=.c)
	rm -f $(IMG_OBJ) $(SOUND_OBJ) $(WOLF_OBJ)
	$(MAKE) -C libbitmachine clean
	$(MAKE) -C apps/wolf clean
	$(MAKE) -C apps/bemacs clean
	$(MAKE) -C apps/bsh clean

backup:
	rsync --delete -crv ~/Projects/BitOS/ ~/Google\ Drive/Projects/BitOS

.SECONDARY:
libwolf.a: $(WOLF_MEDIA)
	$(AR) rcs libwolf.a $(WOLF_MEDIA)


.SECONDARY:
$(ELF_FILE): $(OBJ_FILES) libwolf.a libbitmachine/libc-kernel.a libbitmachine/libbitmachine.a 
	$(LD) $(LDFLAGS) -T start.l -no-keep-memory -Map bin/bitos.map -o $(ELF_FILE) $(OBJ_FILES) $(LIBS) 

strip:
	$(STRIP) -g $(ELF_FILE)

$(BIN_FILE): $(ELF_FILE)
	sh-elf-objcopy --remove-section=.bss --remove-section=.peripheral -v -O binary $(ELF_FILE) $(BIN_FILE)


-include $(ALL_OBJS:%.o=%.d) 

TOOLS_BASE=~/Projects/bitos-build
GCC_BASE=$(TOOLS_BASE)/gcc-5.2.0.bitos.O3
BINUTILS_BASE = $(TOOLS_BASE)/binutils.bitos
relink-tools:	
	-rm $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus
	make -C $(GCC_BASE)
	sh-elf-strip $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus
	cp $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus ~/Google\ Drive/BitFS/bin/
	-rm $(BINUTILS_BASE)/gas/as-new $(BINUTILS_BASE)/ld/ld-new $(BINUTILS_BASE)/binutils/ar
	make -C $(BINUTILS_BASE) all-gas all-ld
	make -C $(BINUTILS_BASE)/binutils ar
	cp $(BINUTILS_BASE)/binutils/ar ~/Google\ Drive/BitFS/bin/ar
	cp $(BINUTILS_BASE)/gas/as-new ~/Google\ Drive/BitFS/bin/as
	cp $(BINUTILS_BASE)/ld/ld-new ~/Google\ Drive/BitFS/bin/ld
	sh-elf-strip ~/Google\ Drive/BitFS/bin/ar ~/Google\ Drive/BitFS/bin/as ~/Google\ Drive/BitFS/bin/ld

local.zip:
	-rm -rf ~/Projects/bitos-build/local/src/BitOS
	cp -r ~/Projects/BitOS ~/Projects/bitos-build/local/src/
	-rm -rf  ~/Projects/bitos-build/local/src/BitOS/newlib-2.0.0-r
	cp ~/Projects/BitOS/libbitmachine/libc-bitos.a ~/Projects/bitos-build/local/sh-elf/sh-elf/lib/m2e/libc.a
	-rm ~/Projects/bitos-build/local.zip
	cp ~/Google\ Drive/BitFS/bin/* ~/Projects/bitos-build/local/bin/
	cd ~/Projects/bitos-build/; zip -r local.zip local
	cp ~/Projects/bitos-build/local.zip ~/Google\ Drive/BitFS
	cp ~/Projects/bitos-build/local.zip ~/Google\ Drive/Projects/BitMachine/Web/BitFS

full: gdrive web relink-tools local.zip