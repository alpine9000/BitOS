rm -f ~/Projects/bitos-build/binutils.bitos
mkdir ~/Projects/bitos-build/binutils.bitos
cd ~/Projects/bitos-build/binutils.bitos
../src/binutils-2.25/configure --host=sh-elf --target=sh-elf --prefix=/usr/local --disable-nls --disable-nls --disable-werror CFLAGS="-O2 -m2e -pie"