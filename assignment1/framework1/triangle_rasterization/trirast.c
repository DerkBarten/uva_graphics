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

/*
 * Rasterize a single triangle.
 * The triangle is specified by its corner coordinates
 * (x0,y0), (x1,y1) and (x2,y2).
 * The triangle is drawn in color (r,g,b).
 */
    /*
    printf("Original: \n");
    printf("[ %f %f ]\n", p00, p01);
    printf("[ %f %f]\n", p10, p11);
    printf("Inverse: \n");
    printf("[ %f %f ]\n", p_inv00, p_inv10);
    printf("[ %f %f]\n\n", p_inv01, p_inv11);
    */
 
void
draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
    PutPixel(10, 10, 255, 255, 255);
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

    for (int h = 0; h < 64; h++) {
        for (int w = 0; w < 128; w++ ) {
            float px = w - x0;
            float py = h - y0;
            
            float beta = p_inv00 * px + p_inv10 * py;
            float gamma = p_inv01 * px + p_inv11 * py;
            float alpha = 1 - beta - gamma;

            if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1  && gamma >= 0 && gamma <= 1) {
                PutPixel(w, h, r, g, b);
                //PutPixel(w, h, 255*alpha, 255*beta, 255*gamma);
            }
        } 
    }    
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
}
