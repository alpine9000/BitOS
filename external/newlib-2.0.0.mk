include common.mk
PATH := $(PATH):/usr/local/sh-elf/bin
SRC=$(BITOS_DIR)/newlib-2.0.0-r
BUILD_DIR=$(TOOLS_BASE)/newlib-2.0.0-r
BIN=$(BUILD_DIR)/sh-elf/m2e/newlib/libc/libc.a
MAKE_MAKEFILE=$(BUILD_DIR)/Makefile

all: $(BIN)

$(MAKE_MAKEFILE): 
	rm -rf $(BUILD_DIR)
	$(BITOS_DIR)/external/configure.scripts/newlib-2.0.0.sh


$(BIN): $(MAKE_MAKEFILE)
	make -C $(BUILD_DIR)
	make -C $(BUILD_DIR) install
	cd /usr/local/sh-elf/sh-elf/lib/m2e &&\
	mv libc.a libc.a.newlib &&\
	ln -sf ~/Projects/BitOS/libbitmachine/libc-bitos.a libc.a &&\
	cp ~/Projects/BitOS/newlib-override/sys/* /usr/local/sh-elf/sh-elf/include/sys/ &&\
	cd ~/Projects/BitOS/libbitmachine/libc &&\
	rm *.o && \
	sh-elf-ar -x /usr/local/sh-elf/sh-elf/lib/m2e/libc.a.newlib
