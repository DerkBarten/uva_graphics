/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ mla.c
 * Description ..... Midpoint Line Algorithm
 * Created by ...... Jurgen Sturm
 *
 * Student name Derk Barten
 * Student email derk.barten@student.uva.nl
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include "SDL.h"
#include "init.h"

/*
 * Midpoint Line Algorithm
 *
 * As you probably will have figured out, this is the part where you prove
 * your programming skills. The code in the mla function should draw a direct
 * line between (x0,y0) and (x1,y1) in the specified color.
 *
 * Until now, the example code below draws only a horizontal line between
 * (x0,y0) and (x1,y0) and a vertical line between (x1,y1).
 *
 * And here the challenge begins..
 *
 * Good luck!
 *
 *
 */
int f(int x, int y, int x0, int y0, int x1, int y1) {
  return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void mla(SDL_Texture *t, int x0, int y0, int x1, int y1, Uint32 colour) {
  int dx = x1 - x0; 
  int dy = y1 - y0;
  int d = f(x0 + 1, y0, x0, y0, x1, y1) + (x1 - x0) / 2;

  // TODO: optimize

  // The quadrants of the southern hemisphere
  int q5 = -dx >= -dy && -dy >= 0;
  int q6 = -dy > -dx && -dx >= 0;
  int q7 = -dy > dx && dx > 0;
  int q8 = dx >= -dy && -dy >= 0;

  // Dirty trick to avoid specifying the souther hemisphere of the circle
  if (q5 || q6 || q7 || q8 ){
    // TODO: xor switch
    int temp = x0;
    x0 = x1;
    x1 = temp;

    temp = y0;
    y0 = y1;
    y1 = temp;

    dx = x1 - x0; 
    dy = y1 - y0;
  }

  // TODO: create a better approach than this

  // First quadrant
  if (dx >= dy && dy >= 0){
    int y = y0;
    for (int x = x0; x <= x1; x++){
      PutPixel(t, x, y, colour);
      if (d < 0) {
        y = y + 1;
        d = d + abs(dx) - abs(dy);
      }
      else
        d = d - abs(dy);
    }
  }

  // Second quadrant
  if (dy > dx && dx >= 0){
    int x = x0;
    for (int y = y0; y <= y1; y++){
      PutPixel(t, x, y, colour);
      if (d < 0) {
        x = x + 1;
        d = d + abs(dy) - abs(dx);
      }
      else
        d = d - abs(dx);
    }
  }

  // Third quadrant
  if (dy > -dx && dx < 0){
    int x = x0;
    for (int y = y0; y <= y1; y++){
      PutPixel(t, x, y, colour);
      if (d < 0) {
        x = x - 1;
        d = d + abs(dy) - abs(dx);
      }
      else
        d = d - abs(dx);
    }
  }

  // Fourth quadrant
  if (-dx >= dy && dy >= 0){
    int y = y0;
    for (int x = x0; x >= x1; x--){
      PutPixel(t, x, y, colour);
      if (d < 0) {
        y = y + 1;
        d = d + abs(dx) - abs(dy);
      }
      else
        d = d - abs(dy);
    }
  }

  return;
}
