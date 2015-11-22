#pragma once
#include "bft.h"

#define CONSOLE_CONTROL_ESCAPE 16

#ifdef _KERNEL_BUILD

extern unsigned console_color;
extern unsigned console_backgroundColor;

int
_console_read_char();

void 
_console_write_char(char c);

unsigned char 
_console_char_avail();

void 
console_setColor(unsigned char r, unsigned char g, unsigned char b);

unsigned 
console_isKeyDown(unsigned key);

void 
console_backspace(void);

void 
console_setCursorPos(unsigned col, unsigned row);

void 
console_setCursorCol(unsigned col);

void 
console_setCursorRow(unsigned row);

unsigned 
console_getCursorRow();

void 
console_insertAtCursor();

void 
console_deleteAtCursor();

void 
console_clearToEndOfLine();

unsigned 
console_getColumns();

unsigned 
console_getLines();

void 
console_reset();

void 
console_setBehaviour(unsigned mask);

void 
console_clearBehaviour(unsigned mask);

#else

#define _console_read_char() _bft->_console_read_char()
#define _console_write_char(x) _bft->_console_write_char(x)
#define _console_char_avail() _bft->_console_char_avail()
#define console_setColor(rgb) _bft->console_setColor(r, g, b)
#define console_isKeyDown(k) _bft->console_isKeyDown(k)
#define console_backspace() _bft->console_backspace()
#define console_setCursorPos(c,r) _bft->console_setCursorPos(c, r)
#define console_setCursorCol(c) _bft->console_setCursorCol(c)
#define console_setCursorRow(r) _bft->console_setCursorRow(r)
#define console_getCursorRow() _bft->console_getCursorRow()
#define console_insertAtCursor() _bft->console_insertAtCursor()
#define console_deleteAtCursor() _bft->console_deleteAtCursor()
#define console_clearToEndOfLine() _bft->console_clearToEndOfLine()
#define console_getColumns() _bft->console_getColumns()
#define console_getLines() _bft->console_getLines()
#define console_reset() _bft->console_reset()
#define console_setBehaviour(x) _bft->console_setBehaviour(x)
#define console_clearBehaviour(so) _bft->console_clearBehaviour(so)

#endif


