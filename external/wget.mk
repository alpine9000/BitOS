include common.mk
MAKE_DIST=$(TOOLS_BASE)/src/wget-1.16.tar.xz
MAKE_SRC=$(SRC_BASE)/wget-1.16/
MAKE_BUILD_DIR=$(TOOLS_BASE)/wget
MAKE_BIN=/usr/local/bin/wget
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_DIST):
	ftp -o $(MAKE_DIST) ftp://ftp.gnu.org/gnu/wget/wget-1.16.tar.xz 

$(MAKE_SRC): $(MAKE_DIST)
	-rm -rf $(MAKE_SRC)
	tar zxfv  $(MAKE_DIST) -C $(SRC_BASE)
	touch $(MAKE_SRC)


$(MAKE_MAKEFILE): $(MAKE_SRC)
	rm -rf $(MAKE_BUILD_DIR)
	mkdir $(MAKE_BUILD_DIR)
	cd $(MAKE_BUILD_DIR) && ../src/wget-1.16/configure --without-ssl


$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -C $(MAKE_BUILD_DIR)
	make -C $(MAKE_BUILD_DIR) install