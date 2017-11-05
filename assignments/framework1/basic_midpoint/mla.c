/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ mla.c
 * Description ..... Midpoint Line Algorithm
 * Created by ...... Jurgen Sturm
 *
 * Student name: Derk Barten
 * Student email: derk.barten@student.uva.nl
 * Collegekaart 11043075
 * Date: 3 November 2017
 * Comments: -
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include "SDL.h"
#include "init.h"

// The formula of a line
int f(int x, int y, int x0, int y0, int x1, int y1) {
  return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void mla(SDL_Texture *t, int x0, int y0, int x1, int y1, Uint32 colour) {
  int dx = x1 - x0; 
  int dy = y1 - y0;
  int d = f(x0 + 1, y0, x0, y0, x1, y1) + (x1 - x0) / 2;

  // The quadrants of the southern hemisphere
  int q4 = -dx >= dy && dy >= 0;
  int q5 = -dx >= -dy && -dy >= 0;
  int q6 = -dy > -dx && -dx >= 0;
  int q7 = -dy > dx && dx > 0;

  // A trick to avoid specifying the whole otherside of the circle
  if (q4 || q5 || q6 || q7 ){
    // Swapping the values using the xor trick
    x0 = x0 ^ x1;
    x1 = x1 ^ x0;
    x0 = x0 ^ x1;

    y0 = y0 ^ y1;
    y1 = y1 ^ y0;
    y0 = y0 ^ y1;

    dx = x1 - x0; 
    dy = y1 - y0;
  }

// Specifies if d need to be increased or decreased
int direction_change = 1;
// Speficies if x is the main direction of the line
int x_direction = 1;

// No need to check Q1 because the above values are the default values of Q1's
// Q8 is used here because all these quadrants increment instead of decrement, 
// which makes it easier to generalize.
// Q8
if (dx >= -dy && -dy >= 0){
  direction_change = -1;
}
// Q2
if (dy > dx && dx >= 0){
  x_direction = 0;
}
// Q3
if (dy > -dx && dx < 0){
  direction_change = -1;
  x_direction = 0;
}


// We need two loops for either looping over the x or y
if (x_direction){
  int y = y0;
  int sum = abs(dx) - abs(dy);
  int abs_dy = abs(dy);
  // Loop over the x direction
  for (int x = x0; x <= x1; x++){
    PutPixel(t, x, y, colour);
    // If the current d is negative, change the y value by -1 or 1
    if (d < 0) {
      y = y + direction_change;
      d = d + sum;
    }
    else
      // Keep decreasing the d
      d = d - abs_dy;
  }
}
else {
  int x = x0;
  int sum = abs(dy) - abs(dx);
  int abs_dx = abs(dx);
  // Loop over the y direction
  for (int y = y0; y <= y1; y++){
    PutPixel(t, x, y, colour);
    // If the current d is negative, change the x value by -1 or 1
    if (d < 0) {
      x = x + direction_change;
      d = d + sum;
    }
    else
      // Keep decreasing the d
      d = d - abs_dx;
  }
}
  return;
}
