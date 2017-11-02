/* Computer Graphics assignment, Triangle Rasterization
 * Filename ........ trirast.c
 * Description ..... Implements triangle rasterization
 * Created by ...... Paul Melis
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "types.h"

float f(float x, float y, float x0, float y0, float x1, float y1) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
  }

/*
 * Rasterize a single triangle.
 * The triangle is specified by its corner coordinates
 * (x0,y0), (x1,y1) and (x2,y2).
 * The triangle is drawn in color (r,g,b).
 */

 // Baseline 40000 triangles per second
void
draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
    // Naively loop over the whole window
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 128; x++ ) {
            float alpha = f(x, y, x1, y1, x2, y2) / f(x0, y0, x1, y1, x2, y2);
            float beta = f(x, y, x2, y2, x0, y0) / f(x1, y1, x2, y2, x0, y0);
            float gamma = f(x, y, x0, y0, x1, y1) / f(x2, y2, x0, y0, x1, y1);

            // Check if the point is in the triangle
            if (alpha >= 0  && beta >= 0 && gamma >= 0) {
                // Check if we should draw the pixel
                if ((alpha > 0 || f(-1, -1, x1, y1, x2, y2) * f(x0, y0, x1, y1, x2, y2) > 0) && 
                    (beta > 0  || f(-1, -1, x2, y2, x0, y0) * f(x1, y1, x2, y2, x0, y0) > 0) && 
                    (gamma > 0 || f(-1, -1, x0, y0, x1, y1) * f(x2, y2, x0, y0, x1, y1) > 0)) {
                    PutPixel(x, y, r, g, b);
                    // Gourad color scheme
                    // PutPixel(w, h, 255*alpha, 255*beta, 255*gamma);
                }
            }
        } 
    }    
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
}
