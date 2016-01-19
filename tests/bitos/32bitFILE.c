/* { dg-do run } */
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>

void test(char* filename)
{
  FILE *fp = fopen(filename, "r");
  
  if (fp == 0) {
    abort();
  }
  

  char buffer[255];
  for(;;) {
    int len = fread(&buffer, 1, sizeof(buffer)-1, fp);
    if (len > 0) {
      buffer[len] = 0;
    } else {
      break;
    }
  } 

  if (fclose(fp) != 0) {
    abort();
  }
}

void prime(char* filename)
{
  FILE *fp = fopen(filename, "w");
  
  if (fp == 0) {
    abort();
  }

  if (fclose(fp) != 0) {
    abort();
  }
}

int main(int argc, char** argv)
{
  char tmpFile[PATH_MAX];

  snprintf(tmpFile, PATH_MAX, "%s.tmp", argv[0]);
  FILE *fp = fopen(tmpFile, "w");
  
  if (fp == 0) {
    abort();
  }

  if (fprintf(fp, "Hello\n") < 0) {
    abort();
  }

  fclose(fp);

  for (int i = 0; i < 35000; i++) {
    prime("32bitFILE.nothing");
  }

  test(tmpFile);

  return 0;
}
