mkdir ~/Projects/bitos-build/newlib-2.0.0-r
cd ~/Projects/bitos-build/newlib-2.0.0-r
../../BitOS/newlib-2.0.0-r/configure --prefix=/usr/local/sh-elf --target=sh-elf --enable-newlib-multithread CFLAGS=-DMALLOC_PROVIDED target_alias=sh-elf
