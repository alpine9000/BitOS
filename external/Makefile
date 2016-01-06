include common.mk

all:
	make -f wget.mk
	make -f gcc-5.3.0.osx.mk
	make -f binutils.cross.mk
	make -f gcc-5.3.0.cross-stage1.mk
	make -f newlib-2.0.0.mk
	make -f gcc-5.3.0.cross-stage2.mk
	make -C $(BITOS_DIR) clean
	make -C $(BITOS_DIR) elf
	ln -sf $(BITOS_DIR)/libbitmachine/crt1.o /usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/m2e/crt1.o
	ln -sf $(BITOS_DIR)/libbitmachine/crtbegin.o /usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/m2e/crtbegin.o 
	ln -sf $(BITOS_DIR)/libbitmachine/crtend.o /usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/m2e/crtend.o
	ln -sf $(BITOS_DIR)/libbitmachine/crti.o /usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/m2e/crti.o 
	ln -sf $(BITOS_DIR)/libbitmachine/crtn.o /usr/local/sh-elf/lib/gcc/sh-elf/5.3.0/m2e/crtn.o
	make -f readline-6.3.mk
	make -C $(BITOS_DIR)
	make -f gcc-5.3.0.bitos.mk
	make -f binutils.bitos.mk
	make -f make-4.1.mk


diff-make:
	rm -f $(SRC_BASE)/make-4.1/*~
	cd $(SRC_BASE) && \
	diff -Naur make-4.1.orig make-4.1  > $(BITOS_DIR)/external/patches/make-4.1.patch; \
	ls -l $(BITOS_DIR)/external/patches/make-4.1.patch

diff-gcc:
	cd $(SRC_BASE)/gcc-5.3.0 && \
	svn diff > $(BITOS_DIR)/external/patches/gcc-5.3.0.patch
	ls -l $(BITOS_DIR)/external/patches/gcc-5.3.0.patch

diff-readline:
	rm -f $(SRC_BASE)/readline-6.3/*~
	cd $(SRC_BASE) && \
	diff -Naur readline-6.3.orig readline-6.3  > $(BITOS_DIR)/external/patches/readline-6.3.patch; \
	ls -l $(BITOS_DIR)/external/patches/readline-6.3.patch

