include common.mk
MAKE_DIST=$(TOOLS_BASE)/src/wget-1.16.tar.xz
MAKE_SRC=$(SRC_BASE)/wget-1.16
MAKE_BUILD_DIR=$(TOOLS_BASE)/wget
MAKE_BIN=$(MAKE_BUILD_DIR)/wget
MAKE_MAKEFILE=$(MAKE_BUILD_DIR)/Makefile

all: $(MAKE_BIN)

$(MAKE_DIST):
	ftp -o $(MAKE_DIST) ftp://ftp.gnu.org/gnu/wget/wget-1.16.tar.xz 

$(MAKE_SRC): $(MAKE_DIST) $(PATCH_FILE)
	-rm -rf $(MAKE_SRC)
	tar zxfv  $(MAKE_DIST) -C $(SRC_BASE)


$(MAKE_MAKEFILE): $(MAKE_SRC)
	$(BITOS_DIR)/external/configure.scripts/wget.sh


$(MAKE_BIN): $(MAKE_MAKEFILE)
	make -C $(MAKE_BUILD_DIR)
	make -C $(MAKE_BUILD_DIR) install