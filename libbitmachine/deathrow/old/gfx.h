#ifndef __GFX_H
#define __GFX_H

#include "peripheral.h"

enum {
  ONSCREEN = 0,
  OFFSCREEN1 = 1,
  OFFSCREEN2 = 2
};

#define gfx_fontWidth 5  
#define gfx_fontHeight 8

void gfx_setFrameBuffer(unsigned int frameBuffer);
void gfx_clearScreen(unsigned long color);
void gfx_fillRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned long color);
void gfx_drawPixel(int x, int y, unsigned long byte);
void gfx_drawLine(int x0, int y0, int x1, int y1, unsigned int color);
void gfx_drawStringEx(unsigned int x, unsigned int y, char *c, unsigned long color, unsigned char size, unsigned char spaceSize);
void gfx_drawString(unsigned int x, unsigned int y, char *c, unsigned long color);
void gfx_bitBlt(short srcX, short srcY, short destX, short destY, unsigned short width, unsigned short height, unsigned short src, unsigned short dest);
void gfx_setFrameBuffer(unsigned int frameBuffer);
void gfx_vsync();
void gfx_setVideoResolution(unsigned short x, unsigned short y, unsigned long scaling);

void gfx_putData(int x, int y, unsigned long byte);
void gfx_fputData(int x, int y, unsigned long byte);
void gfx_saveData();
void gfx_loadData();

inline void gfx_iPutData(int x, int y, unsigned long byte)
{
  peripheral.videoAddressX = x;
  peripheral.videoAddressY = y;
  peripheral.videoData = byte;
}

extern unsigned int videoWidth;
extern unsigned int videoHeight;

#endif
