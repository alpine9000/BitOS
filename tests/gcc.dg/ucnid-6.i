# 1 "ucnid-6.c"
# 1 "/Users/alex/Projects/BitOS/tests/gcc.dg//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "ucnid-6.c"




void abort (void);

int \U000000c0(void) { return 1; }
int \U000000c1(void) { return 2; }
int \U000000c2(void) { return 3; }
int wh\U000000ff(void) { return 4; }
int a\U000000c4b\U00000441\U000003b4e(void) { return 5; }

int main (void)
{

  if (\U000000c0() != 1)
    abort ();
  if (\U000000c1() != 2)
    abort ();
  if (\U000000c2() != 3)
    abort ();
  if (wh\U000000ff() != 4)
    abort ();
  if (a\U000000c4b\U00000441\U000003b4e() != 5)
    abort ();

  return 0;
}
