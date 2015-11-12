/*
Copyright (c) 2004-2007, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
//#include <string>
//#include <vector>
//#include <iostream>

//#include "quickcg.h"
//using namespace QuickCG;

//#include "cpp.h"
extern "C" {
#include <stdio.h>
#include "gfx.h"
#include "console.h"
#include "simulator.h"
#include "peripheral.h"
#include "window.h"
#include "kernel.h"
}

#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24

#include "wolf.h"

extern "C" {
  void itoa(int n, char s[]);
};
char fps_buf[20];
unsigned fps_count = 10;

int worldMap[mapWidth][mapHeight]=
{
  {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

unsigned background_fb, work_fb;
unsigned target_fb;


static int strlen(char*ptr)
{
  int count = 0;
  while (*ptr != 0) {
    count++;
    ptr++;
  }
  return count;
}

void reverse(char s[])
{
  int i, j;
  char c;
 
  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}


void itoa(int n, char s[])
{
  int i, sign;
 
  if ((sign = n) < 0)  /* record sign */
    n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}



void screen(unsigned target, unsigned short w, unsigned short h, unsigned short frameBuffer, const char* name) {

  const int scale = peripheral.devicePixelRatio;
  work_fb = gfx_createFrameBuffer(w, h);
  background_fb = gfx_createFrameBuffer(w, h);
  target_fb = target;

#ifndef FLOOR
  //gfx_setFrameBuffer(2);
  gfx_fillRect(background_fb, 0, 0, w, h, 0xFFCCCCCC);
  gfx_fillRect(background_fb, 0, 0, w, h/(2*scale), 0xFF444444);  
#endif


  //gfx_setFrameBuffer(frameBuffer);
  //#ifndef _KERNEL_MULTI_THREAD
    // peripheral.video[target_fb].blt.frameBufferScaling = ((1 << 16) | scale);
  //#endif

#ifndef FLOOR
  gfx_bitBlt(work_fb, 0, 0, 0, 0, w/scale, h/scale, background_fb);
  gfx_loadData(work_fb);
#endif

  //#ifndef _KERNEL_MULTI_THREAD
  //gfx_bitBlt(0, 0, 0, 0, 0, w, h, 1);
  peripheral.video[target_fb].blt.srcOrigin = 0;
  peripheral.video[target_fb].blt.destOrigin = 0;
  peripheral.video[target_fb].blt.srcSize = (w << 16) | h;
  peripheral.video[target_fb].blt.destSize = (w << 16) | h;
  //#endif
}


void wolfcpp(window_h window, unsigned target)
{
  double posX = 22.0, posY = 11.5; //x and y start position
  double dirX = -1.0, dirY = 0.0; //initial direction vector
  double planeX = 0.0, planeY = 0.66; //the 2d raycaster version of camera plane

  const int scale = peripheral.devicePixelRatio;
  
  double time = 0; //time of current frame
  double oldTime = 0; //time of previous frame
  
  typedef Uint32* texture_map_t[8];
  texture_map_t texture;
  texture_map_t darkTexture;

  screen(target, w,h,1, "Raycaster");

  extern unsigned char eagle_rgba[];
  extern unsigned char redbrick_rgba[];
  extern unsigned char purplestone_rgba[];
  extern unsigned char greystone_rgba[];
  extern unsigned char bluestone_rgba[];
  extern unsigned char mossy_rgba[];
  extern unsigned char wood_rgba[];
  extern unsigned char colorstone_rgba[];
   
  //load some textures
  //int tw, th, error = 0;
  texture[0] = (Uint32*)&eagle_rgba;
  texture[1] = (Uint32*)&redbrick_rgba;
  texture[2] = (Uint32*)&purplestone_rgba;
  texture[3] = (Uint32*)&greystone_rgba;
  //texture[4] = (Uint32*)&bluestone_rgba;
  texture[4] = (Uint32*)&greystone_rgba;
  texture[5] = (Uint32*)&mossy_rgba;
  texture[6] = (Uint32*)&wood_rgba;
  texture[7] = (Uint32*)&colorstone_rgba;

  darkTexture[0] = (Uint32*)&bluestone_rgba;
  darkTexture[1] = (Uint32*)&bluestone_rgba;
  darkTexture[2] = (Uint32*)&bluestone_rgba;
  darkTexture[3] = (Uint32*)&bluestone_rgba;
  darkTexture[4] = (Uint32*)&bluestone_rgba;
  darkTexture[5] = (Uint32*)&bluestone_rgba;
  darkTexture[6] = (Uint32*)&bluestone_rgba;
  darkTexture[7] = (Uint32*)&bluestone_rgba;


  //start the main loop
  while (1)
  {
    for(int x = 0; x < w; x++)
    {
      //calculate ray position and direction 
      double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
      double rayPosX = posX;
      double rayPosY = posY;
      double rayDirX = dirX + planeX * cameraX;
      double rayDirY = dirY + planeY * cameraX;
     
      //which box of the map we're in  
      int mapX = int(rayPosX);
      int mapY = int(rayPosY);
       
      //length of ray from current position to next x or y-side
      double sideDistX;
      double sideDistY;
       
      //length of ray from one x or y-side to next x or y-side
      double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
      double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
      double perpWallDist;
       
      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;
      
      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?

      //calculate step and initial sideDist
      if (rayDirX < 0)
      {
        stepX = -1;
        sideDistX = (rayPosX - mapX) * deltaDistX;
      }
      else
      {
        stepX = 1;
        sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
      }
      if (rayDirY < 0)
      {
        stepY = -1;
        sideDistY = (rayPosY - mapY) * deltaDistY;
      }
      else
      {
        stepY = 1;
        sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
      }
      //perform DDA
      while (hit == 0)
      {
        //jump to next map square, OR in x-direction, OR in y-direction
        if (sideDistX < sideDistY)
        {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        }
        else
        {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }
        //Check if ray has hit a wall
        if (worldMap[mapX][mapY] > 0) hit = 1;
      } 
        
      //Calculate distance of perpendicular ray (oblique distance will give fisheye effect!)
      if (side == 0) perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
      else           perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
        
      //Calculate height of line to draw on screen
      int lineHeight = abs(int(h / perpWallDist));
       
      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + h / 2;
      if(drawStart < 0) drawStart = 0;
      int drawEnd = lineHeight / 2 + h / 2;
      if(drawEnd >= h) drawEnd = h - 1;
      //texturing calculations
      int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!
       
      //calculate value of wallX
      double wallX; //where exactly the wall was hit
      if (side == 1) wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
      else           wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
      wallX -= floor((wallX));
       
      //x coordinate on the texture
      int texX = int(wallX * double(texWidth));
      if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
      if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

      //peripheral.videoAddressX = x;

      const double dh = (double)h/2.0;
      const double lh2 = (double)lineHeight/2.0;
      texture_map_t* tptr;
      if (side) {
	tptr= &darkTexture;
      } else {
	tptr= &texture;
      }
      for(int y = drawStart; y < drawEnd; y++)
      {
	//unsigned int d = (y<<8) - (h * 128) + (lineHeight * 128); //256 and 128 factors to avoid floats
	//unsigned int texY = ((d * texHeight) / lineHeight) >>8;

	//double d = (double)y - (double)h/2.0 + (double)lineHeight/2.0;
	//double dtexY = ((d * th) / lh);

	double d = y - dh + lh2;
	unsigned int texY = ((d * (double)texHeight) / (double)lineHeight);

	drawRGBA(x, y, (*tptr)[texNum][texWidth * texY + texX]);
      }
      
#ifdef FLOOR
      //FLOOR CASTING
#ifdef FOOR_TEX
      float floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall

      //4 different wall directions possible
      if(side == 0 && rayDirX > 0)
      {
        floorXWall = mapX;
        floorYWall = mapY + wallX;
      }
      else if(side == 0 && rayDirX < 0)
      {
        floorXWall = mapX + 1.0;
        floorYWall = mapY + wallX;
      }
      else if(side == 1 && rayDirY > 0)
      {
        floorXWall = mapX + wallX;
        floorYWall = mapY;
      }
      else
      {
        floorXWall = mapX + wallX;
        floorYWall = mapY + 1.0;
      } 
      
      float distWall, distPlayer, currentDist;

      distWall = perpWallDist;
      distPlayer = 0.0;
#endif
      if (drawEnd < 0) drawEnd = h; //becomes < 0 when the integer overflows
      
      //draw the floor from drawEnd to the bottom of the screen

      for(int y = drawEnd + 1; y <= h; y++)
      {
#ifdef FOOR_TEX
        currentDist = h / (2.0 * y - h); //you could make a small lookup table for this instead

        float weight = (currentDist - distPlayer) / (distWall - distPlayer);
         
        float currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
        float currentFloorY = weight * floorYWall + (1.0 - weight) * posY;
        
        int floorTexX, floorTexY;
        floorTexX = int(currentFloorX * texWidth) % texWidth;
        floorTexY = int(currentFloorY * texHeight) % texHeight;
        
        int checkerBoardPattern = (int(currentFloorX + currentFloorY)) % 2;
        int floorTexture;
        if(checkerBoardPattern == 0) floorTexture = 3;
        else floorTexture = 4;
        	
	drawRGBA(x, y, texture[floorTexture][texWidth * floorTexY + floorTexX]);
	drawRGBA(x, h-y, texture[6][texWidth * floorTexY + floorTexX]);
#endif
	drawRGBA(x, y-1, 0xCCCCCCFF);
	drawRGBA(x, h-y, 0x444444FF);


      }
#endif
    }
    
    gfx_saveData(work_fb);

    if (fps_count == 10) {
      itoa(time-oldTime, fps_buf);
      fps_count = 0;
    } else {
      fps_count++;
    }
    
    gfx_drawStringEx(work_fb, 0, 0, fps_buf, 0xFFFFFFFF, 1, 1);

    //    gfx_bitBlt(0, 0, 0, 0, 0, w, h, 1);
    peripheral.video[target_fb].blt.bltFromSrcWithSize = work_fb;

    gfx_setFrameFrameBufferDirty(target_fb);

#ifndef FLOOR
    gfx_bitBlt(work_fb, 0, 0, 0, 0, w/scale, h/scale, background_fb);
    //    peripheral.video[1].blt.bltFromSrcWith = background_fb;
#endif
    gfx_loadData(work_fb);

    //timing for input and FPS counter
    oldTime = time;
    time = getTicks();
    float frameTime = (time - oldTime) / 1000.0; //frametime is the time this frame has taken, in seconds
      
    //speed modifiers
    float moveSpeed = frameTime * 3.0; //the constant value is in squares/second
    float rotSpeed = frameTime * 2.0; //the constant value is in radians/second

    //move forward if no wall in front of you
    if (keyDown(window, SDLK_UP))
    {
      if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
      if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
    }
    //move backwards if no wall behind you
    if (keyDown(window, SDLK_DOWN))
    {
      if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
      if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
    }
    //rotate to the right
    if (keyDown(window, SDLK_RIGHT))
    {
      //both camera direction and camera plane must be rotated
      float oldDirX = dirX;
      dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
      dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
      float oldPlaneX = planeX;
      planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
      planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    //rotate to the left
    if (keyDown(window, SDLK_LEFT))
    {
      //both camera direction and camera plane must be rotated
      float oldDirX = dirX;
      dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
      dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
      float oldPlaneX = planeX;
      planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
      planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }
    //simulator_yield();
    //    gfx_vsync();
    thread_blocked();
  }
}

extern "C" {
#ifdef _KERNEL_BUILD
  int wolf(int argc, char*argv[])
#else
  int main(int argc, char* argv[])
#endif
  {
   
    window_h window = window_create((char*)"Wolf2", 0, 0, w, h);
    thread_window(window);
#ifdef _KERNEL_BUILD
    window_enableCursor(window, 0);
#endif
    wolfcpp(window, window_getFrameBuffer(window));
    return 0;
  }
}
