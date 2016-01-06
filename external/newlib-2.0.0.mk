include common.mk
PATH := $(PATH):/usr/local/sh-elf/bin
SRC=$(BITOS_DIR)/newlib-2.0.0-r
BUILD_DIR=$(TOOLS_BASE)/newlib-2.0.0-r
BIN=/usr/local/sh-elf/sh-elf/lib/m2e/libc.a.newlib
MAKE_MAKEFILE=$(BUILD_DIR)/Makefile

all: $(BIN)

$(MAKE_MAKEFILE): 
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	cd $(BUILD_DIR) && \
	../../BitOS/newlib-2.0.0-r/configure --prefix=/usr/local/sh-elf --target=sh-elf --enable-newlib-multithread CFLAGS=-DMALLOC_PROVIDED target_alias=sh-elf


$(BIN): $(MAKE_MAKEFILE)
	make -C $(BUILD_DIR)
	make -C $(BUILD_DIR) install
	cd /usr/local/sh-elf/sh-elf/lib/m2e &&\
	mv libc.a libc.a.newlib &&\
	ln -sf $(BITOS_DIR)/libbitmachine/libc-bitos.a libc.a &&\
	cp $(BITOS_DIR)/newlib-override/sys/* /usr/local/sh-elf/sh-elf/include/sys/ &&\
	cd $(BITOS_DIR)/libbitmachine/libc &&\
	rm -f *.o && \
	sh-elf-ar -x /usr/local/sh-elf/sh-elf/lib/m2e/libc.a.newlib
