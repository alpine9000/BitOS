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

#define SCREEN_WIDTH 217
#define SCREEN_HEIGHT 248

#define printf _bft->simulator_printf

typedef enum {
  ALIVE = 1,
  EXPLODING = 2,
  DEAD = 0
} actor_state_t;

typedef enum {
  AUDIO_CHANNEL_SHOOT = 0,
  AUDIO_CHANNEL_INVADER1 = 1,
  AUDIO_CHANNEL_INVADER2 = 2,
  AUDIO_CHANNEL_INVADER3 = 3,
  AUDIO_CHANNEL_INVADER4 = 4,
  AUDIO_CHANNEL_KILLED = 5
} audio_channel_t;


typedef struct {
  int width;
  int height;
  int count;
} sprite_t;


typedef struct {
  int x;
  int y;
  int sprite;
  int spriteIndex;
  actor_state_t _state;
  unsigned data;
} actor_t;

#define SCORE_X   23
#define SCORE_Y   17
#define HISCORE_X 87
#define scoreBoardHeight 24

#define NUM_DEFENDERS_X 7
#define NUM_DEFENDERS_Y 233
#define NUM_BASES 4

#define invaderW 12
#define invaderH 8
#define missileW 3
#define missileH 7
#define defenderW 13
#define defenderH 8
#define baseW 22
#define baseH 16
#define baseTop 184

static actor_t invaders[55];

static actor_t missile = {0, 0, 5, 0, DEAD, 0};

static actor_t defender = {0, 208, 4, 0, ALIVE, 0};

static actor_t spareDefenders[] = {
  {24, 232, 4, 0, ALIVE, 0},
  {40, 232, 4, 0, ALIVE, 0}
};

static actor_t bases[NUM_BASES] = {
  {30, baseTop, 6, 0, ALIVE, 0},
  {75, baseTop, 6, 1, ALIVE, 0},
  {120, baseTop, 6, 2, ALIVE, 0},
  {165, baseTop, 6, 3, ALIVE, 0}
};

int spriteMapWidth = 128;
int spriteMapHeight = 64;

static sprite_t spriteConfig[] = { 
  {invaderW, invaderH, 3},
  {invaderW, invaderH, 3},
  {invaderW, invaderH, 3},
  {missileW, missileH, 3},
  {defenderW,defenderH, 1},
  {1, 4, 2}, // Defender Missile
  {baseW, baseH, 4},
  {0, 0, 0}
};

static int invaderIndex = 0;
static int invaderDirection = 1;
static int invaderSpeed = 500;

static int score = 0;
static int hiscore = 0;
static unsigned frame = 0;
static int screenDirty = 1;
static int numDefenders = 3;

static int target, work, width, height, spriteFrameBuffer;

static void 
initInvader(int x, int y, unsigned _sprite)
{
  actor_t *i = &  invaders[invaderIndex++];
  i->x = x;
  i->y = y;
  i->sprite = _sprite;
  i->spriteIndex = 0;
  i->_state = ALIVE;
}


static void 
initInvaders()
{
  unsigned char sprites[] = {2, 0, 0, 1, 1};
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 11; x++) {
      initInvader(20+(x * 16), 40+(y * 16), sprites[y]);
    } 
  }
}

static void 
drawSpriteRGBA(unsigned x, unsigned y, unsigned *ptr)
{
  unsigned data = (unsigned)(*ptr);
  unsigned char alpha = data & 0xFF;
  data = (data >> 8) | alpha << 24;
  gfx_drawPixel(spriteFrameBuffer, x, y, data);
}

static void
transferSprite(int sprite, int index)
{
  unsigned *ptr = (unsigned*) &sprite_rgba;
  sprite_t* sp = &spriteConfig[sprite];
  int sy = sp->height*index;  
  int sx = 0;

  for (int i = 0; i < sprite; i++) {
    sx += spriteConfig[i].width;
  }
  
  for (int y = sy; y < (sy+sp->height); y++) {
    for (int x = sx; x < (sx+sp->width); x++) {
      unsigned* dp = ptr + (y*spriteMapWidth) + x;
      drawSpriteRGBA(x, y, dp);
    }
  }
}

//static 
void
putSpritePixelRGBA(int sprite, int index, int x, int y, unsigned pixel)
{
 unsigned *ptr = (unsigned*) &sprite_rgba;
  sprite_t* sp = &spriteConfig[sprite];
  int sy = sp->height*index;  
  int sx = 0;

  for (int i = 0; i < sprite; i++) {
    sx += spriteConfig[i].width;
  }
  
  unsigned* dp = ptr + ((sy+y)*spriteMapWidth) + (sx+x);
  *dp = pixel;
}

static unsigned
getSpritePixelRGBA(int sprite, int index, int x, int y)
{
  sprite_t* sp = &spriteConfig[sprite];

  if (!(x > 0 && x < sp->width && y > 0 && y < sp->height)) {
    return 0;
  }

  unsigned *ptr = (unsigned*) &sprite_rgba;
  int sy = sp->height*index;  
  int sx = 0;

  for (int i = 0; i < sprite; i++) {
    sx += spriteConfig[i].width;
  }
  
  unsigned* dp = ptr + ((sy+y)*spriteMapWidth) + (sx+x);
  return *dp;
}

static void 
initRender()
{
  for (int sprite = 0; spriteConfig[sprite].count != 0; sprite++) {
    for (int index = 0; index < spriteConfig[sprite].count; index++) {
      transferSprite(sprite, index);
    }
  }

  gfx_fillRect(work, 0, 0, width, height, 0xFF000000);
  gfx_drawStringRetro(work, 5, 1, "SCORE<1> HI-SCORE SCORE<2>", 0xFFFFFFFF, 1, 3);  
}


static void 
initAudio()
{
  audio_selectChannel(AUDIO_CHANNEL_SHOOT);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&shoot_wav);
  audio_setLength(shoot_wav_length);

  audio_selectChannel(AUDIO_CHANNEL_INVADER1);
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

  audio_selectChannel(AUDIO_CHANNEL_KILLED);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&invaderkilled_wav);
  audio_setLength(invaderkilled_wav_length);
}


static void 
downShiftInvaders()
{
  invaderSpeed = invaderSpeed - 50;

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = &invaders[i];
    inv->y += invaderH;
  }
}


static void
moveMissile()
{
  if (missile._state == ALIVE) {
    missile.y-=4;
    if ((missile.y+missileH) < baseTop) {
      missile.spriteIndex = 1;
    }
    screenDirty = 1;
  }

  if (missile.y <= scoreBoardHeight) {
    missile._state = DEAD;
  }
}


static void
moveInvaders(int time) 
{
  static int last = 0;
  static unsigned beats[] = {AUDIO_CHANNEL_INVADER1, AUDIO_CHANNEL_INVADER2, AUDIO_CHANNEL_INVADER3, AUDIO_CHANNEL_INVADER4};
  static unsigned char beatIndex = 0;

  if (time-last > invaderSpeed) {
    last = time;
    audio_execute(beats[beatIndex++]);
    if (beatIndex > 3) {
      beatIndex = 0;
    }

    for (int i = 0; i < invaderIndex; i++) {
      actor_t *inv = &invaders[i];
      if (inv->_state == ALIVE) {
	inv->x += invaderDirection;
	inv->spriteIndex = !inv->spriteIndex;
      } else if (inv->_state == EXPLODING) {
	inv->_state = DEAD;
      }
    }
    
    for (int i = 0; i < invaderIndex; i++) {
      actor_t *inv = &invaders[i];
      if (inv->_state == ALIVE) {
	if (inv->x > width-20) {
	  invaderDirection = -1;
	  downShiftInvaders();
	  break;
	}
      }
    }
    
    for (int i = 0; i < invaderIndex; i++) {
      actor_t *inv = &invaders[i];
      if (inv->_state == ALIVE) {
	if (inv->x < 10) {
	  invaderDirection = 1;
	  downShiftInvaders();
	  break;
	}
      }
    }

    screenDirty = 1;
  }
}



static void
renderActor(actor_t* actor)
{
  sprite_t *s = &spriteConfig[actor->sprite];
  
  int sx = 0;
  for (int i = 0; i < actor->sprite; i++) {
    sx += spriteConfig[i].width;
  }
  int sy = s->height*actor->spriteIndex;
  gfx_bitBlt(work, sx, sy, actor->x, actor->y, s->width, s->height, spriteFrameBuffer);
}

static void
renderBases()
{
  for (unsigned i = 0; i < NUM_BASES; i++) {
    renderActor(&bases[i]);
  }
}

static void
renderStatusBar(int w, int h)
{
  gfx_drawLine(work, 0, 231, w, 231, 0xFF00FF00);
  gfx_fillRect(work, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, gfx_retroFontWidth, gfx_retroFontHeight, 0xFF000000);
  char buffer[2] = {'0' + numDefenders, 0};
  gfx_drawStringRetro(work, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, buffer , 0xFFFFFFFF, 1, 0);    

  for (int i = 0; i < numDefenders; i++) {
    renderActor(&spareDefenders[i]);
  }

  gfx_drawStringRetro(work, 135, NUM_DEFENDERS_Y, "CREDIT 00", 0xFFFFFFFF, 1, 3);    

}

static void 
renderMissile()
{
  if (missile._state == ALIVE) {
    renderActor(&missile);
    /*if (frame % 10 == 0) {
      missile.spriteIndex++;
      missile.spriteIndex = missile.spriteIndex % spriteConfig[missile.sprite].count;
      }*/
  }
}


static char *
scoretoa(int i, int bufsize, char* buf)
{
  buf[bufsize-1] = 0;
  char *p = buf + bufsize-1;
  do {
    *--p = '0' + (i % 10);
    i /= 10;
  } while (i != 0);
  do {
    *--p = '0';
  } while (p != buf);

  return buf;
}


static void 
renderScores()
{
  static char scoreBuffer[5];
  static char hiScoreBuffer[5];
  static int lastScore = -1;
  static int lastHiScore = -1;
  int dirty = 0;

  if (lastScore != score) {
    scoretoa(score, 5, scoreBuffer);
    lastScore = score;
    dirty = 1;
  }

  if (lastHiScore != hiscore) {
    scoretoa(hiscore, 5, hiScoreBuffer);
    lastHiScore = hiscore;
    dirty = 1;
  }
  
  if (dirty) {
    gfx_fillRect(work, 0, gfx_retroFontHeight, width, gfx_retroFontHeight+2, 0xFF000000);
    gfx_drawStringRetro(work, SCORE_X, SCORE_Y, scoreBuffer, 0xFFFFFFFF, 1, 3);    
    gfx_drawStringRetro(work, HISCORE_X, SCORE_Y, hiScoreBuffer, 0xFFFFFFFF, 1, 3);
  }
}


static void 
render(int scale, int dw, int dh)
{
  if (!screenDirty) {
    return;
  }

  gfx_fillRect(work, 0, scoreBoardHeight, width, height-scoreBoardHeight, 0xFF000000);

  renderScores();

  renderActor(&defender);

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = & invaders[i];    
    if (inv->_state != DEAD) {
      renderActor(inv);
    }
  }

  renderMissile();

  renderBases();

  renderStatusBar(width, height);

  if (scale) {
    gfx_bitBltEx(target, 0, 0, 0, 0, width, height, dw, dh, work);
  } else {
    gfx_bitBlt(target, 0, 0, 0, 0, width, height, work);
  }

  screenDirty = 0;
}


static void 
shoot()
{
  if (missile._state == DEAD) {
    missile.x = defender.x + (invaderW/2);
    missile.y = defender.y-spriteConfig[missile.sprite].height;
    missile.spriteIndex = 0;
    missile._state = ALIVE;
    audio_execute(AUDIO_CHANNEL_SHOOT);
  }
}


static void 
collision(unsigned time)
{
  if (missile._state == DEAD) {
    return;
  }


  for (int i = 0; i < NUM_BASES; i++) {
    unsigned pixel = getSpritePixelRGBA(bases[i].sprite, bases[i].spriteIndex, missile.x-bases[i].x, missile.y-bases[i].y);
    if (pixel != 0x000000FF && pixel != 0) {
      missile._state = DEAD;
      return;
    }
  }



  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = &invaders[i];
    if (inv->_state == ALIVE) {
      if (inv->x < missile.x + missileW &&
	  inv->x + invaderW > missile.x &&
	  inv->y < missile.y + missileH &&
	  invaderH + inv->y > missile.y) {
	audio_execute(AUDIO_CHANNEL_KILLED);
	inv->_state = EXPLODING;
	inv->spriteIndex = 2;
	inv->data = time;
	missile._state = DEAD;
	score++;
	if (score > hiscore) {
	  hiscore = score;
	}
	break;
      }
    } 
  }
}


int 
main(int agrc, char* argv[])
{
  unsigned time = 0;
  int scale = 2;
  const int w = SCREEN_WIDTH, h = SCREEN_HEIGHT;
  const int dw = w*scale, dh = h*scale;

  window_h window = window_create("Space Invaders", 0, 0, dw, dh);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, dw, dh, 0xFFFFFFFF);
  kernel_threadSetWindow(window);

  printf("Console debugging enabled...%d\n", time);

  width = w;
  height = h;
  target = window_getFrameBuffer(window);
  work = gfx_createFrameBuffer(w, h);
  spriteFrameBuffer = gfx_createFrameBuffer(w, h);

  initInvaders();
  initAudio();
  initRender();

  int lastTime = 0;

  for (int count = 0;;) {
   peripheral.simulator.startStopWatch = 1;

   time = peripheral.time.elapsedMilliSeconds;

    if (_console_char_avail()) {
      switch ( _console_read_char()) {
      case ' ':
	shoot();
	break;
      }
    }

    if (window_isKeyDown(window, 37)) {
      defender.x-=2;
      if (defender.x < 0) {
	defender.x = 0;
      }
      screenDirty = 1;
    }
    
    if (window_isKeyDown(window, 39)) {
      defender.x+=2;
      if (defender.x >= width-invaderW) {
	defender.x = width-invaderW-1;
      }
      screenDirty = 1;
    }

    collision(time);
    if (screenDirty) {
      render(scale != 1, dw, dh);
    }

    moveInvaders(time);
    moveMissile();

    if (count++ % 60 == 0) {
      printf("frame = %d, delta = %d\n", peripheral.simulator.stopWatchElapsed, time-lastTime);
    }

    lastTime = time;
    frame++;
    peripheral.simulator.yield = 1;
    //kernel_threadBlocked();
  }
}
