rm -f ~/Projects/bitos-build/binutils
mkdir ~/Projects/bitos-build/binutils
cd ~/Projects/bitos-build/binutils
../src/binutils-2.25/configure --target=sh-elf --prefix=/usr/local/sh-elf --disable-nls --disable-werror
