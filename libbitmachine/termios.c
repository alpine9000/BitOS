#undef _KERNEL_BUILD
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "panic.h"
#include "simulator.h"
#include "console.h"
#include "bft.h"

//#define DEBUG_TERMIOS

#ifdef DEBUG_TERMIOS
#define printf _bft->simulator_printf
#else
static void ___do_nothing(const char * format, ...)
{

}
#define printf ___do_nothing
#endif


//https://www.gnu.org/software/termutils/manual/termcap-1.3/text/termcap.txt

int tputs(const char *str, int affcnt, int (*_putc)(int))
{
  if (strcmp(str, "le") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_BACKSPACE);
    _putc(0);
    printf("tputs le %d %x(console_backspace)\n", affcnt, putc);


  } else if (strcmp(str, "cr") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_SET_COL);
    _putc(0);
    printf("tputs cr 0\n");


  } else if (strcmp(str, "ce") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_CLEAR_TO_END_OF_LINE);
    _putc(1);    
    printf("tputs ce (console_clearToEndOfLine)\n");


  } else if (strstr(str, "IC") != 0) {
    int col, row;
    sscanf(str, "IC %d %d",  &col, &row);
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_INSERT_AT_CURSOR);
    _putc(row);    
    printf("tputs IC (console_insertAtCursor) %d\n", affcnt);

  } else if (strcmp(str, "cl") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_RESET);
    _putc(1);    
    printf("tputs cl (console_reset)\n");


  } else if (strstr(str, "DC") != 0) {
    int col, row;
    sscanf(str, "IC %d %d",  &col, &row);
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_DELETE_AT_CURSOR);
    _putc(row);    
    printf("tputs IC (console_deletetCursor)\n");

  } else if (strcmp(str, "up") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_UP);
    _putc(1);    
    printf("tputs up console_setCursorRow(console_getCursorRow()-1);\n");


  } else if (strcmp(str, "so") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_STANDOUT);
    _putc(1);    
  } else if (strcmp(str, "se") == 0) {
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_STANDOUT);
    _putc(0);    

  } else if (strstr(str, "cm") != 0) {
    int col, row;
    sscanf(str, "cm %d %d",  &col, &row);
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_SET_ROW);
    _putc(row);
    _putc(CONSOLE_CONTROL_ESCAPE);
    _putc(CONSOLE_SET_COL);
    _putc(col);

    printf("tputs %s [cm %d %d (console_setCursorPos)]\n", str, col, row);
  } else {
    printf("tputs %s %d\n", str, affcnt);
  }

  return 1;
}


int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p)
{
  printf("tcsetattr %d %x\n", fildes, optional_actions);
  printf("c_iflag = %x\n", termios_p->c_iflag);
  printf("c_oflag = %x\n", termios_p->c_oflag);
  printf("c_cflag = %x\n", termios_p->c_cflag);
  printf("c_lflag = %x\n", termios_p->c_lflag);
  //printf("raw = %d\n", termios_p->raw);
  //  console_setRaw(termios_p->raw);
  
  return 1;
}

int tcflow(int fildes, int action)
{
  panic("tcflow");
  return 0;
}

int tcflush(int fildes, int action)
{
  panic("tcflush");
  return 0;
}

char *tgoto(const char *cap, int col, int row)
{
  static char buffer[255];
  //  strcpy(buffer, cap);
  sprintf(buffer, "%s %d %d", cap, col, row);
  printf("tgoto %s %d %d\n", cap, col, row);
  return buffer;
}


int tcgetattr(int fd, struct termios *termios_p)
{
  printf("tcgetattr\n");
  memset(termios_p, 0xFFFF, sizeof(struct termios));
  termios_p->raw = 0;
  return 1;
}

int tgetnum(char *id)
{
  printf("tgetnum %s\n", id);
  if (strcmp(id, "co") == 0) {
    unsigned cols = console_getColumns();
    printf("%d\n", cols);
    return cols;
  }
  if (strcmp(id, "li") == 0) {
    unsigned lines = console_getLines();
    printf("%d\n", lines);
    return lines;
  }
  return 1;
}


char *tgetstr(char *id, char **area)
{
  
  if (strcmp(id, "kl") == 0) { // Left key
    static char val[] =  {0x80 | 37, 0};
    return val;
  } else if (strcmp(id, "kr") == 0) { // Right key
    static char val[] =  {0x80 | 39, 0};
    return val;
  } else if (strcmp(id, "ku") == 0) { // Up key
    static char val[] =  {0x80 | 36, 0};
    return val;
  } else if (strcmp(id, "kd") == 0) { // Down key
    static char val[] =  {0x80 | 40, 0};
    return val;
  } else if (strcmp(id, "km") == 0) { // Meta ?
    //return 0;
  }
  printf("tgetstr %s\n", id);
  return id;
}


int tgetflag(char *id)
{

  
  if (strcmp(id, "km") == 0) {
    //printf("tgetflag %s -> 1\n", id);
    return 1;
  }

  //  printf("tgetflag %s\n", id);
  return 0;
}

int tgetent(char *bp, const char *name)
{
  //printf("tgetent %s\n", name);
  return 1;
}


void cfmakeraw(struct termios *termios_p)
{
  termios_p->raw = 1;
}
