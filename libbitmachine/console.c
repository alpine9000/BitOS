#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "simulator.h"
#include "console.h"
#include "kernel.h"
#include "peripheral.h"
#include "gfx.h"
#include "kernel.h"
#include "window.h"
#include "panic.h"
#include "audio.h"

static const  unsigned console_characterSpacing = gfx_spaceWidth;


static unsigned 
_fb() 
{
  return window_getFrameBuffer(kernel_threadGetWindow());
}

static unsigned 
_w() 
{
  return window_getW(kernel_threadGetWindow());
}

static unsigned 
_h() 
{
  return window_getH(kernel_threadGetWindow());
}

static unsigned 
_x() 
{
  return window_getCursorX(kernel_threadGetWindow());
}

static unsigned 
_y() 
{
  return window_getCursorY(kernel_threadGetWindow());
}

static void 
_console_processControl(window_h windowH)
{
  switch (window_getConsoleControl(windowH)->type) {
  case CONSOLE_SET_ROW:
    console_setCursorRow(window_getConsoleControl(windowH)->arg);
    break;
  case CONSOLE_SET_COL:
    console_setCursorCol(window_getConsoleControl(windowH)->arg);
    break;
  case CONSOLE_STANDOUT:
    if (window_getConsoleControl(windowH)->arg) {
      console_setBehaviour(CONSOLE_BEHAVIOUR_STANDOUT);
    } else {
      console_clearBehaviour(CONSOLE_BEHAVIOUR_STANDOUT);
    }
    break;
  case CONSOLE_CLEAR_TO_END_OF_LINE:
    console_clearToEndOfLine();
    break;
  case CONSOLE_RESET:
    console_reset();
    break;
  case CONSOLE_INSERT_AT_CURSOR:
    if ((int)window_getConsoleControl(windowH)->arg < 0) {
      panic("_console_processControl: CONSOLE_INSERT_AT_CURSOR: negative arg");
    }
    for (unsigned i = 0; i < window_getConsoleControl(windowH)->arg; i++) {
      console_insertAtCursor();
    }
    break;
  case CONSOLE_DELETE_AT_CURSOR:
    if ((int)window_getConsoleControl(windowH)->arg < 0) {
      panic("_console_processControl: CONSOLE_DELETE_AT_CURSOR: negative arg");
    }
    for (unsigned i = 0; i < window_getConsoleControl(windowH)->arg; i++) {
      console_deleteAtCursor();
    }
    break;
  case CONSOLE_UP:
    console_setCursorRow(console_getCursorRow()-1);
    break;
  case CONSOLE_BACKSPACE:
    console_backspace();
    break;
  }

}

static void 
_console_output_char(char c)
{
  char string[] = {c, 0};
  unsigned x = _x(), y = _y();
  window_h windowH = kernel_threadGetWindow();
  unsigned background;
  unsigned foreground;
    
  if (window_getConsoleControl(windowH)->behaviour & CONSOLE_BEHAVIOUR_STANDOUT) {
    foreground = window_getBackgroundColor(windowH);
    background = window_getColor(windowH);
  } else {
    background = window_getBackgroundColor(windowH);
    foreground = window_getColor(windowH);
  }

  if (c != '\n') {
    gfx_fillRect(_fb(), x, y, window_getCharacterPixelWidth(windowH)+ console_characterSpacing, window_getCharacterPixelHeight(windowH), background); 
    gfx_drawString(_fb(), x, y, string, foreground);
    x+=(window_getCharacterPixelWidth(windowH) + console_characterSpacing);
    if (window_getConsoleControl(windowH)->behaviour & CONSOLE_BEHAVIOUR_AUTO_WRAP) {
      if (x >= _w()) {
	x = 0;
	y += window_getCharacterPixelHeight(windowH);
      }
    }
  } else {
    x = 0;
    y+= window_getCharacterPixelHeight(windowH);
  }
  
  if (window_getConsoleControl(windowH)->behaviour & CONSOLE_BEHAVIOUR_AUTO_SCROLL) {
    if (y >= _h()-window_getCharacterPixelHeight(windowH)) {
      gfx_bitBlt(_fb(), 0, 0, 0, -window_getCharacterPixelHeight(windowH), _w(), _h(), _fb());
      gfx_fillRect(_fb(), 0, _h()-window_getCharacterPixelHeight(windowH), _w(), window_getCharacterPixelHeight(windowH), background);
      y -= window_getCharacterPixelHeight(windowH);
    }
  }

  window_setCursor(windowH, x, y);
}

void 
_console_write_char(char c) 
{
  fds_t *fds = kernel_threadGetFds();
  if (fds->_stdout == STDOUT_FILENO) {
    window_h windowH = kernel_threadGetWindow();
    console_control_t *control = window_getConsoleControl(windowH);

    if (control->behaviour & CONSOLE_BEHAVIOUR_AUTO_SCROLL) {
      peripheral.console.consoleWrite = c;
    }

    switch (control->state) {
    case WAITING_FOR_TYPE:
      control->type = c;
      control->state++;
      return;
    case WAITING_FOR_ARG:
      control->arg = c;
      control->state = 0;
      _console_processControl(windowH);
      return;
    default:
      break;
    }
    
    switch (c) {
    case  7: // bell
      audio_bell();
      return;
    case CONSOLE_CONTROL_ESCAPE: // Data link escape
      control->state++;
      return;
    }
    
    switch (c) {
    case 1: //CTRL_A
      break;
    case 11: // CTRL_K
      break;
    default:
      _console_output_char(c);
      break;
    } 
    

  } else {
    write(fds->_stdout, &c, 1);
  }
}

unsigned char 
_console_char_avail() 
{
  fds_t *fds = kernel_threadGetFds();
  if (fds->_stdin == STDIN_FILENO) {
    if (window_getZ(kernel_threadGetWindow()) == 0) {
      return peripheral.console.consoleReadBufferSize;
    } else {
      return 0;
    }
  } else {
    panic("_console_char_avail on file\n");
    return 0;
  }
}

int 
_console_read_char() 
{
  fds_t *fds = kernel_threadGetFds();
  if (fds->_stdin == STDIN_FILENO) {
    if (window_getZ(kernel_threadGetWindow()) == 0) {
      //return peripheral.console.consoleRead;
      int c = peripheral.console.consoleRead;
      if (c == 3) { // ctrl-c
	raise(SIGINT);
      }
      return c;
    } else {
      return -1;
    }
  } else {
    int c;
    return read(fds->_stdin, &c, 1);
  }
}

unsigned 
console_isKeyDown(unsigned key)
{
  peripheral.console.consoleSelectKeyState = key;
  return peripheral.console.consoleKeyState;
}

void 
console_backspace()  
{
  window_setCursor(kernel_threadGetWindow(), _x() - (window_getCharacterPixelWidth(window)+console_characterSpacing), _y());
}

void 
console_insertAtCursor()
{
  unsigned dx = _x()+window_getCharacterPixelWidth(window)+console_characterSpacing;
  gfx_bitBlt(_fb(), _x(), _y(), dx, _y(), _w()-dx, window_getCharacterPixelHeight(window), _fb());
}

void 
console_deleteAtCursor()
{
  unsigned dx = _x()+window_getCharacterPixelWidth(window)+console_characterSpacing;
  gfx_bitBlt(_fb(), dx, _y(), _x(), _y(), _w()-dx, window_getCharacterPixelHeight(window), _fb());
}

void 
console_setCursorPos(unsigned col, unsigned row)
{
  window_h w = kernel_threadGetWindow();
  
  unsigned x = col * (window_getCharacterPixelWidth(w)+console_characterSpacing);
  unsigned y = row * window_getCharacterPixelHeight(w);
  
  window_setCursor(w, x, y);
}

void 
console_setCursorCol(unsigned col)
{
  unsigned x = col * (window_getCharacterPixelWidth(window)+console_characterSpacing);
  
  window_setCursorX(kernel_threadGetWindow(), x);
}

void 
console_setCursorRow(unsigned row)
{
  window_h w = kernel_threadGetWindow();
  unsigned y = row * (window_getCharacterPixelHeight(w));
  
  window_setCursorY(w, y);

}

unsigned 
console_getCursorRow()
{
  window_h w = kernel_threadGetWindow();
  return window_getCursorY(w) / window_getCharacterPixelHeight(w);
}

void 
console_clearToEndOfLine()
{
  gfx_fillRect(_fb(), _x(), _y(), _w()-_x(), window_getCharacterPixelHeight(window), window_getBackgroundColor(kernel_threadGetWindow())); 
}


unsigned 
console_getColumns()
{
  window_h w = kernel_threadGetWindow();
  return  window_getW(w) / (window_getCharacterPixelWidth(w)+console_characterSpacing) + 1;
}


unsigned console_getLines()
{
  window_h w = kernel_threadGetWindow();
  return window_getH(w) / (window_getCharacterPixelHeight(w));
}

void 
console_reset()
{
  console_setCursorPos(0, 0);
  gfx_fillRect(_fb(), _x(), _y(), _w(), _h(), window_getBackgroundColor(kernel_threadGetWindow())); 
}

void 
console_clearBehaviour(unsigned mask)
{
  window_getConsoleControl(kernel_threadGetWindow())->behaviour &= ~mask;
}

void 
console_setBehaviour(unsigned mask)
{
  window_getConsoleControl(kernel_threadGetWindow())->behaviour |= mask;
}
