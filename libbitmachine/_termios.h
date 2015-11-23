#pragma once

int tputs(const char *str, int affcnt, int (*putc)(int));
int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p);
int tcflow(int fildes, int action);
int tcflush(int fildes, int action);
char* tgoto(const char *cap, int col, int row);
int tcgetattr(int fd, struct termios *termios_p);
int tgetnum(char *id);
char* tgetstr(char *id, char **area);
int tgetflag(char *id);
int tgetent(char *bp, const char *name);

