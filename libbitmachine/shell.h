#pragma once

extern unsigned shell_windowWidth;
extern unsigned shell_windowHeight;

int 
shell_copy(char* s, char* dest_filename);

int
shell_stress(int argc, char** argv);

int
shell_test(int argc, char** argv);

int 
shell_filesAreIdentical(char* p1, char* p2);

void 
shell_exec(char* cmd);

void
shell_globArgv(char* command, int* out_argc, char*** out_argv);

char** 
shell_argvDup(int argc, char** argv, int skip);

const char *
shell_getFilenameExt(const char *filename);
