cd ~/Projects/bitos-build/
rm -rf make-4.1
mkdir make-4.1
cd make-4.1
../src/make-4.1/configure --host=sh-elf --prefix=/usr/local/sh-elf/ CFLAGS="-g -m2e -pie -D__BITOS__"
