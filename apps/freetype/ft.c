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

#define WIDTH   640
#define HEIGHT  480

  
int
     timeval_subtract (result, x, y)
          struct timeval *result, *x, *y;
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }
     
  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;
     
  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
void my_draw_bitmap(unsigned fb, FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;


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
	    //printf("%d\n", c);
	  }
	}
    }
}


void runShell()
{

  unsigned w = 640, h = 480;
  unsigned window = window_create("Shell", 0, 0, w, h);
  unsigned fb = window_getFrameBuffer(window);
  gfx_fillRect(fb, 0, 0, w, h, 0xFFFFFFFF);
  thread_window(window);

FT_Library  library;
  FT_Face     face;      /* handle to face object */


  unsigned error = FT_Init_FreeType( &library );
  if (error) {
    printf("Failed to init FT\n");
  } else {
    printf("FT_Init_FreeType OK\n");
  }

  int fd = open("/fonts/tnr.ttf", O_RDONLY);

    struct stat s;
    if (fstat(fd, &s) < 0) {
      printf("fstat failed\n");
    }

    printf("fd = %d, size = %d\n", fd, (int)s.st_size);

    unsigned *address = malloc(s.st_size);
        read(fd, address, s.st_size);
    error = FT_New_Memory_Face( library, (const FT_Byte*)address, s.st_size, 0, &face);
    // error = FT_New_Face( library, "/arial.ttf", 0, &face);
  if (error) {
    printf("FFT_New_Face Failed: %d\n", error);
  } else {
    printf("FT_New_Face OK\n");
  }



  error = FT_Set_Char_Size(
			   face,    /* handle to face object           */
			   0,       /* char_width in 1/64th of points  */
			   16*64,   /* char_height in 1/64th of points */
			   300,     /* horizontal device resolution    */
			   300);   /* vertical device resolution      */

  if (error) {
    printf("FT_Set_Char_Size: %d\n", error);
  } else {
    printf("FT_Set_Char_Size OK\n");
  }


    FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
  int           pen_x, pen_y, n;

  //  char* text = "The quick brown fox jumps over the lazy dog";
  char* text = "Hello there";
  int num_chars = strlen(text);

  pen_x = 0;
  pen_y = 200;
  
  
  struct timeval start, end, diff;
  gettimeofday(&start, 0);
  printf("%d %d\n", (int)start.tv_sec, (int)start.tv_usec);
#if 0
  for ( n = 0; n < num_chars; n++ )
    {
      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Char( face, text[n], FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT );
      if ( error ) {
	printf("FT_Load_Char Error %d\n", error);
	continue;  /* ignore errors */
      }

      /* now, draw to our target surface */
      my_draw_bitmap(fb, &slot->bitmap,
		      pen_x + slot->bitmap_left,
		      pen_y - slot->bitmap_top );

      /* increment pen position */
            pen_x += slot->advance.x >> 6;
    }
#else
  FT_UInt previous = 0;
  FT_UInt       glyph_index;

  for ( n = 0; n < num_chars; n++ )
    {
      /* convert character code to glyph index */
      glyph_index = FT_Get_Char_Index( face, text[n] );

      /* retrieve kerning distance and move pen position */
      if ( previous && glyph_index )
	{
	  FT_Vector  delta;


	  FT_Get_Kerning( face, previous, glyph_index,
			  FT_KERNING_DEFAULT, &delta );
	  
	  printf("%d\n", (int) delta.x);

	  pen_x += delta.x >> 6;
	}

      /* load glyph image into the slot (erase previous one) */
      error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT  );
      if ( error ) {
	printf("FT_Load_Char Error %d\n", error);
	continue;  /* ignore errors */
      }

      /* now draw to our target surface */
      my_draw_bitmap(fb, &slot->bitmap,
		      pen_x + slot->bitmap_left,
		      pen_y - slot->bitmap_top );

      /* increment pen position */
      pen_x += slot->advance.x >> 6;

      /* record current glyph index */
      previous = glyph_index;
    }


#endif

  gettimeofday(&end, 0);
  printf("%d %d\n", (int)end.tv_sec, (int)end.tv_usec);
  timeval_subtract(&diff, &end, &start);
  printf("%d %d\n", (int)diff.tv_sec, (int)diff.tv_usec);
  printf("Done\n");
  for(int i = 0; i < 2*60;i++) {
    thread_blocked();
  }

    window_close(window);
      free(address);
  FT_Done_Face    ( face );
   FT_Done_FreeType( library );

}



int main()
{
  runShell();
  return 0;
}
