#pragma once

extern unsigned shell_windowWidth;
extern unsigned shell_windowHeight;

int 
shell_copy(char* s, char* dest_filename);

int
shell_stress(int argc, char** argv);

int
shell_test(int argc, char** argv);
