PATH := $(PATH):/usr/local/sh-elf/bin
BITOS_PATH=~/Projects/BitOS
TOOLS_BASE=~/Projects/bitos-build
WARNINGS = -pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
include ./makefiles/optimize.mk
-include ./makefiles/gitversion.mk
CPP_WARNINGS = $(WARNINGS) -Wno-char-subscripts 
CFLAGS=$(VERSION) -g $(OPTIMIZE) $(WARNINGS) -D_KERNEL_BUILD  -I/usr/local/sh-elf/include -I. -I./libbitmachine -m2e -falign-jumps
CPPFLAGS = -D_KERNEL_BUILD $(OPTIMIZE) $(CPP_WARNINGS)  -m2e -falign-jumps  -I./libbitmachine -fno-rtti -fno-exceptions
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

install:
	cp apps/bsh/bsh /usr/local/bin
	cp apps/bemacs/bemacs /usr/local/bin
	cp apps/si/si /usr/local/bin


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

include ./makefiles/base.mk

touch:
	touch $(IMG_OBJ) $(SOUND_OBJ) $(WOLF_OBJ)
	$(MAKE) -C libbitmachine touch
	$(MAKE) -C apps/wolf touch
	$(MAKE) -C apps/bemacs touch
	$(MAKE) -C apps/bsh touch
	$(MAKE) -C apps/si touch

clean:
	-rm -f $(BIN_FILE) libwolf.a  *~ $(ELF_FILE) $(ALL_OBJS) $(ALL_OBJS:%.o=%.d)
	rm -f $(IMG_OBJ) $(SOUND_OBJ) $(WOLF_OBJ)
	$(MAKE) -C libbitmachine clean
	$(MAKE) -C apps/wolf clean
	$(MAKE) -C apps/bemacs clean
	$(MAKE) -C apps/bsh clean
	$(MAKE) -C apps/si clean

backup:
	git -C ~/Google\ Drive/Projects/BitOS pull

.SECONDARY:
libwolf.a: $(WOLF_MEDIA)
	$(AR) rcs libwolf.a $(WOLF_MEDIA)


.SECONDARY:
$(ELF_FILE): $(OBJ_FILES) libwolf.a libbitmachine/libc-kernel.a libbitmachine/libbitmachine.a 
	$(LD) $(LDFLAGS) -T start.l -no-keep-memory -Map bin/bitos.map -o $(ELF_FILE) $(OBJ_FILES) $(LIBS) 

strip:
	$(STRIP) $(ELF_FILE)

$(BIN_FILE): $(ELF_FILE)
	sh-elf-objcopy --remove-section=.bss --remove-section=.peripheral -v -O binary $(ELF_FILE) $(BIN_FILE)


-include $(ALL_OBJS:%.o=%.d) 


FS_BASE=$(TOOLS_BASE)/filesystem
GCC_BASE=$(TOOLS_BASE)/gcc-5.3.0.bitos
MAKE_BASE=$(TOOLS_BASE)/make-4.1
BINUTILS_BASE = $(TOOLS_BASE)/binutils.bitos
GCC_LIBEXEC=$(FS_BASE)/usr/local/sh-elf/libexec/gcc/sh-elf/5.3.0/
fs: filesystem

filesystem:

	-rm -rf $(FS_BASE)
	-rm $(TOOLS_BASE)/filesystem.zip
	mkdir $(FS_BASE)

	-rm $(BINUTILS_BASE)/gas/as-new $(BINUTILS_BASE)/ld/ld-new $(BINUTILS_BASE)/binutils/ar $(BINUTILS_BASE)/binutils/strip-new
	make -C $(BINUTILS_BASE) all-gas all-ld
	make -C $(BINUTILS_BASE)/binutils ar
	make -C $(BINUTILS_BASE)/binutils strip-new
	-mkdir $(FS_BASE)/usr
	-mkdir $(FS_BASE)/usr/local
	-mkdir $(FS_BASE)/usr/local/bin
	-mkdir $(FS_BASE)/bin
	-mkdir $(FS_BASE)/usr/local/home
	-mkdir $(FS_BASE)/usr/local/src

	cp $(MAKE_BASE)/make $(FS_BASE)/usr/local/bin
	cp $(BITOS_PATH)/apps/bemacs/bemacs $(FS_BASE)/usr/local/bin	
	cp $(BITOS_PATH)/apps/bsh/bsh $(FS_BASE)/usr/local/bin
	cp $(BITOS_PATH)/apps/si/si $(FS_BASE)/usr/local/bin
	cp $(BITOS_PATH)/bin/bitos.elf $(FS_BASE)/usr/local/bin
	cp -r /usr/local/sh-elf $(FS_BASE)/usr/local
	rm -r $(FS_BASE)/usr/local/sh-elf/share
	rm -r $(FS_BASE)/usr/local/sh-elf/bin
	mkdir $(FS_BASE)/usr/local/sh-elf/bin

	rm -r $(GCC_LIBEXEC)/*
	cp $(BINUTILS_BASE)/gas/as-new $(FS_BASE)/usr/local/bin/as
	cp $(GCC_BASE)/gcc/xgcc $(FS_BASE)/usr/local/sh-elf/bin/sh-elf-gcc
	cp $(GCC_BASE)/gcc/xg++ $(FS_BASE)/usr/local/sh-elf/bin/sh-elf-g++
	cp $(BINUTILS_BASE)/ld/ld-new  $(FS_BASE)/usr/local/sh-elf/bin/sh-elf-ld
	cp $(BINUTILS_BASE)/binutils/ar $(FS_BASE)/usr/local/sh-elf/bin/sh-elf-ar
	cp $(BINUTILS_BASE)/binutils/strip-new  $(FS_BASE)/usr/local/sh-elf/bin/sh-elf-strip
	cp $(GCC_BASE)/gcc/lto-wrapper $(GCC_BASE)/gcc/cc1 $(GCC_BASE)/gcc/cc1plus $(GCC_LIBEXEC)

	rm -r $(FS_BASE)/usr/local/sh-elf/sh-elf/bin
	rm -r $(FS_BASE)/usr/local/sh-elf/sh-elf/lib/ml
	rm -r $(FS_BASE)/usr/local/sh-elf/sh-elf/lib/*.*
	rm -r $(FS_BASE)/usr/local/sh-elf/lib/libcc1.*
	rm -r $(FS_BASE)/usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/ml
	rm -r $(FS_BASE)/usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/plugin
	rm -r $(FS_BASE)/usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/*.o
	rm -r $(FS_BASE)/usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/*.a

	cp -r $(BITOS_PATH) $(FS_BASE)/usr/local/src/BitOS
	rm -rf $(FS_BASE)/usr/local/src/BitOS/.git
	rm -f $(FS_BASE)/usr/local/src/BitOS/makefiles/gitversion.mk
	rm -f $(FS_BASE)/usr/local/src/BitOS/makefiles/base.mk
	cp -r $(BITOS_PATH)/makefiles/simple.mk $(FS_BASE)/usr/local/src/BitOS/makefiles/base.mk
	-rm -rf  $(FS_BASE)/usr/local/src/BitOS/newlib-2.0.0-r
	cp $(BITOS_PATH)/libbitmachine/libc-bitos.a $(FS_BASE)/usr/local/sh-elf/sh-elf/lib/m2e/libc.a
	mv $(FS_BASE)/usr/local/sh-elf/sh-elf/include/c++  $(FS_BASE)/usr/local/sh-elf/include


	-rm `find $(FS_BASE)/usr/local/src -name "*~"`
	$(STRIP) $(FS_BASE)/usr/local/bin/* 
	$(STRIP) $(GCC_LIBEXEC)/cc1
	$(STRIP) $(GCC_LIBEXEC)/cc1plus
	$(STRIP) $(FS_BASE)/usr/local/sh-elf/bin/* 


	cp $(BITOS_PATH)/links/sh-elf-ar $(FS_BASE)/usr/local/bin/ar
	cp $(BITOS_PATH)/links/sh-elf-ld $(FS_BASE)/usr/local/bin/ld
	cp $(BITOS_PATH)/links/bsh $(FS_BASE)/bin/sh
	cp $(BITOS_PATH)/links/bsh $(FS_BASE)/usr/local/bin/sh
	cp $(BITOS_PATH)/links/bsh $(FS_BASE)/usr/local/bin/cp
	cp $(BITOS_PATH)/links/bsh $(FS_BASE)/usr/local/bin/rm
	cp $(BITOS_PATH)/links/bsh $(FS_BASE)/usr/local/bin/touch

	cp $(BITOS_PATH)/apps/hello/* $(FS_BASE)/usr/local/home/
	-rm $(TOOLS_BASE)/filesystem.zip
	cd $(TOOLS_BASE)/filesystem; zip -r $(TOOLS_BASE)/filesystem.zip *


localfs: fs
	cp $(TOOLS_BASE)/filesystem.zip ~/Projects/BitMachine/BitFS

gdrivefs: fs
	cp $(TOOLS_BASE)/filesystem.zip ~/Google\ Drive/BitFS


full: gdrive web filesystem