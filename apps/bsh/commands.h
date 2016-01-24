#pragma once

int 
shell_execBuiltin(int argc, char** argv);

int 
shell_launchBuiltin(int argc, char** argv);

int 
shell_copy(char* s, char* dest_filename);

int
shell_stress(int argc, char** argv);

int
shell_test(int argc, char** argv);

