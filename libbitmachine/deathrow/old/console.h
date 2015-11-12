#ifndef ___CONSOLE_H
#define ___CONSOLE_H


void print(char* s);
void flushc(char c );

void _console_show_cursor();
void _console_hide_cursor();
int _console_read_char();
void _console_write_char(char c);
unsigned char _console_char_avail();

void console_setColor(unsigned char r, unsigned char g, unsigned char b);
void console_reset();
unsigned long console_isKeyDown(unsigned long key);
void console_backspace();

#endif
