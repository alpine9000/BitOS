include common.mk
PATCH_FILE=$(BITOS_DIR)/external/patches/gcc-5.3.0.patch
MAKE_DIST=$(TOOLS_BASE)/src/gcc-5.3.0.tar.gz
MAKE_SRC=$(SRC_BASE)/gcc-5.3.0
MAKE_BUILD_DIR=$(TOOLS_BASE)/gcc-5.3.0.osx
MAKE_BIN=/usr/local/bin/gcc
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_SRC): $(PATCH_FILE)
	#-rm -rf $(MAKE_SRC)
	svn checkout svn://gcc.gnu.org/svn/gcc/tags/gcc_5_3_0_release $(MAKE_SRC)
	patch -p0 -d $(MAKE_SRC) < $(PATCH_FILE)
	cd $(MAKE_SRC); ./contrib/download_prerequisites


$(MAKE_MAKEFILE): $(MAKE_SRC)
	mkdir $(MAKE_BUILD_DIR)
	cd $(MAKE_BUILD_DIR) && \
	../src/gcc-5.3.0/configure --prefix=/usr/local --enable-languages=c,c++


$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -j4 -C $(MAKE_BUILD_DIR)
	make -C $(MAKE_BUILD_DIR) install