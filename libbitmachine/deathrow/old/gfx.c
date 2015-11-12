#include "gfx.h"
#include "peripheral.h"
#include <stdlib.h>

unsigned int videoWidth = 320;
unsigned int videoHeight = 240;
extern  unsigned char  font[];


void gfx_setFrameBuffer(unsigned int frameBuffer)
{
  peripheral.videoFrameBuffer = frameBuffer;
}

void gfx_clearScreen(unsigned long color)
{
  gfx_fillRect(0, 0, videoWidth, videoHeight, color);
}

void gfx_drawPixel(int x, int y, unsigned long byte)
{
  peripheral.videoAddressX = x;
  peripheral.videoAddressY = y;
  peripheral.videoPixel = byte;
}

void gfx_putData(int x, int y, unsigned long byte)
{
  peripheral.videoAddressX = x;
  peripheral.videoAddressY = y;
  peripheral.videoData = byte;
}

void gfx_fputData(int x, int y, unsigned long byte)
{
  *((&videoRam.data) + ((y * videoWidth) + x)) = byte;
 
}

void gfx_saveData()
{
  peripheral.videoSaveData = 1;
}

void gfx_loadData()
{
  peripheral.videoLoadData = 1;
}

void gfx_fillRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned long color)
{
  /*  for (unsigned _x = x; _x < (x+width); ++_x) {
    for (unsigned _y = y; _y < (y+height); ++_y) {
      drawPixel(_x, _y, color);
    }
    }*/


  peripheral.videoAddressX = x;
  peripheral.videoAddressY = y;
  peripheral.videoWidth = width;
  peripheral.videoHeight = height;
  peripheral.videoRect = color;
}

void gfx_bitBlt(short srcX, short srcY, short destX, short destY, unsigned short width, unsigned short height, unsigned short src, unsigned short dest)
{
  peripheral.bltSrc = ((srcX & 0xFFFF) << 16) | (srcY & 0xFFFF);
  peripheral.bltDest = ((destX & 0xFFFF) << 16) | (destY & 0xFFFF);
  peripheral.bltSize = (width << 16) | height;
  peripheral.bltFrameBuffers = (src << 16) | dest;
}

// bresenham's algorithm - thx wikpedia
#define swap(a, b) { int t = a; a = b; b = t; }
void gfx_drawLine(int x0, int y0, int x1, int y1, unsigned int color) {
  unsigned long steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  unsigned long dx, dy;
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
      gfx_drawPixel(y0, x0, color);
    } else {
      gfx_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// draw a character
void gfx_drawChar(unsigned long x, unsigned long y, char c, unsigned long color, unsigned char size) {
  for (unsigned char i =0; i<5; i++ ) {
    unsigned char line = font[(c*5)+i];
    for (unsigned char j = 0; j<8; j++) {
      if (line & 0x1) {
	if (size == 1) // default size
	  gfx_drawPixel(x+i, y+j, 0xFF000000|color);
	else {  // big size
	  gfx_fillRect(x+i*size, y+j*size, size, size, color);
	} 
      }
      line >>= 1;
    }
  }
}

  void gfx_drawString(unsigned int x, unsigned int y, char *c, unsigned long color) {
  gfx_drawStringEx(x, y, c, color, 1, 1);
}

void gfx_drawStringEx(unsigned int x, unsigned int y, char *c, unsigned long color, unsigned char size, unsigned char spaceSize)
{
  while (c[0] != 0) {
    gfx_drawChar(x, y, c[0], color, size);
    x += size*(5+spaceSize);
    c++;
    if (x + 5 >= videoWidth) {
      y += 10;
      x = 0;
    }
  }
}

void gfx_vsync() {
  while (!peripheral.vblank);
}

void gfx_setVideoResolution(unsigned short x, unsigned short y, unsigned long scaling)
{
  videoWidth = x;
  videoHeight = y;
  peripheral.videoScaling = scaling;
  peripheral.videoResolution = ((x & 0xFFFF) << 16) | (y & 0xFFFF);
}
