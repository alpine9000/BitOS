#include <sys/dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "gfx.h"
#include "console.h"
#include "peripheral.h"
#include "kernel.h"
#include "audio.h"
#include "window.h"
#include "si.h"
#include "gfx.h"

#define NUM_INVADERS 6
#define NUM_MISSLES 3

static const unsigned char invaderW = 12;
static const unsigned char invaderH = 8;
static const unsigned char missleW = 3;
static const unsigned char missleH = 7;

typedef struct {
  short x;
  short y;
  unsigned char sprite;
  unsigned char state;
} invader_t;

invader_t invaders[55];
invader_t missle = {0, 0, 0, 0};

unsigned char invaderIndex = 0;
char invaderDirection = 1;
unsigned short invaderSpeed = 500;
unsigned short missleSpeed = 1;

unsigned target, work, sprite, width, height;

void initInvader(short x, short y, unsigned char _sprite)
{
  invader_t *i = &  invaders[invaderIndex++];
  i->x = x;
  i->y = y;
  i->sprite = _sprite;
  i->state = 0;
}

void initInvaders()
{
  unsigned char sprites[] = {2, 0, 0, 1, 1};
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 11; x++) {
      initInvader(20+(x * 16), 40+(y * 16), sprites[y]);
    } 
  }
}

void downShiftInvaders()
{
  invaderSpeed = invaderSpeed - 50;

  for (int i = 0; i < invaderIndex; i++) {
    invader_t *inv = &invaders[i];
    inv->y += invaderH;
  }
}

void moveMissle()
{
  if (missle.state == 1) {
    missle.y-=4;
  }
  if (missle.y <= 10) {
   missle.state = 0;
  }
}

void moveInvaders(unsigned time) 
{
  static unsigned last = 0;
  static unsigned beats[] = {1, 2, 3, 4};
  static unsigned char beatIndex = 0;

  if (time-last > invaderSpeed) {
    last = time;
    audio_execute(beats[beatIndex++]);
    if (beatIndex > 3) {
      beatIndex = 0;
    }

    for (int i = 0; i < invaderIndex; i++) {
      invader_t *inv = &invaders[i];
      inv->x += invaderDirection;
      inv->state = !inv->state;
    }
    
    for (int i = 0; i < invaderIndex; i++) {
      invader_t *inv = &invaders[i];
      if (inv->x > (short)width-20) {
	invaderDirection = -1;
	downShiftInvaders();
	break;
      }
    }
    
    for (int i = 0; i < invaderIndex; i++) {
      invader_t *inv = &invaders[i];
      if (inv->x < 10) {
	invaderDirection = 1;
	downShiftInvaders();
	break;
      }
    }
  }
}

void renderMissle()
{
  if (missle.state == 1) {
    gfx_bitBlt(work, missle.sprite*missleW, invaderH*NUM_INVADERS, missle.x, missle.y, missleW, missleH, sprite);
  }
}


void drawRGBA(unsigned short x, unsigned short y, unsigned *ptr)
{
  unsigned data = (unsigned)(*ptr);
  unsigned char alpha = data & 0xFF;
  data = (data >> 8) | alpha << 24;
  gfx_drawPixel(sprite, x, y, data);
}

void preRender()
{
  //  gfx_setFrameBuffer(OFFSCREEN2);
  unsigned *ptr = (unsigned*) &sprite_rgba;
  unsigned short x, y;

  unsigned short _height = (invaderH*NUM_INVADERS)+missleH;

  for (y = 0; y < _height; y++) {
    for (x = 0; x < invaderW; x++) {
      drawRGBA(x, y, ptr++);
    }
  }

  //gfx_setFrameBuffer(OFFSCREEN1);
  gfx_fillRect(work, 0, 0, width, _height, 0xFF000000);
  gfx_drawStringRetro(work, 5, 1, "SCORE<1> HI-SCORE SCORE<2>", 0xFFFFFFFF, 1, 3);

}

static unsigned short x = 0, y = 200;

void render()
{
  //  static unsigned beats[] = {60000, 55000, 50000, 45000};


  //gfx_setFrameBuffer(OFFSCREEN1);
  gfx_fillRect(work, 0, 10, width, height-10, 0xFF000000);
  gfx_bitBlt(work, 0, 0, x, y, invaderW, invaderH, sprite);

  for (int i = 0; i < invaderIndex; i++) {
    invader_t *inv = & invaders[i];    
    gfx_bitBlt(work, 0, ((inv->sprite*2)+inv->state)*invaderH, inv->x, inv->y,invaderW, invaderH, sprite);
  }

  renderMissle();

  gfx_bitBlt(target, 0, 0, 0, 0, width, height, work);

  //  gfx_setFrameBuffer(ONSCREEN);
}

void setupAudio()
{
  audio_selectChannel(0);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&shoot_wav);
  audio_setLength(shoot_wav_length);

  audio_selectChannel(1);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&fastinvader1_wav);
  audio_setLength(fastinvader1_wav_length);

  audio_selectChannel(2);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&fastinvader2_wav);
  audio_setLength(fastinvader2_wav_length);

  audio_selectChannel(3);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&fastinvader3_wav);
  audio_setLength(fastinvader3_wav_length);

  audio_selectChannel(4);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&fastinvader4_wav);
  audio_setLength(fastinvader4_wav_length);
  }

void shoot()
{
  if (missle.state == 0) {
    missle.x = x + (invaderW/2);
    missle.y = y-missleH;
    missle.sprite = 0;
    missle.state = 1;
    audio_execute(0);
  }
}



int main(int agrc, char* argv[])
{
  unsigned time;
  unsigned w = 217, h = 248;
  window_h window = window_create("Space Invaders", 0, 0, w, h);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, w, h, 0xFFFFFFFF);
  kernel_setThreadWindow(window);

  width = w;
  height = h;
  target = window_getFrameBuffer(window);
  work = gfx_createFrameBuffer(w, h);
  sprite = gfx_createFrameBuffer(w, h);
  //gfx_setVideoResolution(1, 217, 248, 2);
  //gfx_setVideoResolution(2, 217, 248, 2);
  initInvaders();
  setupAudio();
  preRender();

  for (;;) {


    time = peripheral.time.elapsedMilliSeconds;
    
    if (_console_char_avail()) {
      //if (peripheral.consoleReadBufferSize) {
      switch ( _console_read_char()) {
	//switch (peripheral.consoleRead) {
      case ' ':
	shoot();
	break;
      }
    }
  

    if (window_isKeyDown(window, 37)) {
      x-=2;
      if (x > width-invaderW) {
	x = 0;
      }
    }
    if (window_isKeyDown(window, 39)) {
      x+=2;
      if (x >= width-invaderW) {
	x = width-invaderW-1;
      }
      }
    render();
    moveInvaders(time);
    moveMissle();


    kernel_threadBlocked();
  }
}
