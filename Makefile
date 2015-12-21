BITOS_PATH=~/Projects/BitOS
TOOLS_BASE=~/Projects/bitos-build
GITVERSION := $(shell git describe --abbrev=41 --dirty --always --tags)
WARNINGS = -pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
include ./optimize.mk
CPP_WARNINGS = $(WARNINGS) -Wno-char-subscripts 
CFLAGS=-DGITVERSION=\"$(GITVERSION)\" -g $(OPTIMIZE) $(WARNINGS) -D_KERNEL_BUILD  -I/usr/local/sh-elf/include -I. -I./libbitmachine -m2e  -funit-at-a-time -falign-jumps -fdelete-null-pointer-checks
CPPFLAGS = -D_KERNEL_BUILD $(OPTIMIZE) $(CPP_WARNINGS)  -m2e -funit-at-a-time -falign-jumps  -I./libbitmachine
LDFLAGS= -L$(LIB_PATH)sh-elf/lib/m2e/ -L/usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/m2e/ -L/usr/local/sh-elf/lib

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
SI=./apps/si


all: $(LIB_BITMACHINE) $(BSH) $(BEMACS) $(SI) $(ELF_FILE) 

version:
	@echo $(GITVERSION)

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
	make  -C $(SI)

.SECONDARY:
%.rgba: %.png
	convert $*.png $*.rgba

.SECONDARY:
%.c: %.rgba
	./bin/bin2c $*.rgba $*.c

.SECONDARY:
%.c: %.wav
	./bin2c $*.wav $*.c

include base.mk

clean:
	-rm -f $(BIN_FILE) libwolf.a bitos.sym bitos.map *~ *.rgba y.* lex.yy.? y.tab.? $(ELF_FILE) $(ALL_OBJS) $(ALL_OBJS:%.o=%.d)
	#-rm -f $(IMG_OBJ:.o=.rgba) $(IMG_OBJ:.o=.c) $(SOUND_OBJ:.o=.c)
	rm -f $(IMG_OBJ) $(SOUND_OBJ) $(WOLF_OBJ)
	$(MAKE) -C libbitmachine clean
	$(MAKE) -C apps/wolf clean
	$(MAKE) -C apps/bemacs clean
	$(MAKE) -C apps/bsh clean
	$(MAKE) -C apps/si clean

backup:
	rsync --delete -crv $(BITOS_PATH)/ ~/Google\ Drive/Projects/BitOS

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


#GCC_BASE=$(TOOLS_BASE)/gcc-5.2.0.bitos.O3
GCC_BASE=$(TOOLS_BASE)/gcc-5.3.0.bitos
BINUTILS_BASE = $(TOOLS_BASE)/binutils.bitos
relink-tools:	
	-rm $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus
	make -C $(GCC_BASE)
	$(STRIP) $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus
	cp $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus ~/Google\ Drive/BitFS/bin/
	-rm $(BINUTILS_BASE)/gas/as-new $(BINUTILS_BASE)/ld/ld-new $(BINUTILS_BASE)/binutils/ar
	make -C $(BINUTILS_BASE) all-gas all-ld
	make -C $(BINUTILS_BASE)/binutils ar
	cp $(BINUTILS_BASE)/binutils/ar ~/Google\ Drive/BitFS/bin/ar
	cp $(BINUTILS_BASE)/gas/as-new ~/Google\ Drive/BitFS/bin/as
	cp $(BINUTILS_BASE)/ld/ld-new ~/Google\ Drive/BitFS/bin/ld
	$(STRIP) ~/Google\ Drive/BitFS/bin/ar ~/Google\ Drive/BitFS/bin/as ~/Google\ Drive/BitFS/bin/ld

PATH := $(PATH):$(ELF_PATH)
local.zip:

	-rm -rf $(TOOLS_BASE)/local

	-rm $(BINUTILS_BASE)/gas/as-new $(BINUTILS_BASE)/ld/ld-new $(BINUTILS_BASE)/binutils/ar
	make -C $(BINUTILS_BASE) all-gas all-ld
	make -C $(BINUTILS_BASE)/binutils ar
	-mkdir $(TOOLS_BASE)/local/
	-mkdir $(TOOLS_BASE)/local/bin
	cp $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus $(TOOLS_BASE)/local/bin
	cp $(BINUTILS_BASE)/binutils/ar $(TOOLS_BASE)/local/bin
	cp $(BINUTILS_BASE)/gas/as-new  $(TOOLS_BASE)/local/bin/as
	cp $(BINUTILS_BASE)/ld/ld-new  $(TOOLS_BASE)/local/bin/ld
	cp $(BITOS_PATH)/apps/bemacs/bemacs $(TOOLS_BASE)/local/bin
	cp $(BITOS_PATH)/apps/bsh/bsh $(TOOLS_BASE)/local/bin
	cp $(BITOS_PATH)/apps/si/si $(TOOLS_BASE)/local/bin
	cp $(BITOS_PATH)/bin/bitos.elf $(TOOLS_BASE)/local/bin
	cp -r /usr/local/sh-elf $(TOOLS_BASE)/local
	rm -r $(TOOLS_BASE)/local/sh-elf/share
	rm -r $(TOOLS_BASE)/local/sh-elf/bin
	rm -r $(TOOLS_BASE)/local/sh-elf/libexec
	rm -r $(TOOLS_BASE)/local/sh-elf/sh-elf/bin
	rm -r $(TOOLS_BASE)/local/sh-elf/sh-elf/lib/ml
	rm -r $(TOOLS_BASE)/local/sh-elf/sh-elf/lib/*.*
	-rm -rf $(TOOLS_BASE)/local/src/BitOS
	mkdir $(TOOLS_BASE)/local/src
	cp -r $(BITOS_PATH) $(TOOLS_BASE)/local/src/BitOS
	cp -r $(BITOS_PATH) $(TOOLS_BASE)/local/src/BitOS.2/
	rm -rf $(TOOLS_BASE)/local/src/BitOS/.git
	rm -rf $(TOOLS_BASE)/local/src/BitOS2/.git
	-rm -rf  $(TOOLS_BASE)/local/src/BitOS/newlib-2.0.0-r
	-rm -rf  $(TOOLS_BASE)/local/src/BitOS.2/newlib-2.0.0-r
	cp $(BITOS_PATH)/libbitmachine/libc-bitos.a $(TOOLS_BASE)/local/sh-elf/sh-elf/lib/m2e/libc.a
	$(STRIP) $(TOOLS_BASE)/local/bin/* 
	mkdir $(TOOLS_BASE)/local/home
	cp $(BITOS_PATH)/hello.c $(TOOLS_BASE)/local/home
	-rm $(TOOLS_BASE)/local.zip
	cd $(TOOLS_BASE)/; zip -r local.zip local
	cp $(TOOLS_BASE)/local.zip ~/Google\ Drive/BitFS
	@#cp $(TOOLS_BASE)/local.zip ~/Google\ Drive/Projects/BitMachine/Web/BitFS

full: gdrive web relink-tools local.zip