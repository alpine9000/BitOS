OBJ_FILES=ftc.o
CFLAGS=-O3 -funit-at-a-time -Wall -Werror -std=gnu99 -m2e -I../../libbitmachine -I/usr/local/sh-elf/include/freetype2/
ELF_PATH =/usr/local/sh-elf/bin/
LIB_PATH=/usr/local/sh-elf/
CC=$(ELF_PATH)sh-elf-gcc
CPP=$(ELF_PATH)sh-elf-g++
LIBS=-lfreetype
LDFLAGS=-L../../libbitmachine -L/usr/local/sh-elf/lib
ELF_FILE=a.elf

all: $(ELF_FILE)

%.o: %.s
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@
	$(CC) -MM $(CPPFLAGS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp


.SECONDARY:
$(ELF_FILE): $(OBJ_FILES)
	$(CC) -m2e $(LDFLAGS) -Wl,-Map -Wl,a.map -pie -o $(ELF_FILE) $(OBJ_FILES) $(LIBS) 
	cp $(ELF_FILE) ~/Google\ Drive/BitFS/bin/ft

clean:
	rm *.o *.elf

-include $(OBJ_FILES:%.o=%.d) 