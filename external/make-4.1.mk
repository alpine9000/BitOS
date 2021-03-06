include common.mk
PATCH_FILE=$(BITOS_DIR)/external/patches/make-4.1.patch
MAKE_DIST=$(TOOLS_BASE)/src/make-4.1.tar.gz
MAKE_SRC=$(SRC_BASE)/make-4.1
MAKE_BUILD_DIR=$(TOOLS_BASE)/make-4.1
MAKE_BIN=$(MAKE_BUILD_DIR)/make
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_DIST):
	wget ftp://ftp.gnu.org/gnu/make/make-4.1.tar.gz -O $(MAKE_DIST)	

$(MAKE_SRC): $(MAKE_DIST) $(PATCH_FILE)
	-rm -rf $(MAKE_SRC)
	tar zxfv  $(MAKE_DIST) -C $(SRC_BASE)
	patch -p1 -d $(MAKE_SRC) < $(PATCH_FILE)


$(MAKE_MAKEFILE): $(MAKE_SRC)
	rm -rf $(MAKE_BUILD_DIR)
	mkdir $(MAKE_BUILD_DIR)
	cd $(MAKE_BUILD_DIR) && \
	../src/make-4.1/configure --host=sh-elf --prefix=/usr/local/sh-elf/ CFLAGS="-g -m2e -pie -D__BITOS__"

$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -C $(MAKE_BUILD_DIR)