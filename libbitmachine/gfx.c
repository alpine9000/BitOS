#include "gfx.h"
#include "peripheral.h"
#include "kernel.h"
#include "simulator.h"
#include "panic.h"

static unsigned frameBufferAllocator_lock ;
static thread_h frameBufferAllocator[PERIPHERAL_MAX_FRAMEBUFFERS];
static int frameBufferDirty[PERIPHERAL_MAX_FRAMEBUFFERS];

//unsigned int videoWidth = 640;
//unsigned int videoHeight = 480;

unsigned int videoWidth = 1140;
unsigned int videoHeight = 768;
extern  unsigned char  font[];

#define dirty() frameBufferDirty[fb] = 1;

void 
gfx_init()
{
  frameBufferAllocator[0] = 0; // 0 is screen
  for (int i = 1; i < PERIPHERAL_MAX_FRAMEBUFFERS; i++) {
    frameBufferAllocator[i] = INVALID_THREAD;
  }
}

unsigned 
gfx_frameFrameBufferDirty(unsigned fb)
{
  unsigned dirty = frameBufferDirty[fb];
  frameBufferDirty[fb] = 0;
  return dirty;
}

void 
gfx_setFrameFrameBufferDirty(unsigned fb)
{
  frameBufferDirty[fb] = 1;
}

void 
gfx_setVideoResolution(unsigned fb, unsigned short x, unsigned short y, unsigned scaling)
{
  if (fb == 0) {
    videoWidth = x;
    videoHeight = y;
  }
  peripheral.videoScaling = scaling;
  peripheral.video[fb].videoResolution = ((x & 0xFFFF) << 16) | (y & 0xFFFF);
}

unsigned 
gfx_createFrameBuffer(unsigned w, unsigned h)
{
  kernel_spinLock(&frameBufferAllocator_lock);
  
  unsigned fb = PERIPHERAL_MAX_FRAMEBUFFERS;
  for (int i = 1; i < PERIPHERAL_MAX_FRAMEBUFFERS; i++) {
    if (frameBufferAllocator[i] == INVALID_THREAD) {
      frameBufferAllocator[i] = kernel_getTid();
      fb = i;
      break;
    }
  }
  
  if (fb >= PERIPHERAL_MAX_FRAMEBUFFERS) {
    panic("gfx_createFrameBuffer - frame buffers exhausted\n");
  }
  kernel_unlock(&frameBufferAllocator_lock);
  gfx_setVideoResolution(fb, w, h, 1);
  dirty();
  return fb;
}

void 
gfx_releaseFrameBuffer(unsigned fb)
{
  kernel_spinLock(&frameBufferAllocator_lock);
  frameBufferAllocator[fb] = INVALID_THREAD;
  kernel_unlock(&frameBufferAllocator_lock);
}

void 
gfx_clearScreen(unsigned fb, unsigned color)
{
  dirty();
  gfx_fillRect(fb, 0, 0, videoWidth, videoHeight, color);
}

void 
gfx_drawPixel(unsigned fb, int x, int y, unsigned color)
{
  dirty();
  peripheral.video[fb].videoAddressX = x;
  peripheral.video[fb].videoAddressY = y;
  peripheral.video[fb].videoPixel = color;
}

void 
gfx_drawRGBA(unsigned fb, unsigned short x, unsigned short y, unsigned *ptr)
{
  dirty();
  unsigned data = (unsigned)(*ptr);
  unsigned char alpha = data & 0xFF;
  data = (data >> 8) | alpha << 24;
  gfx_drawPixel(fb, x, y, data);
}

unsigned 
gfx_createFrameBufferFromData(unsigned w, unsigned h, unsigned* ptr)
{
  unsigned short x, y;
  unsigned fb = gfx_createFrameBuffer(w, h);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      gfx_drawRGBA(fb, x, y, ptr++);
    }
  }
  dirty();

  return fb;
}

void 
gfx_putData(unsigned fb, int x, int y, unsigned byte)
{
  peripheral.video[fb].videoAddressX = x;
  peripheral.video[fb].videoAddressY = y;
  peripheral.video[fb].videoData = byte;
}

void 
gfx_fputData(int x, int y, unsigned byte)
{
  *((&videoRam.data) + ((y * videoWidth) + x)) = byte;
 
}

void 
gfx_saveData(unsigned fb)
{
  peripheral.video[fb].videoSaveData = 1;
}

void 
gfx_loadData(unsigned fb)
{
  peripheral.video[fb].videoLoadData = 1;
}

void 
gfx_fillRect(unsigned fb, int x, int y, unsigned width, unsigned height, unsigned color)
{
  dirty();
  peripheral.video[fb].videoAddressX = x;
  peripheral.video[fb].videoAddressY = y;
  peripheral.video[fb].videoWidth = width;
  peripheral.video[fb].videoHeight = height;
  peripheral.video[fb].videoRect = color;
}

void 
gfx_bitBltEx(unsigned fb, short sx, short sy, short dx, short dy, unsigned short sw, unsigned short sh, unsigned short dw, unsigned short dh, unsigned src)
{
  dirty();
  peripheral.video[fb].blt.srcOrigin = ((sx & 0xFFFF) << 16) | (sy & 0xFFFF);
  peripheral.video[fb].blt.destOrigin = ((dx & 0xFFFF) << 16) | (dy & 0xFFFF);
  peripheral.video[fb].blt.srcSize = (sw << 16) | sh;
  peripheral.video[fb].blt.destSize = (dw << 16) | dh;
  peripheral.video[fb].blt.bltFromSrcWithSize = src;
}

void 
gfx_bitBlt(unsigned fb, short srcX, short srcY, short destX, short destY, unsigned short width, unsigned short height, unsigned src)
{
  dirty();
  peripheral.video[fb].blt.srcOrigin = ((srcX & 0xFFFF) << 16) | (srcY & 0xFFFF);
  peripheral.video[fb].blt.destOrigin = ((destX & 0xFFFF) << 16) | (destY & 0xFFFF);
  peripheral.video[fb].blt.srcSize = (width << 16) | height;
  peripheral.video[fb].blt.bltFromSrcWithScaling = src;
}

// bresenham's algorithm - thx wikpedia
#define swap(a, b) { int t = a; a = b; b = t; }
#define abs(x) (x > 0 ? x : -x)
void 
gfx_drawLine(unsigned fb, int x0, int y0, int x1, int y1, unsigned color) {
  unsigned steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  unsigned dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int err = dx / 2;
  int ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      gfx_drawPixel(fb, y0, x0, color);
    } else {
      gfx_drawPixel(fb, x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
  dirty();
}

// draw a character

void 
gfx_drawCharRetro(unsigned fb, int x, int y, char c, unsigned color, unsigned char size) {
  for (unsigned char i =0; i<gfx_retroFontWidth; i++ ) {
    unsigned char line = font[(c*gfx_retroFontWidth)+i];
    for (unsigned char j = 0; j<gfx_retroFontHeight; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  gfx_drawPixel(fb, x+i, y+j, 0xFF000000|color);
	else {  // big size
	  gfx_fillRect(fb, x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}

void 
gfx_drawChar(unsigned fb, int x, int y, char c, unsigned color, unsigned char size) {
  kernel_spinLock(&frameBufferAllocator_lock);
  peripheral.pcg.fb = fb;
  peripheral.pcg.x = x;
  peripheral.pcg.y = y;
  char text[2] = {c,0};
  peripheral.pcg.color = color;
  peripheral.pcg.text = (unsigned)text;
  kernel_unlock(&frameBufferAllocator_lock);
  dirty();
}


void 
gfx_drawString(unsigned fb, int x,  int y, char *c, unsigned  color) {
  gfx_drawStringEx(fb, x, y, c, color, 1, gfx_spaceWidth);
}

void 
gfx_drawStringEx(unsigned fb, int  x, int y, char *c, unsigned color, int size, int spaceSize)
{
  while (c[0] != 0) {
    gfx_drawChar(fb, x, y, c[0], color, size);
    x += size*(gfx_fontWidth+spaceSize);
    c++;
    /*if (x + 5 >= videoWidth) {
      y += 10;
      x = 0;
      }*/
  }
}

void 
gfx_drawStringRetro(unsigned fb, int  x, int y, char *c, unsigned color, int size, int spaceSize)
{
  while (c[0] != 0) {
    gfx_drawCharRetro(fb, x, y, c[0], color, size);
    x += size*(gfx_retroFontWidth+spaceSize);
    c++;
  }
}

void 
gfx_vsync() {
  //kernel_vsync();

  //extern unsigned currentThread;
  //  peripheral.simulator.currentThread = 100+currentThread;
  //  simulator_yield();
  
  kernel_threadBlocked();
}

void 
gfx_alpha(unsigned fb, unsigned alpha) {
    peripheral.video[fb].alpha = alpha;
}

unsigned 
gfx_getVideoWidth()
{
  return videoWidth;
}

unsigned 
gfx_getVideoHeight()
{
  return videoHeight;
}
