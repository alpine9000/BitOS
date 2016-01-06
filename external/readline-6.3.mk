include common.mk
PATCH_FILE=$(BITOS_DIR)/external/patches/readline-6.3.patch
MAKE_DIST=$(TOOLS_BASE)/src/readline-6.3.tar.gz
MAKE_SRC=$(SRC_BASE)/readline-6.3
MAKE_BUILD_DIR=$(TOOLS_BASE)/readline-6.3
MAKE_BIN=/usr/local/sh-elf/lib/libreadline.a
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_DIST):
	wget ftp://ftp.cwru.edu/pub/bash/readline-6.3.tar.gz -O $(MAKE_DIST)

$(MAKE_SRC): $(MAKE_DIST) $(PATCH_FILE)
	-rm -rf $(MAKE_SRC)
	tar zxfv  $(MAKE_DIST) -C $(SRC_BASE)
	patch -p1 -d $(MAKE_SRC) < $(PATCH_FILE)


$(MAKE_MAKEFILE): $(MAKE_SRC)
	rm -rf $(MAKE_BUILD_DIR)
	mkdir $(MAKE_BUILD_DIR)
	cd $(MAKE_BUILD_DIR) && \
	../src/readline-6.3/configure --prefix="/usr/local/sh-elf" --host=sh-elf CFLAGS="-Os -D_POSIX_VERSION -m2e -pie " bash_cv_wcwidth_broken=no  --disable-shared -disable-multibyte --disable-largefile


$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -C $(MAKE_BUILD_DIR) install