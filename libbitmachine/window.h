#pragma once

#include "types.h"

#define WINDOWS_MAX 20

typedef enum {
  WINDOW_DEAD = 0,
  WINDOW_VISIBLE,
  WINDOW_HIDDEN
} window_state_t;

#ifdef _KERNEL_BUILD

void 
window_init();

void 
window_loop();

window_h 
window_create(char* title, unsigned x, unsigned y, unsigned w, unsigned h);

console_control_t* 
window_getConsoleControl(window_h window);

void 
window_close(window_h window);

int 
window_getFrameBuffer(window_h window);

unsigned 
window_isKeyDown(window_h window, unsigned key);

unsigned 
window_charAvailable(window_h window);

int 
window_readChar(window_h window);

int 
window_getZ(window_h window);

unsigned 
window_getH(window_h window);

unsigned 
window_getW(window_h window);

unsigned 
window_getCursorX(window_h window);

unsigned 
window_getCursorY(window_h window);

void 
window_setCursor(window_h window, unsigned x, unsigned y);

void 
window_setBase(void* base);

void* 
window_getBase();

void 
window_toTop(window_h window);

void 
window_setCursorX(window_h window, unsigned x);

void 
window_setCursorY(window_h window, unsigned y);

unsigned 
window_getColor(window_h window);

unsigned 
window_getBackgroundColor(window_h window);

void 
window_enableCursor(window_h window, unsigned enabled);

void
window_cleanup(thread_h owner);

#define window_getCharacterPixelWidth(w) (gfx_fontWidth)
#define window_getCharacterPixelHeight(w) (gfx_fontHeight)

#else

#include "bft.h"
#define window_create _bft->window_create
#define window_getFrameBuffer _bft->window_getFrameBuffer
#define window_close _bft->window_close
#define window_isKeyDown(window, key) _bft->window_isKeyDown(window,key)

#endif

#define window_titleBarHeight  (gfx_fontHeight+4)
