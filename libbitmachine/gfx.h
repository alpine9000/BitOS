#pragma once

#include "peripheral.h"

enum {
  ONSCREEN = 0,
  OFFSCREEN1 = 1,
  OFFSCREEN2 = 2,
  OFFSCREEN3 = 3
};

#define gfx_retroFontWidth 5  
#define gfx_retroFontHeight 8
#define gfx_retroSpaceWidth 1

#define gfx_fontWidth 7
#define gfx_fontHeight 14
#define gfx_spaceWidth 0

#ifdef _KERNEL_BUILD

unsigned 
gfx_createFrameBuffer(unsigned w, unsigned h);

unsigned 
gfx_createFrameBufferFromData(unsigned w, unsigned h, unsigned* ptr);

void 
gfx_clearScreen(unsigned fb, unsigned color);

void 
gfx_fillRect(unsigned fb, int x, int y, unsigned width, unsigned height, unsigned color);

void 
gfx_drawPixel(unsigned fb, int x, int y, unsigned byte);

void 
gfx_drawRGBA(unsigned fb, unsigned short x, unsigned short y, unsigned *ptr);

void 
gfx_drawLine(unsigned fb, int x0, int y0, int x1, int y1, unsigned color);

void 
gfx_drawStringEx(unsigned fb, int  x, int y, char *c, unsigned color, int size, int spaceSize);

void 
gfx_drawString(unsigned fb,int x,int y, char *c, unsigned color);

void 
gfx_drawStringRetro(unsigned fb, int  x, int y, char *c, unsigned color, int size, int spaceSize);

void 
gfx_bitBlt(unsigned fb, short srcX, short srcY, short destX, short destY, unsigned short width, unsigned short height, unsigned src);

void 
gfx_bitBltEx(unsigned fb, short sx, short sy, short dx, short dy, unsigned short sw, unsigned short sh, unsigned short dw, unsigned short dh, unsigned src);

void 
gfx_vsync();

void 
gfx_setVideoResolution(unsigned fb, unsigned short x, unsigned short y, unsigned scaling);

void 
gfx_putData(unsigned fb, int x, int y, unsigned byte);

void 
gfx_fputData(int x, int y, unsigned byte);

void 
gfx_saveData(unsigned fb);

void 
gfx_loadData(unsigned fb);

void 
gfx_alpha(unsigned fb, unsigned alpha);

void 
gfx_releaseFrameBuffer(unsigned fb);

void 
gfx_init();

unsigned 
gfx_frameFrameBufferDirty(unsigned fb);

void 
gfx_setFrameFrameBufferDirty(unsigned fb);

unsigned 
gfx_getVideoWidth();

unsigned 
gfx_getVideoHeight();

#else

#define gfx_fillRect(fb,x,y,width,height,color) _bft->gfx_fillRect(fb,x,y,width,height,color) 
#define gfx_drawPixel(fb,x,y,color) _bft->gfx_drawPixel(fb,x,y,color)
#define gfx_bitBlt(fb,srcX,srcY,destX,destY,width,height,src)  _bft->gfx_bitBlt(fb, srcX, srcY, destX, destY, width, height, src)
#define gfx_bitBltEx(fb,sx,sy,dx,dy,sw,sh,dw,dh,src) _bft->gfx_bitBltEx(fb,sx,sy,dx,dy,sw,sh,dw,dh,src)
#define gfx_drawStringEx(fb, x, y, c, color, size, spaceSize)  _bft->gfx_drawStringEx(fb, x, y, c,  color, size, spaceSize)
#define gfx_createFrameBuffer(w,h) _bft->gfx_createFrameBuffer(w, h)
#define gfx_loadData(fb) _bft->gfx_loadData(fb)
#define gfx_saveData(fb) _bft->gfx_saveData(fb)
#define gfx_getVideoWidth() _bft->gfx_getVideoWidth()
#define gfx_getVideoHeight() _bft->gfx_getVideoHeight()
#define gfx_drawStringRetro(fb, x, y, c, color, size, spaceSize)  _bft->gfx_drawStringRetro(fb, x, y, c,  color, size, spaceSize)
#define gfx_drawLine(fb, x0, y0, x1, y1, color) _bft->gfx_drawLine(fb, x0, y0, x1, y1, color)
#endif

inline void gfx_iPutData(unsigned fb, int x, int y, unsigned byte)
{
  peripheral.video[fb].videoAddressX = x;
  peripheral.video[fb].videoAddressY = y;
  peripheral.video[fb].videoData = byte;
}

extern unsigned videoWidth;
extern unsigned videoHeight;

