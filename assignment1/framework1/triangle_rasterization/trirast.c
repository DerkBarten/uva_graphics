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

int f(int x, int y, int x0, int y0, int x1, int y1) {
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
    // Create the matrix of (P1-P0), (P2-P0)
    float p00 = x1 - x0;
    float p01 = y1 - y0;
    float p10 = x2 - x0;
    float p11 = y2 - y0;

    // Caluclate the inverse matrix
    float det = 1.0/(p00 * p11 - p10 * p01);
    float p_inv00 = det * p11;
    float p_inv01 = det * -p01;
    float p_inv10 = det * -p10;
    float p_inv11 = det * p00;

    // Naively loop over the whole window
    for (int h = 0; h < 64; h++) {
        for (int w = 0; w < 128; w++ ) {
            float px = w - x0;
            float py = h - y0;
            
            // x1y1
            float beta = p_inv00 * px + p_inv10 * py;
            // x2y2
            float gamma = p_inv01 * px + p_inv11 * py;
            // x0y0
            float alpha = 1 - beta - gamma;

            // Check if the point is in the triangle
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                // Check if we should draw the pixel
                if ((alpha > 0 || f(-1, -1, x1, y1, x2, y2) * f(x0, y0, x1, y1, x2, y2) > 0) && 
                    (beta > 0  || f(-1, -1, x2, y2, x0, y0) * f(x1, y1, x2, y2, x0, y0) > 0) && 
                    (gamma > 0 || f(-1, -1, x0, y0, x1, y1) * f(x2, y2, x0, y0, x1, y1) > 0)) {
                    PutPixel(w, h, r, g, b);
                    // Gourad color scheme
                    // PutPixel(w, h, 255*alpha, 255*beta, 255*gamma);
                }
                // if (f(w, h, x1, y1, x2, y2) == 0.0 ||
                //     f(w, h, x2, y2, x0, y0) == 0.0 ||
                //     f(w, h, x0, y0, x1, y1) == 0.0) {
                //         PutPixel(w, h, r, g, b);
                // }
            }
        } 
    }    
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
}
