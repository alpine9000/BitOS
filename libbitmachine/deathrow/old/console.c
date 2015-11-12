#include "console.h"
#include "peripheral.h"
#include "gfx.h"
#include <stdio.h>

#define VIDEO
//#define CONSOLE


static unsigned int color = 0, x = 0, y = 0;

void console_reset()
{
  x = y = 0;
  color = 0xFF000000;
}

void console_setColor(unsigned char r, unsigned char g, unsigned char b)
{
#ifdef VIDEO
  color = 0xFF000000 | (((r << 16) & 0xFF0000) | ((g << 8) & 0xFF00) | b);
#endif
#ifdef CONSOLE
  peripheral.consoleColor = color;
#endif
}



void _console_write_char(char c) {
#ifdef CONSOLE
  peripheral.consoleWrite = c;
#endif
#ifdef VIDEO
  char string[] = {c, 0};

  if (c != '\n') {
    gfx_drawString(x, y, string, color);
    x+=(gfx_fontWidth+1);
    if (x >= videoWidth) {
      x = 0;
      y += gfx_fontHeight;
    }
  } else {
    x = 0;
    y+= gfx_fontHeight;
  }

  if (y >= videoHeight-gfx_fontHeight) {
    gfx_bitBlt(0, 0, 0, -gfx_fontHeight, videoWidth, videoHeight, 0, 0);
    y -= gfx_fontHeight;
  }
#endif
}

void _console_show_cursor() {
#ifdef VIDEO
   gfx_drawString(x, y, "|", color);
#endif
}

void _console_hide_cursor() {
#ifdef VIDEO
  gfx_fillRect(x, y, gfx_fontWidth, gfx_fontHeight, 0xFFFFFFFF); 
#endif
}

int _console_read_char() {
  return peripheral.consoleRead;
}

unsigned char _console_char_avail() {
  return peripheral.consoleReadBufferSize;
}


unsigned long console_isKeyDown(unsigned long key)
{
  peripheral.consoleSelectKeyState = key;
  return peripheral.consoleKeyState;
}

void print(char* s)
{
  printf(s);
  fflush(stdout);
}

void flushc(char c )
{
  putchar(c);
  fflush(stdout);
}


void console_backspace()
{
  x -= (2*(gfx_fontWidth+1));
  putchar(' ');
  fflush(stdout);

}

  
