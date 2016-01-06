cd ~/Projects/bitos-build/
rm -rf readline-6.3
mkdir readline-6.3
cd readline-6.3
../src/readline-6.3/configure --prefix="/usr/local/sh-elf" --host=sh-elf CFLAGS="-Os -D_POSIX_VERSION -m2e -pie " bash_cv_wcwidth_broken=no  --disable-shared -disable-multibyte --disable-largefile
