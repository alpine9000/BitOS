#include <sys/dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
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
  void* data;
} actor_t;

typedef struct {
  int row;
  int column;
} invader_data_t;

#define MISSILE_SPEED 4
#define BOMB_SPEED 1
#define BOMB_DROP_FRAMES 400
#define DEFENDER_EXPLOSION_FRAMES 100
#define DEFENDER_EXPLOSION_FRAMERATE 10

#define SCORE_X   23
#define SCORE_Y   17
#define HISCORE_X 87
#define STATUS_LINE_Y 231
#define GAMEOVER_X 70
#define GAMEOVER_Y 52
#define INVADER_TOP 70

#define NUM_DEFENDERS_X 7
#define NUM_DEFENDERS_Y 233
#define NUM_BASES 4
#define NUM_INVADER_ROWS 5
#define NUM_INVADER_COLUMNS 11
#define NUM_INVADERS (NUM_INVADER_ROWS*NUM_INVADER_COLUMNS)
#define MAX_BOMBS 4
#define NUM_BOMB_RANDOMS 10
#define NUM_BOMB_RANDOM_COLUMNS 11
#define NUM_DEFENDER_EXPLOSION_SPRITES 3

#define SCOREBOARD_HEIGHT 24
#define MISSILE_WIDTH 1
#define MISSILE_HEIGHT 4
#define INVADER_WIDTH 12
#define INVADER_HEIGHT 8
#define BOMB_WIDTH 3
#define BOMB_HEIGHT 7
#define DEFENDER_WIDTH 15
#define DEFENDER_HEIGHT 8
#define BASE_WIDTH 22
#define BASE_HEIGHT 16
#define BASE_TOP 184
#define SPRITEMAP_WIDTH 128
#define SPRITEMAP_HEIGHT 64


static sprite_t spriteConfig[] = { 
  {INVADER_WIDTH, INVADER_HEIGHT, 3},
  {INVADER_WIDTH, INVADER_HEIGHT, 3},
  {INVADER_WIDTH, INVADER_HEIGHT, 3},
  {BOMB_WIDTH, BOMB_HEIGHT, 3},
  {DEFENDER_WIDTH,DEFENDER_HEIGHT, 4},
  {MISSILE_WIDTH, MISSILE_HEIGHT, 2},
  {BASE_WIDTH, BASE_HEIGHT, 4},
  {0, 0, 0}
};

typedef enum {
  SPRITE_INVADER1 = 0,
  SPRITE_INVADER2 = 1,
  SPRITE_INVADER3 = 2,
  SPRITE_MISSILE = 3,
  SPRITE_DEFENDER = 4,
  SPRITE_DEFENDER_MISSILE = 5,
  SPRITE_BASE = 6
} sprite_index_t;


static actor_t invaders[NUM_INVADER_COLUMNS*NUM_INVADER_ROWS];

static actor_t bombs[MAX_BOMBS] = {0};

static actor_t missile = {0, 0, SPRITE_DEFENDER_MISSILE, 0, DEAD, 0};

static actor_t defender = {0, 208, SPRITE_DEFENDER, 0, ALIVE, 0};

static actor_t spareDefenders[] = {
  {24, 232, SPRITE_DEFENDER, 0, ALIVE, 0},
  {40, 232, SPRITE_DEFENDER, 0, ALIVE, 0}
};

static actor_t bases[NUM_BASES] = {
  {30, BASE_TOP, SPRITE_BASE, 0, ALIVE, 0},
  {75, BASE_TOP, SPRITE_BASE, 1, ALIVE, 0},
  {120, BASE_TOP, SPRITE_BASE, 2, ALIVE, 0},
  {165, BASE_TOP, SPRITE_BASE, 3, ALIVE, 0}
};

typedef struct {
  int x;
  int y;
} coord_t;

coord_t missileBaseExplosion[] = 
  {
    {-2, -3}, {0, -3}, {2, -3}, 
    {-1, -2}, {-3, -2}, {0, -2}, {1, -2}, {3, -2},
    {-1, -1}, {-2, -1}, {0, -1}, {1, -1}, {2, -1},
    {-1, 0}, {-3, 0}, {1, 0}, {3, 0}, {0, 0}
  };

coord_t bombBaseExplosion[] = 
  {
    {-2, 3}, {0, 3}, {2, 3}, 
    {-1, 2}, {-3, 2}, {0, 2}, {1, 2}, {3, 2},
    {-1, 1}, {-2, 1}, {0, 1}, {1, 1}, {2, 1},
    {-1, 0}, {-3, 0}, {1, 0}, {3, 0}, {0, 0}
  };

coord_t* baseExplosion[] = {
  missileBaseExplosion,
  bombBaseExplosion
};


int bombRandomMap[NUM_BOMB_RANDOMS] = { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0};
int bombRandomColumns[NUM_BOMB_RANDOM_COLUMNS] = { 0, 2, 6, 1, 9, 3, 5, 7, 8, 4, 10};

static int killScores[NUM_INVADER_ROWS] = { 30, 20, 20, 10, 10};
static int invaderIndex = 0;
static int invaderDirection = 1;
static int invaderSpeed = 500;
static int score = 0;
static int hiscore = 0;
static unsigned frame = 0;
static int screenDirty = 1;
static int numDefenders = 3;
static int target, work, width, height, spriteFrameBuffer;
static window_h window;

static void 
initInvader(int x, int y, int row, int column, unsigned sprite)
{
  actor_t *i = &  invaders[invaderIndex++];
  i->x = x;
  i->y = y;
  i->sprite = sprite;
  i->spriteIndex = 0;
  i->_state = ALIVE;
  invader_data_t* data = malloc(sizeof(data));
  data->row = row;
  data->column = column;
  i->data = data;
}


static void 
initInvaders()
{
  unsigned char sprites[] = {2, 0, 0, 1, 1};
  for (int y = NUM_INVADER_ROWS-1; y >= 0; y--) {
    for (int x = 0; x < NUM_INVADER_COLUMNS; x++) {
      initInvader(20+(x * 16), INVADER_TOP+(y * 16), y, x, sprites[y]);
    } 
  }
}

static void 
dropBomb(int index, int x, int y)
{
  actor_t *i = &bombs[index];
  i->x = x;
  i->y = y;
  i->sprite = SPRITE_MISSILE;
  i->spriteIndex = 0;
  i->_state = ALIVE;
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
      unsigned* dp = ptr + (y*SPRITEMAP_WIDTH) + x;
      drawSpriteRGBA(x, y, dp);
    }
  }
}


static void
putSpritePixelRGBA(int sprite, int index, int x, int y, unsigned pixel)
{
  unsigned *ptr = (unsigned*) &sprite_rgba;
  sprite_t* sp = &spriteConfig[sprite];
  int sy = sp->height*index;  
  int sx = 0;
  
  if (x < 0 || y < 0) {
    return;
  }
    
  for (int i = 0; i < sprite; i++) {
    sx += spriteConfig[i].width;
  }
  
  unsigned* dp = ptr + ((sy+y)*SPRITEMAP_WIDTH) + (sx+x);
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
  
  unsigned* dp = ptr + ((sy+y)*SPRITEMAP_WIDTH) + (sx+x);
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
    inv->y += INVADER_HEIGHT;
  }
}


static void
moveMissile()
{
  if (missile._state == ALIVE) {
    missile.y-=MISSILE_SPEED;
    if ((missile.y+MISSILE_HEIGHT) < BASE_TOP) {
      missile.spriteIndex = 1;
    }
    screenDirty = 1;
  }

  if (missile.y <= SCOREBOARD_HEIGHT) {
    missile._state = DEAD;
  }
}


static void
moveBombs()
{
  for (int i = 0; i < MAX_BOMBS; i++) {
    actor_t* b = &bombs[i];
    if (b->_state == ALIVE) {
      b->y+=BOMB_SPEED;
      if (b->y >= STATUS_LINE_Y)
	b->_state = DEAD;
    }
    screenDirty = 1;
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
renderGameOver()
{
  if (numDefenders == 0) {
    gfx_fillRect(work, GAMEOVER_X, GAMEOVER_Y, 9*(gfx_retroFontWidth+3), gfx_retroFontHeight, 0xFF000000);
    gfx_drawStringRetro(work, GAMEOVER_X, GAMEOVER_Y, "GAME OVER", 0xFFFFFFFF, 1, 3);  
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
  gfx_drawLine(work, 0, STATUS_LINE_Y, w, STATUS_LINE_Y, 0xFF00FF00);
  gfx_fillRect(work, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, gfx_retroFontWidth, gfx_retroFontHeight, 0xFF000000);
  char buffer[2] = {'0' + numDefenders, 0};
  gfx_drawStringRetro(work, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, buffer , 0xFFFFFFFF, 1, 0);    

  for (int i = 0; i < numDefenders-1; i++) {
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
    gfx_fillRect(work, 0, SCORE_Y, width, gfx_retroFontHeight+2, 0xFF000000);
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

  gfx_fillRect(work, 0, SCOREBOARD_HEIGHT, width, height-SCOREBOARD_HEIGHT, 0xFF000000);

  renderScores();

  renderGameOver();

  if (defender._state != DEAD) {
    renderActor(&defender);
  }

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = & invaders[i];    
    if (inv->_state != DEAD) {
      renderActor(inv);
    }
  }

  for (int i = 0; i < MAX_BOMBS; i++) {
    actor_t *bomb = & bombs[i];    
    if (bomb->_state != DEAD) {
      renderActor(bomb);
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


actor_t* findBottomInvader()
{
  int column = bombRandomColumns[frame %  NUM_BOMB_RANDOM_COLUMNS];
  for (int i = 0; i < NUM_INVADERS; i++) {
    if (invaders[i]._state == ALIVE  && ((invader_data_t*)invaders[i].data)->column  == column) {
      return &invaders[i];
      break;
    }
  }

  return 0;
}

static void
bomb()
{
  if (frame % BOMB_DROP_FRAMES == 0) {
    if (bombRandomMap[frame % NUM_BOMB_RANDOMS]) {
      for (int i = 0; i < MAX_BOMBS; i++) {
	if (bombs[i]._state == DEAD) {
	  actor_t *invader;
	  if ((invader = findBottomInvader()) != 0) {
	    dropBomb(i, invader->x + INVADER_WIDTH/2, invader->y + INVADER_HEIGHT);
	    break;
	  }
	}
      }
    }
  }
}

static void 
shoot()
{
  if (missile._state == DEAD) {
    missile.x = defender.x + (INVADER_WIDTH/2);
    missile.y = defender.y-spriteConfig[missile.sprite].height;
    missile.spriteIndex = 0;
    missile._state = ALIVE;
    audio_execute(AUDIO_CHANNEL_SHOOT);
  }
}


static void
explodeBase(int baseIndex, int explosionIndex, int x, int y)
{
  int baseX = bases[baseIndex].x;
  int baseY = bases[baseIndex].y;
  int i;

  for (i = 0; baseExplosion[explosionIndex][i].x != 0 || baseExplosion[explosionIndex][i].y != 0; i++) {
    putSpritePixelRGBA(SPRITE_BASE, baseIndex, (x+baseExplosion[explosionIndex][i].x-baseX), (y+baseExplosion[explosionIndex][i].y-baseY), 0x000000FF);
  }
  putSpritePixelRGBA(SPRITE_BASE, baseIndex, (x+baseExplosion[explosionIndex][i].x-baseX), (y+baseExplosion[explosionIndex][i].y-baseY), 0x000000FF);
  transferSprite(SPRITE_BASE, baseIndex);
}

static int
missileBaseCollision()
{
  #define numOffsets 3
  int xOffsets[numOffsets] = {0, 1, -1};

  for (int y = MISSILE_SPEED; y >= 0; y--) {
    for (int x = 0; x < numOffsets; x++) {
      for (int i = 0; i < NUM_BASES; i++) {
	unsigned pixel = getSpritePixelRGBA(bases[i].sprite, bases[i].spriteIndex, missile.x-bases[i].x+xOffsets[x], missile.y-bases[i].y+y);
	if (pixel != 0x000000FF && pixel != 0) {
	  missile._state = DEAD;
	  explodeBase(i, 0, missile.x+xOffsets[x], missile.y+y);
	  return 1;
	}
      }
    }
  }

  return 0;
}

static int
bombBaseCollision(int bombIndex)
{
  #define numOffsets 3
  int xOffsets[numOffsets] = {0, 1, -1};
  actor_t* b = &bombs[bombIndex];
  int bombY = b->y + BOMB_HEIGHT;

  for (int y = BOMB_SPEED; y >= 0; y--) {
    for (int x = 0; x < numOffsets; x++) {
      for (int i = 0; i < NUM_BASES; i++) {
	unsigned pixel = getSpritePixelRGBA(bases[i].sprite, bases[i].spriteIndex, b->x-bases[i].x+xOffsets[x], bombY-bases[i].y+y);
	if (pixel != 0x000000FF && pixel != 0) {
	  b->_state = DEAD;
	  explodeBase(i, 1, b->x+xOffsets[x], bombY+y-1);
	  return 1;
	}
      }
    }
  }

  return 0;
}


static void
bombCollision()
{
  for (int i = 0; i < MAX_BOMBS; i++) {
    if (bombs[i]._state == ALIVE) {
      if (defender.x < bombs[i].x + BOMB_WIDTH &&
	  defender.x + DEFENDER_WIDTH > bombs[i].x &&
	  defender.y < bombs[i].y + BOMB_HEIGHT &&
	  DEFENDER_HEIGHT + defender.y > bombs[i].y) {
	defender._state = EXPLODING;
	defender.data = 0;
	defender.spriteIndex = 1;
	bombs[i]._state = DEAD;
	break;
      }
    }
  }
}

static void 
missileCollision()
{
  if (missile._state == DEAD) {
    return;
  }

  if (missileBaseCollision()) {
    return;
  }

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = &invaders[i];
    if (inv->_state == ALIVE) {
      if (inv->x < missile.x + MISSILE_WIDTH &&
	  inv->x + INVADER_WIDTH > missile.x &&
	  inv->y < missile.y + MISSILE_HEIGHT &&
	  INVADER_HEIGHT + inv->y > missile.y) {
	audio_execute(AUDIO_CHANNEL_KILLED);
	inv->_state = EXPLODING;
	inv->spriteIndex = 2;
	missile._state = DEAD;
	score = score + killScores[((invader_data_t*)inv->data)->row];
	if (score > hiscore) {
	  hiscore = score;
	}
	break;
      }
    } 
  }
}

static void
moveDefender()
{
    if (defender._state == ALIVE) {
      if (window_isKeyDown(window, 37)) {
	defender.x-=2;
	if (defender.x < 0) {
	  defender.x = 0;
	}
	screenDirty = 1;
      }
      
      if (window_isKeyDown(window, 39)) {
	defender.x+=2;
	if (defender.x >= width-INVADER_WIDTH) {
	  defender.x = width-INVADER_WIDTH-1;
	}
	screenDirty = 1;
      }
    } else if (defender._state == EXPLODING) {
      int explosionCount = (int)defender.data;
      if (explosionCount++ > DEFENDER_EXPLOSION_FRAMES) {
	defender.data = 0;
	defender.spriteIndex = 0;
	if (--numDefenders > 0) {
	  defender._state = ALIVE;
	} else {
	  defender._state = DEAD;
	}
      } else {
	defender.data = (void*)explosionCount;
	if (explosionCount % DEFENDER_EXPLOSION_FRAMERATE == 0) {
	  defender.spriteIndex++;
	}
	if (defender.spriteIndex == NUM_DEFENDER_EXPLOSION_SPRITES) {
	  defender.spriteIndex = 1;
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

  window = window_create("Space Invaders", 0, 0, dw, dh);
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

   if (numDefenders > 0) {
     if (_console_char_avail()) {
       switch ( _console_read_char()) {
       case ' ':
	 shoot();
	 break;
       }
     }
     
     bomb();
     
     moveDefender();
     missileCollision();
     bombCollision();
     
     for (int i = 0; i < MAX_BOMBS; i++) {
       if (bombs[i]._state == ALIVE) {
	 bombBaseCollision(i);
       }
     }
     
     if (screenDirty) {
       render(scale != 1, dw, dh);
     }
     
     moveInvaders(time);
     moveMissile();
     moveBombs();
     
     if (0 && count++ % 60 == 0) {
       printf("frame = %d, delta = %d\n", peripheral.simulator.stopWatchElapsed, time-lastTime);
     }
     
     lastTime = time;
     frame++;
     peripheral.simulator.yield = 1;
   } else {
     kernel_threadBlocked();
   }
  }
}
