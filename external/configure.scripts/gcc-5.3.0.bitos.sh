rm -rf ~/Projects/bitos-build/gcc-5.3.0.bitos
mkdir ~/Projects/bitos-build/gcc-5.3.0.bitos
cd ~/Projects/bitos-build/gcc-5.3.0.bitos
../src/gcc-5.3.0/configure --host=sh-elf --target=sh-elf --prefix=/usr/local/sh-elf/ --with-gnu-as --with-gnu-ld --with-newlib --with-multilib-list=m2e --without-headers --enable-languages=c,c++ --disable-nls --disable-libssp --disable-libada --disable-libquadmath --disable-libgomp CFLAGS="-D__BITOS__ -O2 -g -m2e -pie" CXXFLAGS="-D__BITOS__ -O2 -m2e -pie -fpermissive -g" --disable-libstdcxx --enable-checking=release