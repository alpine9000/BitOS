#include <stdio.h>
#include <sys/syslimits.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "process.h"

static char* filename_base(char* filename, char *buffer)
{
  strcpy(buffer, filename);
  for (int i = strlen(filename); i > 0; --i) {
    if (filename[i] == '.') {
      buffer[i] = 0;
      break;
    }
  }

  return buffer;
}

static char* filename_ext(char* filename)
{
  char* ext = strrchr(filename, '.');
  if (!ext) {
    return 0;
  } else {
    return ext + 1;
  }
}


static struct timeval* timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return result;
}


static int build_needsBuild(char* src, char* dest)
{
  struct stat st_src, st_dest;

  if (stat(src, &st_src) == 0 && stat(dest, &st_dest) == 0) {
    return st_src.st_mtime > st_dest.st_mtime;
  } else {
    return 1;
  }
}

static int build_compileSource(unsigned force, char* filename, char* compiler, char* flags)
{
  char source[PATH_MAX];
  char dest[PATH_MAX];
  char command[1024];
  char basename[255];

  filename_base(filename, basename);
  sprintf(source, "%s", filename);
  sprintf(dest, "%s.s", basename);

  if (force || build_needsBuild(source, dest)) {
    sprintf(command, "%s %s %s -o %s -dumpbase %s -auxbase-strip %s", compiler, flags, source, dest, source, dest);
    printf(command);
    printf("\n");
    return process_load(command);
  } else {
    printf("%s is up to date\n", dest);
    return 1;
  }
}

static int build_assemble(unsigned force, char* filename)
{
  char source[PATH_MAX];
  char dest[PATH_MAX];
  char command[1024];
  char basename[255];

  filename_base(filename, basename);
  sprintf(source, "%s", filename);
  sprintf(dest, "%s.o", basename);

  if (force || build_needsBuild(source, dest)) {
    sprintf(command, "as %s -o %s", source, dest);
    printf(command);
    printf("\n");
    return process_load(command);
  } else {
    printf("%s is up to date\n", dest);
    return 1;
  }
}

static int link_(char* filename)
{
  char buffer[1024];
  char base[PATH_MAX];

  filename_base(filename, base);
  sprintf(buffer, "ld -pie /usr/local/sh-elf/lib/gcc/sh-elf/5.1.0/m2e/crt1.o %s  /usr/local/sh-elf/sh-elf/lib/m2e/libc.a /usr/local/sh-elf/lib/gcc/sh-elf/5.1.0/m2e/libgcc.a -o /usr/local/bin/%s",filename, base);
  printf(buffer);
  printf("\n");
  return process_load(buffer);
}

static int build_compile(unsigned force, char* filename, char* cc1flags, char* cxxflags)
{
  char buffer[PATH_MAX];
  char base[PATH_MAX];
  char* ext = filename_ext(filename);
  int retval = 0;

  if (strcmp(ext, "S") == 0 || strcmp(ext, "s") == 0) {
    retval =  build_assemble(force, filename);
  } else {      
    if (strcmp(ext, "c") == 0) {
      retval = build_compileSource(force, filename, "cc1", cc1flags);      
    } else if (strcmp(ext, "cpp") == 0) {
      retval = build_compileSource(force, filename, "cc1plus", cxxflags);	
    } 
    if (retval) {
      sprintf(buffer, "%s.s", filename_base(filename, base));
      retval =  build_assemble(force, buffer);
    }
  }

  return retval;
}


static int build_files(int force, char** files)
{
  const char* CC1FLAGS="-D_KERNEL_ASSERTS -D_KERNEL_BUILD -O3 -Wfatal-errors -Werror -Wall -Wextra -Wno-unused-parameter -I. -m2e -funit-at-a-time -falign-jumps -quiet -imultilib m2e -iprefix /usr/local/sh-elf/5.1.0/lib/gcc/sh-elf/5.1.0  -I/usr/local/src/BitOS -I/usr/local/src/BitOS/libbitmachine";
  const char* CXXFLAGS="-D_KERNEL_BUILD -O3 -Wfatal-errors -Wall -Werror -Wextra -Wno-unused-parameter -Wno-char-subscripts  -m2e -funit-at-a-time -falign-jumps -quiet -imultilib m2e -iprefix /usr/local/sh-elf/sh-elf/include/c++/5.1.0/sh-elf/ -I/usr/local/src/BitOS -I/usr/local/src/BitOS/libbitmachine ";

  for (int i = 0; files[i] != 0; i++) {
    if (!build_compile(force, files[i], (char*)CC1FLAGS, (char*)CXXFLAGS)) {
      return 0;
    }
  }

  return 1;
}

static void build_appendObjectFiles(char** files, char* buffer, unsigned bufMax)
{
  for (int i = 0; files[i] != 0; i++) {
    char basename[PATH_MAX];
    filename_base(files[i], basename);
    strncat(buffer, basename, bufMax);
    strncat(buffer, ".o ", bufMax);
  }
}

static int build_elf(char** files, char* output)
{
  const unsigned bufMax = 4096;
  char buffer[bufMax];

  snprintf(buffer, bufMax, "ld -L. -L/usr/local/sh-elf/sh-elf/lib/m2e  -L/usr/local/sh-elf/lib/gcc/sh-elf/5.1.0/m2e -L/usr/local/sh-elf/lib -T start.l -no-keep-memory -o %s ", output);

  build_appendObjectFiles(files, buffer, bufMax);    

  strncat(buffer, "/usr/local/sh-elf/sh-elf/lib/m2e/crt0.o -L. -Llibbitmachine  --start-group -lwolf -lbitmachine -lc-kernel -lm -lgcc --end-group", bufMax);

  printf("%s\n", buffer);
  return process_load(buffer);
}

static int build_library(unsigned force, char* library, char** files)
{
  const unsigned bufMax = 4096;
  char buffer[bufMax];

  snprintf(buffer, bufMax, "ar rcsv %s ", library);

  build_appendObjectFiles(files, buffer, bufMax);

  printf("%s\n", buffer);
  return process_load(buffer);
}


static void build_bitos(unsigned force, char* srcdir, char* output)
{
  char libdir[PATH_MAX];
  char* files[] = { "apps/wolf/wolf.cpp" , "media/martini.c", "vector.c", "init.c",  0};
  char* libfiles[] = { "start.s", "syscall.s", "kernel_asm.S", "kernel_client_asm.S", "cpp.c", "glcdfont.c", "gfx.c", "console.c", "audio.c", "simulator.c", "interrupts.c", "file.c", "kernel.c", "window.c", "malloc.c", "memory.c", "syscalls.c", "bft_kernel.c", "panic.c", "argv.c", "process.c", "shell.c", "build.c", 0};

  sprintf(libdir, "%s/libbitmachine", srcdir);
  
  chdir(libdir);
  if (build_files(force, libfiles) == 0) {
    return;
  }

  if (build_library(force, "libbitmachine.a", libfiles) == 0) {
    return;
  }

  chdir(srcdir);

  if (build_files(force, files) == 0) {
    return;
  }
  
  build_elf(files, output);
}

void build(int argc, char** argv)
{
  unsigned force = 0;

  if (argc > 1 && strcmp(argv[1], "-f") == 0) {
    force = 1;
  }

  build_bitos(force, "/usr/local/src/BitOS", "/usr/local/src/BitOS/bin/bitos.elf");
}

void build2(int argc, char** argv)
{
  unsigned force = 0;

  if (argc > 1 && strcmp(argv[1], "-f") == 0) {
    force = 1;
  }


  build_bitos(force, "/usr/local/src/BitOS", "/usr/local/src/BitOS/bitos-local-2.elf");
}


void bcc(int argc, char** argv)
{
  if (argc != 2) {
    printf("usage: %s file (then get ready for a long wait)\n", argv[0]);
    return;
  }


  struct timeval start, end, diff;
  char buffer[1024];
  char base[PATH_MAX];
  filename_base(argv[1], base);

  gettimeofday(&start,0);

  int success = 0;
  if (build_compile(1, argv[1], "-m2e -quiet", "-m2e -pie")) {
    sprintf(buffer, "%s.o", base);
    success = link_(buffer);
  } else {
    success = 0;
  }

  gettimeofday(&end,0);

  timeval_subtract(&diff, &end, &start);

  printf("completed [%s] in %ld.%03ld\n", success ? "success" : "failed", diff.tv_sec, (diff.tv_usec/1000));
}
