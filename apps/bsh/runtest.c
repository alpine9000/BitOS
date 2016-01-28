#include <stdio.h>
#include <sys/syslimits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/dirent.h>
#include <string.h>
#include "shell.h"
#include "thread.h"
#include "commands.h"

static int numPass = 0;
static int numFail = 0;

static int
_runtest_file(char* filename, int force)
{
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  char resultFilename[PATH_MAX];
  char failFilename[PATH_MAX];
  char fullPath[PATH_MAX];

  sprintf(resultFilename, "%s/%s.pass", cwd, filename);
  sprintf(failFilename, "%s/%s.fail", cwd, filename);

  sprintf(fullPath, "%s/%s", cwd, filename);
  
  if (!force) {
    struct stat buf;
    if (stat(resultFilename, &buf) == 0) {
      printf("%s... Skipped [Passed]\n", fullPath);
      numPass++;
      return 1;
    }

    if (stat(failFilename, &buf) == 0) {
      printf("%s... Skipped [Failed]\n", fullPath);
      numFail++;
      return 1;
    }
  }

  char touchCmd[PATH_MAX*2];
  printf("%s... ", fullPath);
  if (thread_run(fullPath) == 0) {
    sprintf(touchCmd, "touch %s", resultFilename);
    shell_exec(touchCmd);
    numPass++;
    printf("Passed\n");
  } else {
    sprintf(touchCmd, "touch %s", failFilename);
    shell_exec(touchCmd);
    numFail++;
    printf("Failed\n");
  }

  return 1;
}


static int
_runtest_dir(char* src, int force)
{
  struct stat buf;
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);

  if (stat(src, &buf) == 0) {
    if (S_ISDIR(buf.st_mode)) {
      printf("Entering %s/%s...\n", cwd, src);
      DIR* dirp = opendir(src);
      struct dirent* dp;
      
      do {
	if ((dp = readdir(dirp)) != NULL) {
	  char sbuffer[PATH_MAX];
	  sprintf(sbuffer, "%s/%s", src, dp->d_name);
	  _runtest_dir(sbuffer, force);
	}
      } while (dp != NULL);	
      closedir(dirp);      
    } else {
      if (strcmp(shell_getFilenameExt(src), "test") == 0) {
	_runtest_file(src, force);
      }
    }
  }

  return 1;
}


int 
runtest_rt(int argc, char** argv)
{
  int c;
  int recursive = 0;
  int force = 0;
  numPass = 0;
  numFail = 0;
  
  while ((c = getopt(argc, argv, "rf")) != -1) {  
    switch (c) {
    case 'r':
      recursive = 1;
      break;
    case 'f':
      force = 1;
      break;
    default:
      commands_usage(argv[0]);
      return -1;
      break;
    }
  }


  if (recursive) {
    _runtest_dir(".", force);
  } else {
    if (argc == 1) {
      commands_usage(argv[0]);
      return -1;
    } else {	
      for (int i = 1; i < argc; i++) {
	_runtest_file(argv[i], force);
      }
    }
  }

  printf("\n\n%d tests completed\n%d passed\n%d failed\n", numPass+numFail, numPass, numFail);

  return 0;
}
