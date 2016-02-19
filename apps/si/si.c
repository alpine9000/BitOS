#include "gfx.h"
#include "console.h"
#include "peripheral.h"
#include "kernel.h"
#include "audio.h"
#include "window.h"
#include "si.h"


#define DEBUG

#define USE(x) do { x = x; } while(0);
#ifdef DEBUG
#define printf(...) _bft->simulator_printf(__VA_ARGS__)
#else
#define printf(...)
#endif

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
  AUDIO_CHANNEL_KILLED = 5,
  AUDIO_CHANNEL_EXPLOSION = 6
} audio_channel_t;


typedef enum {
  SCREEN_DEMO,
  SCREEN_START,
  SCREEN_PLAYER_TURN_MESSAGE,
  SCREEN_GAME
} screen_t;

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


typedef struct {
  char* text;
  int x;
  int y;
  unsigned color;
} screen_text_t;

#define SCREEN_WIDTH 217
#define SCREEN_HEIGHT 248
#define SCALE  2
#define SCALED_W (SCREEN_WIDTH*SCALE)
#define SCALED_H (SCREEN_HEIGHT*SCALE)

#define MISSILE_SPEED 4
#define BOMB_SPEED 1
#define BOMB_DROP_FRAMES 50
#define DEFENDER_EXPLOSION_FRAMES 100
#define DEFENDER_EXPLOSION_FRAMERATE 10
#define DEMO_FRAME_TIME 100
#define PLAYER_TURN_MESSAGE_TIME 4000
#define SCORE_FLICKER_TIME 50


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
#define MYSTERY_INVADER_WIDTH 16
#define MYSTERY_INVADER_HEIGHT 8
#define SPRITEMAP_WIDTH 128
#define SPRITEMAP_HEIGHT 64
#define GREEN_TOP (BASE_TOP-INVADER_HEIGHT)

#define SCORE_X   23
#define SCORE_Y   17
#define HISCORE_X 87
#define STATUS_LINE_Y 231
#define GAMEOVER_X 70
#define GAMEOVER_Y 52
#define INVADER_TOP 70
#define INVADER_LEFT 20
#define INVADER_RIGHT_MARGIN 20
#define INVADER_SPACING 16
#define DEFENDER_Y 208
#define CREDIT_LABEL_X 135

#define NUM_DEMO_INVADERS 4
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

#define COLOR_GREEN 0xFF00FF00
#define RGBA_COLOR_BACKGROUND 0x000000FF

static sprite_t spriteConfig[] = { 
  {INVADER_WIDTH, INVADER_HEIGHT, 5},
  {INVADER_WIDTH, INVADER_HEIGHT, 5},
  {INVADER_WIDTH, INVADER_HEIGHT, 5},
  {BOMB_WIDTH, BOMB_HEIGHT, 3},
  {DEFENDER_WIDTH,DEFENDER_HEIGHT, 4},
  {MISSILE_WIDTH, MISSILE_HEIGHT, 2},
  {BASE_WIDTH, BASE_HEIGHT, 4},
  {MYSTERY_INVADER_WIDTH, MYSTERY_INVADER_HEIGHT, 1},
  {0, 0, 0}
};

typedef enum {
  SPRITE_INVADER1 = 0,
  SPRITE_INVADER2 = 1,
  SPRITE_INVADER3 = 2,
  SPRITE_MISSILE = 3,
  SPRITE_DEFENDER = 4,
  SPRITE_DEFENDER_MISSILE = 5,
  SPRITE_BASE = 6,
  SPRITE_MYSTERY_INVADER = 7
} sprite_index_t;


static actor_t invaders[NUM_INVADER_COLUMNS*NUM_INVADER_ROWS];
static invader_data_t invaderData[NUM_INVADER_COLUMNS*NUM_INVADER_ROWS];

static actor_t bombs[MAX_BOMBS] = {0};

static actor_t missile = {0, 0, SPRITE_DEFENDER_MISSILE, 0, DEAD, 0};

static actor_t defender = {0, DEFENDER_Y, SPRITE_DEFENDER, 0, ALIVE, 0};

static actor_t demoInvaders[NUM_DEMO_INVADERS] = {
  {64, 137, SPRITE_MYSTERY_INVADER, 0, ALIVE, 0},
  {66, 152, SPRITE_INVADER3, 0, ALIVE, 0},
  {66, 168, SPRITE_INVADER1, 0, ALIVE, 0},
  {66, 184, SPRITE_INVADER2, 3, ALIVE, 0}
};

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
static int credits = 0;
static int creditsMode = 1;
static int hiscore = 0;
static unsigned frame = 0;
static int screenDirty = 1;
static int numDefenders = 0;
static int target, work, spriteFrameBuffer;
static window_h window;
static int renderTime = 0;
static int lastTime = 0;
static int demoIndex = 0;
static int playerTurnMessageTime = 0;
static screen_t currentScreen = SCREEN_DEMO;


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
initInvader(int x, int y, int row, int column, unsigned sprite)
{
  actor_t *i = &  invaders[invaderIndex];
  i->x = x;
  i->y = y;
  i->sprite = sprite;
  i->spriteIndex = 0;
  i->_state = ALIVE;
  invader_data_t* data = &invaderData[invaderIndex];
  data->row = row;
  data->column = column;
  i->data = data;
  invaderIndex++;
}


static void 
initInvaders()
{
  unsigned char sprites[] = {2, 0, 0, 1, 1};
  for (int y = NUM_INVADER_ROWS-1; y >= 0; y--) {
    for (int x = 0; x < NUM_INVADER_COLUMNS; x++) {
      initInvader(INVADER_LEFT+(x * INVADER_SPACING), INVADER_TOP+(y * INVADER_SPACING), y, x, sprites[y]);
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

  if (!(x >= 0 && x < sp->width && y >= 0 && y < sp->height)) {
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

  gfx_fillRect(work, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0xFF000000);
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

  audio_selectChannel(AUDIO_CHANNEL_EXPLOSION);
  audio_setType(AUDIO_TYPE_BUFFER);
  audio_setAddress((unsigned*)&explosion_wav);
  audio_setLength(explosion_wav_length);
}


static void 
downShiftInvaders()
{
  invaderSpeed = invaderSpeed - 50;

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = &invaders[i];
    inv->y += (INVADER_HEIGHT);
    if (inv->y >= GREEN_TOP && inv->spriteIndex < 2) {
       inv->spriteIndex += 3;
    }
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


//static 
void
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
	switch (inv->spriteIndex) {
	case 0:
	  inv->spriteIndex = 1;
	  break;
	case 1:
	  inv->spriteIndex = 0;
	  break;
	case 3:
	  inv->spriteIndex = 4;
	  break;
	case 4:
	  inv->spriteIndex = 3;
	  break;
	}
      } else if (inv->_state == EXPLODING) {
	inv->_state = DEAD;
      }
    }
    
    for (int i = 0; i < invaderIndex; i++) {
      actor_t *inv = &invaders[i];
      if (inv->_state == ALIVE) {
	if (inv->x > SCREEN_WIDTH-INVADER_RIGHT_MARGIN) {
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


//static 
void
renderBases()
{
  for (unsigned i = 0; i < NUM_BASES; i++) {
    renderActor(&bases[i]);
  }
}


static void
renderStatusBar(int drawLine)
{
  if (drawLine) {
    gfx_drawLine(work, 0, STATUS_LINE_Y, SCREEN_WIDTH, STATUS_LINE_Y, COLOR_GREEN);
  }

  gfx_fillRect(work, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, gfx_retroFontWidth, gfx_retroFontHeight, 0xFF000000);
  if (drawLine) {
    char buffer[2] = {'0' + numDefenders, 0};
    gfx_drawStringRetro(work, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, buffer , 0xFFFFFFFF, 1, 0);    
  }

  for (int i = 0; i < numDefenders-1; i++) {
    renderActor(&spareDefenders[i]);
  }

  gfx_drawStringRetro(work, CREDIT_LABEL_X, NUM_DEFENDERS_Y, "CREDIT ", 0xFFFFFFFF, 1, 3);    
  static char creditbuffer[3];
  static int lastCredits = -1;
  int display = creditsMode ? credits : renderTime;
  if (display < 99) {
    if (display != lastCredits) {
      scoretoa(display, 3, creditbuffer);
      lastCredits = display;
    }
    gfx_drawStringRetro(work, CREDIT_LABEL_X+((gfx_retroFontWidth+3)*7), NUM_DEFENDERS_Y, creditbuffer, 0xFFFFFFFF, 1, 3);    
  } else {
    gfx_drawStringRetro(work, CREDIT_LABEL_X+((gfx_retroFontWidth+3)*7), NUM_DEFENDERS_Y, "XX", 0xFFFFFFFF, 1, 3);    
  }

}


static void 
renderMissile()
{
  if (missile._state == ALIVE) {
    renderActor(&missile);
  }
}


static void 
renderScores(int hideScore)
{
  static char scoreBuffer[5];
  static char hiScoreBuffer[5];
  static int lastScore = -1;
  static int lastHiScore = -1;
  static int lastHide = -1;
  int dirty = 0;

  if (lastHide != hideScore) {
    dirty = 1;
    lastHide = hideScore;
  }

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
    gfx_fillRect(work, 0, SCORE_Y, SCREEN_WIDTH, gfx_retroFontHeight+2, 0xFF000000);
    if (hideScore != 1) {
      gfx_drawStringRetro(work, SCORE_X, SCORE_Y, scoreBuffer, 0xFFFFFFFF, 1, 3);    
    }
    gfx_drawStringRetro(work, HISCORE_X, SCORE_Y, hiScoreBuffer, 0xFFFFFFFF, 1, 3);
  }
}


static void
renderDefender()
{
  if (defender._state != DEAD) {
    renderActor(&defender);
  }
}


static void
renderInvaders()
{
  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = & invaders[i];    
    if (inv->_state != DEAD) {
      renderActor(inv);
    }
  }
}


static void
renderBombs()
{
 for (int i = 0; i < MAX_BOMBS; i++) {
    actor_t *bomb = & bombs[i];    
    if (bomb->_state != DEAD) {
      renderActor(bomb);
    }
  }
}


static void 
renderGameScreen(int scale)
{
  gfx_fillRect(work, 0, SCOREBOARD_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0xFF000000);

  renderScores(0);

  renderGameOver();

  renderDefender();

  renderBases();

  renderInvaders();

  renderBombs();

  renderMissile();

  renderStatusBar(1);

  if (scale) {
    gfx_bitBltEx(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCALED_W, SCALED_H, work);
  } else {
    gfx_bitBlt(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, work);
  }

  screenDirty = 0;
}


static void
renderStartScreen()
{
  gfx_fillRect(work, 0, SCOREBOARD_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0xFF000000);

  renderScores(0);

  renderStatusBar(0);

  screen_text_t text[] = {
    {"PUSH", (SCREEN_WIDTH/2) - (2*(gfx_retroFontWidth+3)), 120-(3*gfx_retroFontHeight),  0xFFFFFFFF},
    {"ONLY 1PLAYER  BUTTON", (SCREEN_WIDTH/2) - (10*(gfx_retroFontWidth+3)), 120,  0xFFFFFFFF}
  };

  for (unsigned i = 0; i < sizeof(text)/sizeof(screen_text_t); i++) {
    gfx_drawStringRetro(work, text[i].x, text[i].y, text[i].text, text[i].color, 1, 3);  
  }
    

  if (SCALE != 1) {
    gfx_bitBltEx(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCALED_W, SCALED_H, work);
  } else {
    gfx_bitBlt(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, work);
  }

  screenDirty = 0;
}

static void
renderPlayer1TurnMessageScreen(int time)
{
  static int flickerTime = 0;
  static int showScore = 0;

  gfx_fillRect(work, 0, SCOREBOARD_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0xFF000000);

  if (time - flickerTime > SCORE_FLICKER_TIME) {
    showScore = !showScore;
    flickerTime = time;
  }

  renderScores(showScore);

  renderStatusBar(0);

  screen_text_t text[] = {
    {"PLAY PLAYER<1>", (SCREEN_WIDTH/2) - (7*(gfx_retroFontWidth+3)), 120-(2*gfx_retroFontHeight),  0xFFFFFFFF}
  };

  for (unsigned i = 0; i < sizeof(text)/sizeof(screen_text_t); i++) {
    gfx_drawStringRetro(work, text[i].x, text[i].y, text[i].text, text[i].color, 1, 3);  
  }
    
  if (SCALE != 1) {
    gfx_bitBltEx(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCALED_W, SCALED_H, work);
  } else {
    gfx_bitBlt(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, work);
  }

  screenDirty = 0;
}


static void
renderDemoScreen(int time)
{
  static int lastDemoTime = 0;
  gfx_fillRect(work, 0, SCOREBOARD_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0xFF000000);

  renderScores(0);

  renderStatusBar(0);

  static screen_text_t text[] = {
    {"P", 97, 64, 0xFFFFFFFF},
    {"L", 97+(1*(gfx_retroFontWidth+3)), 64, 0xFFFFFFFF},
    {"A", 97+(2*(gfx_retroFontWidth+3)), 64, 0xFFFFFFFF},
    {"Y", 97+(3*(gfx_retroFontWidth+3)), 64, 0xFFFFFFFF},

    {"S", 56+(0*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"P", 56+(1*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"A", 56+(2*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"C", 56+(3*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"E", 56+(4*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {" ", 56+(5*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {" ", 56+(6*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},

    {"I", 114+(0*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"N", 114+(1*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"V", 114+(2*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"A", 114+(3*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"D", 114+(4*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"E", 114+(5*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"R", 114+(6*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"S", 114+(7*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},
    {"E", 114+(8*(gfx_retroFontWidth+3)), 89, 0xFFFFFFFF},

    {"*SCORE ADVANCE TABLE*", 32, 120, 0xFFFFFFFF},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"?", 81+(1*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {" ", 81+(2*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"M", 81+(3*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"Y", 81+(4*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"S", 81+(5*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"T", 81+(6*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"E", 81+(7*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"R", 81+(8*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},
    {"Y", 81+(9*(gfx_retroFontWidth+3)), 137, 0xFFFFFFFF},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"3", 81+(1*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"0", 81+(2*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {" ", 81+(3*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"P", 81+(4*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"O", 81+(5*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"I", 81+(6*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"N", 81+(7*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"T", 81+(8*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},
    {"S", 81+(9*(gfx_retroFontWidth+3)), 153, 0xFFFFFFFF},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"2", 81+(1*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"0", 81+(2*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {" ", 81+(3*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"P", 81+(4*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"O", 81+(5*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"I", 81+(6*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"N", 81+(7*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"T", 81+(8*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},
    {"S", 81+(9*(gfx_retroFontWidth+3)), 169, 0xFFFFFFFF},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"1", 81+(1*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"0", 81+(2*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {" ", 81+(3*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"P", 81+(4*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"O", 81+(5*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"I", 81+(6*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"N", 81+(7*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"T", 81+(8*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},
    {"S", 81+(9*(gfx_retroFontWidth+3)), 185, COLOR_GREEN},

  };

  for (int i = 0; i < demoIndex; i++) {
    gfx_drawStringRetro(work, text[i].x, text[i].y, text[i].text, text[i].color, 1, 3);  
    if (text[i].text[0] == '*') {
      for (int a = 0; a < NUM_DEMO_INVADERS; a++) {
	renderActor(&demoInvaders[a]);
      }
    }
  }

  if (lastDemoTime == 0) {
    lastDemoTime = time + (5*DEMO_FRAME_TIME);
  }

  if (time - lastDemoTime >  DEMO_FRAME_TIME && demoIndex < (int)(sizeof(text)/sizeof(screen_text_t))) {
    demoIndex++;
    lastDemoTime = time;
  }

  if (SCALE != 1) {
    gfx_bitBltEx(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SCALED_W, SCALED_H, work);
  } else {
    gfx_bitBlt(target, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, work);
  }

  screenDirty = 0;
}

static actor_t* 
findBottomInvaderForColumn(int column)
{
  for (int i = 0; i < NUM_INVADERS; i++) {
    if (invaders[i]._state == ALIVE  && ((invader_data_t*)invaders[i].data)->column  == column) {
      return &invaders[i];
      break;
    }
  }
  
  return 0;
}


static actor_t* 
findBottomInvader()
{
  return findBottomInvaderForColumn(bombRandomColumns[frame %  NUM_BOMB_RANDOM_COLUMNS]);
}


static void
dropBombs()
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
explodeBase(int baseIndex, int explosionIndex, int x, int y)
{
  int baseX = bases[baseIndex].x;
  int baseY = bases[baseIndex].y;
  int i;

  for (i = 0; baseExplosion[explosionIndex][i].x != 0 || baseExplosion[explosionIndex][i].y != 0; i++) {
    putSpritePixelRGBA(SPRITE_BASE, baseIndex, (x+baseExplosion[explosionIndex][i].x-baseX), (y+baseExplosion[explosionIndex][i].y-baseY), RGBA_COLOR_BACKGROUND);
  }
  putSpritePixelRGBA(SPRITE_BASE, baseIndex, (x+baseExplosion[explosionIndex][i].x-baseX), (y+baseExplosion[explosionIndex][i].y-baseY), RGBA_COLOR_BACKGROUND);
  transferSprite(SPRITE_BASE, baseIndex);
  screenDirty = 1;
}


static inline int
actorCollision(actor_t* a, int aw, int ah, actor_t* b, int bw, int bh)
{
  return (a->x < b->x + bw &&
	  a->x + aw > b->x &&
	  a->y < b->y + bh &&
	  ah + a->y > b->y);
}

static void
invaderBaseCollision()
{
  int dirtyBases[NUM_BASES] = {0};
  for (int c = 0; c < NUM_INVADER_COLUMNS; c++) {
    actor_t *inv = findBottomInvaderForColumn(c);
    for (int b = 0; b < NUM_BASES; b++) {      
      if (inv && inv->_state == ALIVE && actorCollision(inv, INVADER_WIDTH, INVADER_HEIGHT, &bases[b], BASE_WIDTH, BASE_HEIGHT)) {
	for (unsigned x = inv->x; x < (unsigned)inv->x+INVADER_WIDTH; x++) {      
	  for (unsigned y = inv->y-1; y <= (unsigned)inv->y+INVADER_HEIGHT; y++) {
	    int _x = x-bases[b].x;
	    int _y = y-bases[b].y;
	    unsigned pixel = getSpritePixelRGBA(bases[b].sprite, bases[b].spriteIndex, _x, _y);
	    if (pixel != RGBA_COLOR_BACKGROUND && pixel != 0) {
	      putSpritePixelRGBA(bases[b].sprite, bases[b].spriteIndex, _x, _y, RGBA_COLOR_BACKGROUND);
	      dirtyBases[b] = 1;
	    }
	  }
	}
      }
    }
  }

  for (int i = 0; i < NUM_BASES; i++) {
    if (dirtyBases[i]) {
      transferSprite(bases[i].sprite, bases[i].spriteIndex);
    }
  }

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
	if (pixel != RGBA_COLOR_BACKGROUND && pixel != 0) {
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
	if (pixel != RGBA_COLOR_BACKGROUND && pixel != 0) {
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
bombBasesCollision()
{
  for (int i = 0; i < MAX_BOMBS; i++) {
    if (bombs[i]._state == ALIVE) {
      bombBaseCollision(i);
    }
  }
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
	audio_execute(AUDIO_CHANNEL_EXPLOSION);
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
      if (defender.x >= SCREEN_WIDTH-INVADER_WIDTH) {
	defender.x = SCREEN_WIDTH-INVADER_WIDTH-1;
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
    screenDirty = 1;
  }
}


static void
shootMissile()
{
  if (missile._state == DEAD) {
    missile.x = defender.x + (INVADER_WIDTH/2);
    missile.y = defender.y-spriteConfig[missile.sprite].height;
    missile.spriteIndex = 0;
    missile._state = ALIVE;
      audio_execute(AUDIO_CHANNEL_SHOOT);      
  }
}


static int 
getKey()
{
  if (_console_char_avail()) {
    return _console_read_char();
  }
  return -1;
}

static void
init()
{
  initInvaders();
  initAudio();
  initRender();
}

static void
gameLoop(unsigned time, int key)
{
  if (numDefenders > 0) {
    if (key == ' ') {
      shootMissile();
    }
    
    dropBombs();
    
    moveDefender();
    moveInvaders(time);
    moveMissile();
    moveBombs();
    
    invaderBaseCollision();
    missileCollision();
    bombCollision();
    bombBasesCollision();     
  }

  if (screenDirty) {
    renderGameScreen(SCALE != 1);
  }     
  if (frame % 20 == 0) {
    renderTime = time-lastTime;      
  }
  lastTime = time;
  frame++;
}

static void
startLoop(int time, int key)
{
  if (key == '1' && credits > 0) {
    credits--;
    currentScreen = SCREEN_PLAYER_TURN_MESSAGE;
    playerTurnMessageTime = time;
  } else {
    renderStartScreen();
  }
}

static void
playerTurnMessageLoop(int time, int key) 
{
  if (time - playerTurnMessageTime > PLAYER_TURN_MESSAGE_TIME) {
    numDefenders = 3;
    currentScreen = SCREEN_GAME;
  } else {
    renderPlayer1TurnMessageScreen(time);
  }
}

static void
demoLoop(int time, int key)
{
  renderDemoScreen(time);
}

int 
main(int agrc, char* argv[])
{
  unsigned time = 0;

  window = window_create("Space Invaders", 0, 0, SCALED_W, SCALED_H);
  gfx_fillRect(window_getFrameBuffer(window), 0, 0, SCALED_W, SCALED_H, 0xFFFFFFFF);
  kernel_threadSetWindow(window);

  target = window_getFrameBuffer(window);
  work = gfx_createFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
  spriteFrameBuffer = gfx_createFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

  init();

  USE(lastTime);

  for (int count = 0;;) {
    USE(count);
    peripheral.simulator.startStopWatch = 1;
    
    time = peripheral.time.elapsedMilliSeconds;
    
    int key = getKey();
   
    
    switch (key) {
    case 'c':
      credits++;
      currentScreen = SCREEN_START;
      screenDirty = 1;
      printf("credits = %d\n", credits);
      break;
    case 'g':
      numDefenders = 3;
      currentScreen = SCREEN_GAME;
      break;
    case 'd':
      downShiftInvaders();
      break;
    case 'r':
      creditsMode = !creditsMode;
      screenDirty = 1;
      printf("creditMode = %d\n", creditsMode);
      break;
    default:
	break;
    }

    switch (currentScreen) {
    case SCREEN_DEMO:
      demoLoop(time, key);
      break;
    case SCREEN_START:
      startLoop(time, key);
      break;
    case SCREEN_PLAYER_TURN_MESSAGE:
      playerTurnMessageLoop(time, key);      
      break;
    case SCREEN_GAME:
      gameLoop(time, key);
      break;
    }
    peripheral.simulator.yield = 1;
  }
}
