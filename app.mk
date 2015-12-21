WARNINGS = -pedantic-errors -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wshadow
CPP_WARNINGS = $(WARNINGS) -Wno-char-subscripts 
include ../../optimize.mk
CFLAGS=$(OPTIMIZE) -g $(WARNINGS) -m2e -I../../libbitmachine  -I/usr/local/sh-elf/include
CPPFLAGS=$(OPTIMIZE) -g $(CPP_WARNINGS) -m2e -I../../libbitmachine -I/usr/local/sh-elf/include
LIB_PATH=/usr/local/sh-elf/
LDFLAGS=-L../../libbitmachine -L/usr/local/sh-elf/lib

all: $(ELF_FILE)

include ../../base.mk

.SECONDARY:
%.rgba: %.png
	convert $*.png $*.rgba

.SECONDARY:
%.c: %.rgba
	../../bin2c $*.rgba $*.c

.SECONDARY:
%.c: %.wav
	../../bin2c $*.wav $*.c


.SECONDARY:
$(ELF_FILE): $(OBJ_FILES) ../../libbitmachine/libc-bitos.a
	$(CC) -m2e $(LDFLAGS) -Wl,-Map -Wl,a.map -pie -o $(ELF_FILE) $(OBJ_FILES) $(LIBS) 
	cp $(ELF_FILE) ~/Google\ Drive/BitFS/bin/

clean:
	-rm *.o $(ELF_FILE)

-include $(OBJ_FILES:%.o=%.d) 