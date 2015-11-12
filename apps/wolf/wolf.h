//#define FLOOR

#include "gfx.h"

#define abs(x) (x > 0 ? x : -x)
typedef unsigned long Uint32;
typedef unsigned long ColorRGB;
#define RGB_Red 0xFFFF0000
#define RGB_Green 0xFF00FF00
#define RGB_Blue 0xFF0000FF
#define RGB_White 0xFFFFFFFF
#define RGB_Yellow 0xFFFFFF00

#define SDLK_UP 38
#define SDLK_DOWN 40
#define SDLK_RIGHT 39
#define SDLK_LEFT 37

//const int w = 312;
//const int h = 154;

const int w = 320;
const int h = 200;
//const int scale = 1;

//const int w = 320/2;
//const int h = 200/2;
//const int scale = 4;

/*
extern void verLine(unsigned short x, unsigned short y1, unsigned short y2, ColorRGB color) {
  gfx_drawLine(x, y1, x, y2, (unsigned long)color);
  }*/

inline double getTicks() {
  //  return simulator_elapsedMilliSeconds();
  return peripheral.time.elapsedMilliSeconds;
}

inline bool keyDown(window_h w, int key) {
  return window_isKeyDown(w, key);
}

typedef struct {
   unsigned long data[h][w];
} vb_t;

volatile vb_t* vb = (vb_t*)&videoRam;

//#define drawRGBA(x, y, data)   gfx_putData(x, y, data)
//#define drawRGBA(x, y, data)   gfx_fputData(x, y, data)
//#define drawRGBA(x, y, data)   gfx_iPutData(x, y, data)
//#define drawRGBA(x, y, byte) *((&videoRam.data) + ((y * videoWidth) + x)) = byte;
#define drawRGBA(x, y, d) (vb->data[y][x] = d)


inline unsigned int TexY(int y, unsigned int lineHeight )
{
  unsigned int d = (y<<8) - (h * 128) + (lineHeight * 128); //256 and 128 factors to avoid floats
  return ((d * texHeight) / lineHeight) >>8;
}
