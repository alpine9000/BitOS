# 1 "sh4a-bitmovua.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "sh4a-bitmovua.c"







struct s0 { long long d : 32; } x0;
long long f0() {
  return x0.d;
}


struct s1 { long long c : 8; long long d : 32; } x1;
long long f1() {
  return x1.d;
}


struct s2 { long long c : 16; long long d : 32; } x2;
long long f2() {
  return x2.d;
}


struct s3 { long long c : 24; long long d : 32; } x3;
long long f3() {
  return x3.d;
}


struct s4 { long long c : 32; long long d : 32; } x4;
long long f4() {
  return x4.d;
}


struct u0 { unsigned long long d : 32; } y_0;
unsigned long long g0() {
  return y_0.d;
}


struct u1 { long long c : 8; unsigned long long d : 32; } y_1;
unsigned long long g1() {
  return y_1.d;
}


struct u2 { long long c : 16; unsigned long long d : 32; } y2;
unsigned long long g2() {
  return y2.d;
}


struct u3 { long long c : 24; unsigned long long d : 32; } y3;
unsigned long long g3() {
  return y3.d;
}


struct u4 { long long c : 32; unsigned long long d : 32; } y4;
unsigned long long g4() {
  return y4.d;
}

# 1 "/usr/local/sh-elf/sh-elf/include/assert.h" 1 3
# 9 "/usr/local/sh-elf/sh-elf/include/assert.h" 3
# 1 "/usr/local/sh-elf/sh-elf/include/_ansi.h" 1 3
# 15 "/usr/local/sh-elf/sh-elf/include/_ansi.h" 3
# 1 "/usr/local/sh-elf/sh-elf/include/newlib.h" 1 3
# 16 "/usr/local/sh-elf/sh-elf/include/_ansi.h" 2 3
# 1 "/usr/local/sh-elf/sh-elf/include/sys/config.h" 1 3



# 1 "/usr/local/sh-elf/sh-elf/include/machine/ieeefp.h" 1 3
# 5 "/usr/local/sh-elf/sh-elf/include/sys/config.h" 2 3
# 1 "/usr/local/sh-elf/sh-elf/include/sys/features.h" 1 3
# 6 "/usr/local/sh-elf/sh-elf/include/sys/config.h" 2 3
# 17 "/usr/local/sh-elf/sh-elf/include/_ansi.h" 2 3
# 10 "/usr/local/sh-elf/sh-elf/include/assert.h" 2 3
# 39 "/usr/local/sh-elf/sh-elf/include/assert.h" 3

# 39 "/usr/local/sh-elf/sh-elf/include/assert.h" 3
void __assert (const char *, int, const char *) __attribute__ ((__noreturn__))
                                 ;
void __assert_func (const char *, int, const char *, const char *) __attribute__ ((__noreturn__))
                                 ;
# 68 "sh4a-bitmovua.c" 2


# 69 "sh4a-bitmovua.c"
int
main (void)
{
  x1.d = 0x12345678;
  
# 73 "sh4a-bitmovua.c" 3
 ((
# 73 "sh4a-bitmovua.c"
 f1 () == 0x12345678
# 73 "sh4a-bitmovua.c" 3
 ) ? (void)0 : __assert_func ("sh4a-bitmovua.c", 73, __func__, 
# 73 "sh4a-bitmovua.c"
 "f1 () == 0x12345678"
# 73 "sh4a-bitmovua.c" 3
 ))
# 73 "sh4a-bitmovua.c"
                             ;

  x2.d = 0x12345678;
  
# 76 "sh4a-bitmovua.c" 3
 ((
# 76 "sh4a-bitmovua.c"
 f2 () == 0x12345678
# 76 "sh4a-bitmovua.c" 3
 ) ? (void)0 : __assert_func ("sh4a-bitmovua.c", 76, __func__, 
# 76 "sh4a-bitmovua.c"
 "f2 () == 0x12345678"
# 76 "sh4a-bitmovua.c" 3
 ))
# 76 "sh4a-bitmovua.c"
                             ;

  x3.d = 0x12345678;
  
# 79 "sh4a-bitmovua.c" 3
 ((
# 79 "sh4a-bitmovua.c"
 f3 () == 0x12345678
# 79 "sh4a-bitmovua.c" 3
 ) ? (void)0 : __assert_func ("sh4a-bitmovua.c", 79, __func__, 
# 79 "sh4a-bitmovua.c"
 "f3 () == 0x12345678"
# 79 "sh4a-bitmovua.c" 3
 ))
# 79 "sh4a-bitmovua.c"
                             ;

  y_1.d = 0x12345678;
  
# 82 "sh4a-bitmovua.c" 3
 ((
# 82 "sh4a-bitmovua.c"
 g1 () == 0x12345678
# 82 "sh4a-bitmovua.c" 3
 ) ? (void)0 : __assert_func ("sh4a-bitmovua.c", 82, __func__, 
# 82 "sh4a-bitmovua.c"
 "g1 () == 0x12345678"
# 82 "sh4a-bitmovua.c" 3
 ))
# 82 "sh4a-bitmovua.c"
                             ;

  y2.d = 0x12345678;
  
# 85 "sh4a-bitmovua.c" 3
 ((
# 85 "sh4a-bitmovua.c"
 g2 () == 0x12345678
# 85 "sh4a-bitmovua.c" 3
 ) ? (void)0 : __assert_func ("sh4a-bitmovua.c", 85, __func__, 
# 85 "sh4a-bitmovua.c"
 "g2 () == 0x12345678"
# 85 "sh4a-bitmovua.c" 3
 ))
# 85 "sh4a-bitmovua.c"
                             ;

  y3.d = 0x12345678;
  
# 88 "sh4a-bitmovua.c" 3
 ((
# 88 "sh4a-bitmovua.c"
 g3 () == 0x12345678
# 88 "sh4a-bitmovua.c" 3
 ) ? (void)0 : __assert_func ("sh4a-bitmovua.c", 88, __func__, 
# 88 "sh4a-bitmovua.c"
 "g3 () == 0x12345678"
# 88 "sh4a-bitmovua.c" 3
 ))
# 88 "sh4a-bitmovua.c"
                             ;

  return 0;
}
