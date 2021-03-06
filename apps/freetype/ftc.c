#include "gfx.h"
#include "simulator.h"
#include "kernel.h"
#include "window.h"
#include "panic.h"
#include <stdio.h>
#include "console.h"
#include "peripheral.h"


#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_MANAGER_H

#define WIDTH   640
#define HEIGHT  480

void assert(char* r, int x) 
{
  if (!x) {
    printf("%s\n", r);
  }
}
FT_Error FtcFaceRequester(FTC_FaceID faceID, FT_Library lib, FT_Pointer reqData, FT_Face *face)
{
  int ret = FT_New_Face(lib, (char *)reqData, 0, face);
  assert("FtcFaceRequester", ret == 0);
  return 0;
}

void my_draw_bitmap(unsigned fb,   FTC_SBit  bitmap, FT_Int x, FT_Int y)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->height;


  for ( i = x, p = 0; i < x_max; i++, p++ )
    {
      for ( j = y, q = 0; j < y_max; j++, q++ )
	{
	  if ( i < 0      || j < 0       ||
	       i >= WIDTH || j >= HEIGHT )
	    continue;

	  //  image[j][i] |= bitmap->buffer[q * bitmap->width + p];
	  unsigned c = bitmap->buffer[q * bitmap->width + p];
	  if (c) {
	    //c = 255-c;
	    //gfx_drawPixel(fb, i, j, 0xFF000000 | c | c << 8 | c << 16);
	    	    gfx_drawPixel(fb, i, j, c << 24);
		    //drawRGBA(i, j, c<<24);
	    //printf("%d\n", c);
	  }
	}
    }
}

int main(int argc, char **argv)
{
  FT_Library lib;
  FTC_Manager ftcManager;
  FTC_SBitCache ftcSBitCache;
  FTC_CMapCache ftcCMapCache;
  FTC_ImageTypeRec ftcImageType;
  FTC_SBit ftcSBit;
  int size = 10;
  int ret;
  unsigned w = WIDTH, h = HEIGHT;
  unsigned window = window_create("Shell", 0, 0, w,  h);
  unsigned fb = window_getFrameBuffer(window);
  gfx_fillRect(fb, 0, 0, w, HEIGHT, 0xFFFFFFFF);
  thread_window(window);


  ret = FT_Init_FreeType(&lib);
  assert("FT_Init_FreeType", ret == 0);
 
  ret = FTC_Manager_New(lib, 0, 0, 0, FtcFaceRequester, "/fonts/ARIAL.TTF", &ftcManager);
  assert("FTC_Manager_New", ret == 0);
  ret = FTC_CMapCache_New(ftcManager, &ftcCMapCache);
  assert("FTC_CMapCache_New", ret == 0);
  ret = FTC_SBitCache_New(ftcManager, &ftcSBitCache);
  assert("FTC_SBitCache_New", ret == 0);
 
  ftcImageType.face_id = 0;
  ftcImageType.width = size;
  ftcImageType.height = size;
  ftcImageType.flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT;

  FT_Face face;
  ret = FTC_Manager_LookupFace(ftcManager, 0, &face);
  assert("FTC_Manager_LookupFace", ret == 0);

  char * blah = "ATAhe quick brown fox jumps over the Lazy dog";

  //int previous = 0;
  for (int y = size; y < 300; y+=size) {
    for (int x = 0, i = 0; i < strlen(blah); i++) {
      int glyphIndex = FTC_CMapCache_Lookup(ftcCMapCache, 0, 0,blah[i]);
      assert("FTC_CMapCache_Lookup", glyphIndex > 0);

      FTC_Node ftcNode;
      ret = FTC_SBitCache_Lookup(ftcSBitCache, &ftcImageType, glyphIndex, &ftcSBit, &ftcNode);
      assert("FTC_SBitCache_Lookup", ret == 0);

      
      //printf("Has kerning = %d\n", (int)(face->face_flags & FT_FACE_FLAG_KERNING));
      /*      if ( previous && glyphIndex )
	{
	  FT_Vector  delta;
	  FT_Get_Kerning( face, previous, glyphIndex, FT_KERNING_DEFAULT, &delta );
	  x += delta.x >> 6;
	  }*/      


      
      my_draw_bitmap(fb, ftcSBit, x + ftcSBit->left, y - ftcSBit->top);

      x += ftcSBit->xadvance;

      //previous = glyphIndex;
      FTC_Node_Unref(ftcNode, ftcManager);
    }
  }
  FTC_Manager_Done(ftcManager);
}
 

 
