include common.mk
MAKE_DIST=$(TOOLS_BASE)/src/binutils-2.25.tar.gz
MAKE_SRC=$(SRC_BASE)/binutils-2.25
MAKE_BUILD_DIR=$(TOOLS_BASE)/binutils.bitos
MAKE_BIN=$(MAKE_BUILD_DIR)/gas/as-new
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_DIST):
	wget http://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.gz -O $(MAKE_DIST)	

$(MAKE_SRC): $(MAKE_DIST)
	-rm -rf $(MAKE_SRC)
	tar zxfv  $(MAKE_DIST) -C $(SRC_BASE)


$(MAKE_MAKEFILE): $(MAKE_SRC)
	rm -f $(MAKE_BUILD_DIR)
	mkdir $(MAKE_BUILD_DIR)
	cd $(MAKE_BUILD_DIR) && \
	../src/binutils-2.25/configure --host=sh-elf --target=sh-elf --prefix=/usr/local --disable-nls --disable-nls --disable-werror CFLAGS="-O2 -m2e -pie"

$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -C $(MAKE_BUILD_DIR)