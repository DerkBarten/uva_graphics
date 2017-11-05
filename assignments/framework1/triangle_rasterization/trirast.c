/* Computer Graphics assignment, Triangle Rasterization
 * Filename ........ trirast.c
 * Description ..... Implements triangle rasterization
 * Created by ...... Paul Melis
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "types.h"

// The formula of a line
float f(float x, float y, float x0, float y0, float x1, float y1) {
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
  }

float float_max(float a, float b, float c)
{
    float m = a;
    if(m < b)
        m = b;
    if(m < c)
        m = c;
    return m;
}

float  float_min(float a, float b, float c)
{
    float m = a;
    if(m > b)
        m = b;
    if(m > c)
        m = c;
    return m;
}

/*
 * Rasterize a single triangle.
 * The triangle is specified by its corner coordinates
 * (x0,y0), (x1,y1) and (x2,y2).
 * The triangle is drawn in color (r,g,b).
 */

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
                    // PutPixel(x, y, 255*alpha, 255*beta, 255*gamma);
                }
            }
        } 
    }    
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
    // Create a bounding box around the triangle
    int x_min = (int)float_min(x0, x1, x2);
    int x_max = (int)(float_max(x0, x1, x2) + 1);
    int y_min = (int)float_min(y0, y1, y2);
    int y_max = (int)(float_max(y0, y1, y2) + 1);

    // Get the f value of the starting point
    float d_alpha_base = f(x_min, y_min, x1, y1, x2, y2) ;
    float d_beta_base = f(x_min, y_min, x2, y2, x0, y0) ;
    float d_gamma_base = f(x_min, y_min, x0, y0, x1, y1);

    // These variables will hold the incremental delta value
    float d_alpha_x, d_alpha_y = 0;
    float d_beta_x, d_beta_y = 0;
    float d_gamma_x, d_gamma_y = 0;

    // These constants are removed from the loop
    float f_alpha = f(x0, y0, x1, y1, x2, y2);
    float f_beta = f(x1, y1, x2, y2, x0, y0);
    float f_gamma = f(x2, y2, x0, y0, x1, y1);
    
    // Initializing outside the loop might save some performance
    float alpha, beta, gamma = 0;

    // These booleans are constant so remove them from the loop
    int alpha_check =  f(-1, -1, x1, y1, x2, y2) * f(x0, y0, x1, y1, x2, y2) > 0;
    int beta_check = f(-1, -1, x2, y2, x0, y0) * f(x1, y1, x2, y2, x0, y0) > 0;
    int gamma_check = f(-1, -1, x0, y0, x1, y1) * f(x2, y2, x0, y0, x1, y1) > 0;

    int in_triangle = 0;
    // Loop over the bounding box around the triangle
    for (int y = y_min; y < y_max; y++) {
        for (int x = x_min; x < x_max; x++ ) {

            // Calculate the alpha/beta/gamma values beloning to the point
            alpha = (d_alpha_base + d_alpha_x  + d_alpha_y)  / f_alpha;
            beta = (d_beta_base + d_beta_x + d_beta_y) / f_beta;
            gamma = (d_gamma_base + d_gamma_x + d_gamma_y) / f_gamma;
            
            // Check for an early exit if we know we exited the triangle
            if ((alpha < 0 || beta < 0 || gamma < 0 ) && in_triangle){
                break;
            }

            // Check if the point is in the triangle
            if (alpha >= 0  && beta >= 0 && gamma >= 0) {
                // Check if we should draw the pixel
                if ((alpha > 0 || alpha_check) && 
                    (beta > 0  || beta_check) && 
                    (gamma > 0 || gamma_check)) {
                    PutPixel(x, y, r, g, b);
                    // Gourad color scheme
                    // PutPixel(x, y, 255*alpha, 255*beta, 255*gamma);
                }
                in_triangle = 1;
            }
            // Instead of recalculating the whole f, just increase the value d
            d_alpha_x += (y1 - y2);
            d_beta_x += (y2 - y0);
            d_gamma_x += (y0 - y1); 
        }
        // Reset the increments done in the x direction
        d_alpha_x = 0;
        d_beta_x= 0;
        d_gamma_x = 0;

        d_alpha_y +=  (x2 - x1);
        d_beta_y += (x0 - x2);
        d_gamma_y += (x1 - x0);

        in_triangle = 0;
    }    
}
