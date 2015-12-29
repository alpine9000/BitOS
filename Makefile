BITOS_PATH=~/Projects/BitOS
TOOLS_BASE=~/Projects/bitos-build
WARNINGS = -pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
include ./optimize.mk
-include ./gitversion.mk
CPP_WARNINGS = $(WARNINGS) -Wno-char-subscripts 
CFLAGS=$(VERSION) -g $(OPTIMIZE) $(WARNINGS) -D_KERNEL_BUILD  -I/usr/local/sh-elf/include -I. -I./libbitmachine -m2e  -funit-at-a-time -falign-jumps -fdelete-null-pointer-checks
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

elf: $(LIB_BITMACHINE) $(ELF_FILE) 

gdrive: all
	cp $(ELF_FILE) ~/Google\ Drive/BitFS/bin
	make  -C $(BSH) gdrive
	make  -C $(BEMACS) gdrive
	make  -C $(SI) gdrive

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

include base.mk

touch:
	touch $(IMG_OBJ) $(SOUND_OBJ) $(WOLF_OBJ)
	$(MAKE) -C libbitmachine touch
	$(MAKE) -C apps/wolf touch
	$(MAKE) -C apps/bemacs touch
	$(MAKE) -C apps/bsh touch
	$(MAKE) -C apps/si touch

clean:
	-rm -f $(BIN_FILE) libwolf.a bitos.sym bitos.map *~ *.rgba y.* lex.yy.? y.tab.? $(ELF_FILE) $(ALL_OBJS) $(ALL_OBJS:%.o=%.d)
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
MAKE_BASE=$(TOOLS_BASE)/make-4.1
GCC_LIBEXEC=$(TOOLS_BASE)/local/sh-elf/libexec/gcc/sh-elf/5.3.0/
BINUTILS_BASE = $(TOOLS_BASE)/binutils.bitos
local.zip:

	-rm -rf $(TOOLS_BASE)/local

	-rm $(BINUTILS_BASE)/gas/as-new $(BINUTILS_BASE)/ld/ld-new $(BINUTILS_BASE)/binutils/ar
	make -C $(BINUTILS_BASE) all-gas all-ld
	make -C $(BINUTILS_BASE)/binutils ar
	-mkdir $(TOOLS_BASE)/local/
	-mkdir $(TOOLS_BASE)/local/bin
	cp $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus $(TOOLS_BASE)/local/bin
	cp $(MAKE_BASE)/make $(TOOLS_BASE)/local/bin
	cp $(BINUTILS_BASE)/binutils/ar $(TOOLS_BASE)/local/bin
	cp $(BINUTILS_BASE)/gas/as-new  $(TOOLS_BASE)/local/bin/as
	cp $(BINUTILS_BASE)/ld/ld-new  $(TOOLS_BASE)/local/bin/ld
	cp $(BITOS_PATH)/apps/bemacs/bemacs $(TOOLS_BASE)/local/bin	
	cp $(BITOS_PATH)/apps/bsh/bsh $(TOOLS_BASE)/local/bin
	cp $(BITOS_PATH)/apps/bsh/bsh $(TOOLS_BASE)/local/bin/cp
	cp $(BITOS_PATH)/apps/bsh/bsh $(TOOLS_BASE)/local/bin/rm
	cp $(BITOS_PATH)/apps/bsh/bsh $(TOOLS_BASE)/local/bin/touch
	cp $(BITOS_PATH)/apps/si/si $(TOOLS_BASE)/local/bin
	cp $(BITOS_PATH)/bin/bitos.elf $(TOOLS_BASE)/local/bin
	cp -r /usr/local/sh-elf $(TOOLS_BASE)/local
	rm -r $(TOOLS_BASE)/local/sh-elf/share
	rm -r $(TOOLS_BASE)/local/sh-elf/bin
	mkdir $(TOOLS_BASE)/local/sh-elf/bin

	rm -r $(GCC_LIBEXEC)/*
	cp $(GCC_BASE)/gcc/xgcc $(TOOLS_BASE)/local/sh-elf/bin/sh-elf-gcc
	cp $(GCC_BASE)/gcc/xg++ $(TOOLS_BASE)/local/sh-elf/bin/sh-elf-g++
	cp $(TOOLS_BASE)/local/bin/ld $(TOOLS_BASE)/local/sh-elf/bin/sh-elf-ld
	cp $(BINUTILS_BASE)/binutils/ar $(TOOLS_BASE)/local/sh-elf/bin/sh-elf-ar
	cp $(GCC_BASE)/gcc/lto-wrapper $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus $(GCC_LIBEXEC)

	rm -r $(TOOLS_BASE)/local/sh-elf/sh-elf/bin
	rm -r $(TOOLS_BASE)/local/sh-elf/sh-elf/lib/ml
	rm -r $(TOOLS_BASE)/local/sh-elf/sh-elf/lib/*.*
	-rm -rf $(TOOLS_BASE)/local/src/BitOS
	-mkdir $(TOOLS_BASE)/local/src
	cp -r $(BITOS_PATH) $(TOOLS_BASE)/local/src/BitOS
	cp -r $(BITOS_PATH) $(TOOLS_BASE)/local/src/BitOS.2/
	rm -rf $(TOOLS_BASE)/local/src/BitOS/.git
	rm -rf $(TOOLS_BASE)/local/src/BitOS.2/.git
	-rm -rf  $(TOOLS_BASE)/local/src/BitOS/newlib-2.0.0-r
	-rm -rf  $(TOOLS_BASE)/local/src/BitOS.2/newlib-2.0.0-r
	cp $(BITOS_PATH)/libbitmachine/libc-bitos.a $(TOOLS_BASE)/local/sh-elf/sh-elf/lib/m2e/libc.a
	mv $(TOOLS_BASE)/local/sh-elf/sh-elf/include/c++  $(TOOLS_BASE)/local/sh-elf/include

	$(STRIP) $(TOOLS_BASE)/local/bin/* 
	$(STRIP) $(GCC_LIBEXEC)/cc1
	$(STRIP) $(GCC_LIBEXEC)/cc1plus
	$(STRIP) $(TOOLS_BASE)/local/sh-elf/bin/* 

	mkdir $(TOOLS_BASE)/local/home
	cp $(BITOS_PATH)/hello.c $(TOOLS_BASE)/local/home
	cp $(BITOS_PATH)/Makefile.hello $(TOOLS_BASE)/local/home/Makefile
	-rm $(TOOLS_BASE)/local.zip
	cd $(TOOLS_BASE)/; zip -r local.zip local
	cp $(TOOLS_BASE)/local.zip ~/Google\ Drive/BitFS
	@#cp $(TOOLS_BASE)/local.zip ~/Google\ Drive/Projects/BitMachine/Web/BitFS

full: gdrive web relink-tools local.zip