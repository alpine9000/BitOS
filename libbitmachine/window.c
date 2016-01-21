#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "peripheral.h"
#include "kernel.h"
#include "gfx.h"
#include "console.h"
#include "window.h"
#include "memory.h"
#include "simulator.h"

extern const unsigned char martini_rgba[];

//static const unsigned systemBarHeight = 12;
static const unsigned systemBarHeight = (gfx_fontHeight+4);
//static const unsigned titleBarHeight = 12;


static const unsigned windowBackgroundColor = 0xFFFFFFFF;
static const unsigned windowColor = 0xFF000000;
static const unsigned cursorColor = 0x5F000000;
static const unsigned desktopColor = 0xFF222222;
static const unsigned systemBarBackgroundColor = 0xFF000000;
static const unsigned systemBarColor = 0xFFFFFFFF;
static unsigned window_nextHandle = 1;

typedef struct {
  char *title;
  unsigned frameBuffer;
  int x;
  int y;
  unsigned w;
  unsigned h;
  unsigned cursorX;
  unsigned cursorY;
  unsigned cursorOn;
  window_state_t state;
  window_h handle;
  console_control_t consoleControl;
  thread_h owner;
} window_t;

typedef struct {
  window_t windows[WINDOWS_MAX];
  int zIndex[WINDOWS_MAX];
  unsigned lock ;
  unsigned titleFrameBuffer;
} window_table_t;

unsigned window_fullRefresh = 1;
static window_table_t windowTable;

static void
_window_close(window_h window);

static inline int 
_window_index(window_h window)
{
  for (int i = 0; i < WINDOWS_MAX; ++i) {
    if (windowTable.windows[i].handle == window) {
      return i;
    }
  }
  return -1;
}

console_control_t* 
window_getConsoleControl(window_h window)
{
  return &windowTable.windows[_window_index(window)].consoleControl;
}

static inline window_h 
_window_handle(int index)
{
  return windowTable.windows[index].handle;
}

static void 
window_move(window_h window, int x, int y)
{
  if (y < 0) {
    y = 0;
  }
  windowTable.windows[_window_index(window)].x = x;
  windowTable.windows[_window_index(window)].y = y;
  window_fullRefresh = 1;
}


static void 
window_composite(unsigned fb)
{
  gfx_alpha(fb, 255);

  gfx_fillRect(fb, 0, systemBarHeight, videoWidth, videoHeight-systemBarHeight, desktopColor);

  static unsigned char count = 0;
  static char buffer[255];
  int dirtyX1 = 0x7FFFFFFF;
  int dirtyY1 = 0x7FFFFFFF;
  int dirtyX2 = 0;
  int dirtyY2 = 0;
  int systemBarDirty = 0;

  if (0 || count++ == 0) {
    systemBarDirty = 1;
    extern struct mallinfo dlmallinfo();
    struct mallinfo mi = dlmallinfo();
    sprintf(buffer, "BitOS   ram: %d, free: %d", memory_total(), memory_total() - mi.uordblks);
    gfx_fillRect(fb, 0, 0, videoWidth, systemBarHeight, systemBarBackgroundColor);    
    gfx_drawString(fb, 2, 2, buffer, systemBarColor);  
  } 

  for (int i = WINDOWS_MAX-1; i >= 0; --i) {
    int wi = windowTable.zIndex[i];
    if (wi >= 0 && windowTable.windows[wi].state == WINDOW_VISIBLE) {
      if (i == 0) {
	gfx_alpha(fb, 255);
      } else {
	gfx_alpha(fb, 200);
      }

      window_t* w = &windowTable.windows[wi];
      unsigned y = w->y+systemBarHeight; 
      gfx_fillRect(fb, w->x, y, w->w, window_titleBarHeight, windowBackgroundColor);
      unsigned titleWidth = strlen(w->title);
      unsigned textWidth = titleWidth*(gfx_fontWidth+gfx_spaceWidth);
      gfx_bitBltEx(fb, 0, 0, w->x+textWidth+4, y+2, 1, 8, w->w-6-textWidth, window_titleBarHeight-4, windowTable.titleFrameBuffer);
      gfx_drawString(fb, w->x+2, y+2, w->title, windowColor);
      gfx_bitBltEx(fb, 0, 0, w->x, y+window_titleBarHeight, w->w, w->h, w->w, w->h, w->frameBuffer);

      if (!window_fullRefresh && gfx_frameFrameBufferDirty(w->frameBuffer)) {
	if (w->x < dirtyX1) {
	  dirtyX1 = w->x;
	}
	if ((int)(w->x+w->w) > dirtyX2) {
	  dirtyX2 = w->x+w->w;
	}
	if (w->y < dirtyY1) {
	  dirtyY1 = w->y;
	}
	if ((int)(w->y+w->h+window_titleBarHeight+systemBarHeight) > dirtyY2) {
	  dirtyY2 = w->y+w->h+window_titleBarHeight+systemBarHeight;
	}
      }
    }
  }


  int topIndex = windowTable.zIndex[0];
  if (topIndex != -1) {
    window_t* w = &windowTable.windows[topIndex];
    if (w->state == WINDOW_VISIBLE && w->cursorOn) {
      gfx_fillRect(fb, w->x+w->cursorX, w->y+w->cursorY+systemBarHeight+window_titleBarHeight-1, gfx_fontWidth+1, gfx_fontHeight+2, cursorColor); 
    }
  }

  if (window_fullRefresh) {
    window_fullRefresh = 0;
    gfx_bitBltEx(0, 0, 0, 0, 0, videoWidth, videoHeight, videoWidth, videoHeight, fb);
  } else {
    gfx_bitBlt(0, dirtyX1, dirtyY1, dirtyX1, dirtyY1, dirtyX2-dirtyX1, dirtyY2-dirtyY1, fb);
    if (systemBarDirty) {
      gfx_bitBlt(0, 0, 0, 0, 0, videoWidth, systemBarHeight, fb);
    }
  }
}

static int 
window_fromCoord(int x, int y)
{
  for (int i = 0; i < WINDOWS_MAX; ++i) {
    int wi = windowTable.zIndex[i];
    if (wi >= 0 && windowTable.windows[wi].state == WINDOW_VISIBLE) {
      window_t* w = &windowTable.windows[wi];
      if (w->x <= x &&
	  w->y <= y && 
	  (int)(w->x+w->w) > x && 
	  (int)(w->y+w->h) > y) {
	return wi;
      }
    }
  }
  
  return -1;
}

static void 
_window_add_to_z(unsigned windowIndex)
{
  for (int i = 0; i < WINDOWS_MAX; i++) {
    if (windowTable.zIndex[i] == -1) {
      windowTable.zIndex[i] = windowIndex;
      break;
    }
  }
}

static void 
_window_remove_from_z(window_h window)
{
  int current = window_getZ(window);
  for (int i = current; i>= 0 &&  i <  WINDOWS_MAX-1; i++) {
    windowTable.zIndex[i] = windowTable.zIndex[i+1];
  }
}

void 
window_loop()
{
  int w = -1;
  int ox, oy;
  unsigned fb = gfx_createFrameBuffer(videoWidth, videoHeight);

  for(;;) {
    int x = peripheral.mouse.x;
    int y = peripheral.mouse.y;
    if (peripheral.mouse.button && w == -1) {
      w = window_fromCoord(x, y);
      if (w >= 0) {
	window_toTop(_window_handle(w));
	ox = x - windowTable.windows[w].x;
	oy = y - windowTable.windows[w].y;
      }
    } else if (!peripheral.mouse.button && w != -1) {
      w = -1;
    }

    if (w >= 0) {
      window_move(_window_handle(w), x-ox, y-oy);
    }

    window_composite(fb);
    gfx_vsync();
  }
}



void 
window_init()
{
  windowTable.titleFrameBuffer =  gfx_createFrameBufferFromData(1, 8, (unsigned*)&martini_rgba);
  
  for (int i = 0; i < WINDOWS_MAX; i++) {
    windowTable.zIndex[i] = -1;
  }
}

void 
window_toTop(window_h window) {
  int current = window_getZ(window);
  for (int i = current; i >  0; i--) {
    if (i > 0) {
      windowTable.zIndex[i] = windowTable.zIndex[i-1];
    }
  }
  windowTable.zIndex[0] = _window_index(window);
}


unsigned 
window_isKeyDown(window_h window, unsigned key)
{
  if (windowTable.zIndex[0] == _window_index(window)) {
    return console_isKeyDown(key);
  } else {
    return 0;
  }
}

int 
window_readChar(window_h window) {
  if (windowTable.zIndex[0] == _window_index(window)) {
    return _console_read_char();
  } else {
    return -1;
  }
}

unsigned 
window_charAvailable(window_h window)
{
  if (windowTable.zIndex[0] == _window_index(window)) {
    return _console_char_avail();
  } else {
    return 0;
  }
}

void
window_cleanup(thread_h owner)
{
  kernel_spinLock(&windowTable.lock);

  for (unsigned i = 0; i < WINDOWS_MAX; i++) {
    if (windowTable.windows[i].state != WINDOW_DEAD &&  windowTable.windows[i].owner == owner) {
      _window_close(windowTable.windows[i].handle);
    }
  }

  kernel_unlock(&windowTable.lock);
}

window_h 
window_create(char* title, unsigned x, unsigned y, unsigned w, unsigned h)
{
  kernel_spinLock(&windowTable.lock);
  for (unsigned i = 0; i < WINDOWS_MAX; i++) {
    if (windowTable.windows[i].state == WINDOW_DEAD) {
      windowTable.windows[i].title = title;
      windowTable.windows[i].state = WINDOW_VISIBLE;
      windowTable.windows[i].frameBuffer = gfx_createFrameBuffer(w, h);
      windowTable.windows[i].x = x;
      windowTable.windows[i].y = y;
      windowTable.windows[i].w = w;
      windowTable.windows[i].h = h;
      windowTable.windows[i].cursorX = 0;
      windowTable.windows[i].cursorY = 0;
      windowTable.windows[i].cursorOn = 1;
      windowTable.windows[i].handle = (window_h)window_nextHandle++;
      windowTable.windows[i].consoleControl.behaviour = CONSOLE_BEHAVIOUR_AUTO_WRAP | CONSOLE_BEHAVIOUR_AUTO_SCROLL;
      windowTable.windows[i].owner = kernel_threadGetId();
      _window_add_to_z(i);
      window_toTop(_window_handle(i));
      kernel_unlock(&windowTable.lock);
      return _window_handle(i);
    }
  }

  kernel_unlock(&windowTable.lock);
  return 0;
}
 
static void
_window_close(window_h window)
{
  window_t* entry = &windowTable.windows[_window_index(window)];
  gfx_releaseFrameBuffer(entry->frameBuffer);
  entry->state = WINDOW_DEAD;
  _window_remove_from_z(window);
  window_fullRefresh = 1;
}

void 
window_close(window_h window)
{
  window_t* entry = &windowTable.windows[_window_index(window)];
  gfx_releaseFrameBuffer(entry->frameBuffer);
  kernel_spinLock(&windowTable.lock);
  entry->state = WINDOW_DEAD;
  _window_remove_from_z(window);
  kernel_unlock(&windowTable.lock);
  window_fullRefresh = 1;
}

unsigned 
window_getW(window_h window)
{
  return windowTable.windows[_window_index(window)].w;
}

unsigned 
window_getCursorX(window_h window)
{
  return windowTable.windows[_window_index(window)].cursorX;
}

unsigned 
window_getCursorY(window_h window)
{
  return windowTable.windows[_window_index(window)].cursorY;
}

void 
window_setCursor(window_h window, unsigned x, unsigned y)
{
  windowTable.windows[_window_index(window)].cursorX = x;
  windowTable.windows[_window_index(window)].cursorY = y;
  gfx_setFrameFrameBufferDirty(windowTable.windows[_window_index(window)].frameBuffer);
}

void 
window_setCursorX(window_h window, unsigned x)
{
  windowTable.windows[_window_index(window)].cursorX = x;
  gfx_setFrameFrameBufferDirty(windowTable.windows[_window_index(window)].frameBuffer);
}

void 
window_setCursorY(window_h window, unsigned y)
{
  windowTable.windows[_window_index(window)].cursorY = y;
  gfx_setFrameFrameBufferDirty(windowTable.windows[_window_index(window)].frameBuffer);
}

unsigned 
window_getH(window_h window)
{
  return windowTable.windows[_window_index(window)].h;
}

int 
window_getFrameBuffer(window_h window)
{
  return windowTable.windows[_window_index(window)].frameBuffer;
}

int 
window_getZ(window_h window)
{
  int index = _window_index(window);
  for (int i = 0; i < WINDOWS_MAX; i++) {
    if (windowTable.zIndex[i] == index) {
      return i;
    }
  }
  return -1;
}

unsigned 
window_getColor(window_h window)
{
  return windowColor;
}

unsigned 
window_getBackgroundColor(window_h window)
{
  return windowBackgroundColor;
}

void 
window_enableCursor(window_h window, unsigned enabled)
{
  windowTable.windows[_window_index(window)].cursorOn = enabled;
}
