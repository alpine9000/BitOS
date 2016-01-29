#pragma once

int 
commands_execBuiltin(int argc, char** argv);

int 
commands_launchBuiltin(int argc, char** argv);

int
shell_stress(int argc, char** argv);

int
commands_test(int argc, char** argv);

void 
commands_usage(char* command);

char *
commands_generator (const char *text, int state);
