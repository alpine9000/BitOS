#pragma once

extern unsigned shell_windowWidth;
extern unsigned shell_windowHeight;


int 
shell_filesAreIdentical(char* p1, char* p2);

void 
shell_exec(char* cmd);

int
shell_execBuiltinFromArgv(int argc, char** argv, int argvSkip);

void
shell_globArgv(char* command, int* out_argc, char*** out_argv);

char** 
shell_argvDup(int argc, char** argv, int skip);

const char *
shell_getFilenameExt(const char *filename);

int
shell_getc();

void
shell_init();

int 
shell_rcopy(char* src, char* dest);

struct timeval* 
shell_timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);

int
shell_listPath(char* path, int argc, char** argv, int longFlag);

void
shell_cleanup();

int 
shell_copy(char* s, char* dest_filename);

int 
shell_isOption(char* argv, char option);

int 
shell_isLongOption(char* argv, char* option);

int 
shell_hasOption(int argc, char** argv, char option);

int 
shell_hasLongOption(int argc, char** argv, char* option);

int 
shell_threadLoad(char* commandLine);
