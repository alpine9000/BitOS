include common.mk
PATCH_FILE=$(BITOS_DIR)/external/patches/gcc-5.3.0.patch
MAKE_DIST=$(TOOLS_BASE)/src/gcc-5.3.0.tar.gz
MAKE_SRC=$(SRC_BASE)/gcc-5.3.0
MAKE_BUILD_DIR=$(TOOLS_BASE)/gcc-5.3.0
MAKE_BIN=/usr/local/sh-elf/bin/sh-elf-gcc
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_SRC): $(PATCH_FILE)
	#-rm -rf $(MAKE_SRC)
	svn checkout svn://gcc.gnu.org/svn/gcc/tags/gcc_5_3_0_release $(MAKE_SRC)
	patch -p0 -d $(MAKE_SRC) < $(PATCH_FILE)
	cd $(MAKE_SRC); ./contrib/download_prerequisites


$(MAKE_MAKEFILE): $(MAKE_SRC)
	-mkdir $(MAKE_BUILD_DIR)
	cd $(MAKE_BUILD_DIR) && \
	../src/gcc-5.3.0/configure --target=sh-elf --prefix=/usr/local/sh-elf/ --with-gnu-as --with-gnu-ld --with-newlib --with-multilib-list=m2e --without-headers --enable-languages=c,c++ --disable-nls --disable-libssp --disable-libada --disable-libquadmath --disable-libgomp --disable-libstdcxx

$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -j4 -C $(MAKE_BUILD_DIR)
	make -C $(MAKE_BUILD_DIR) install