/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ bezier.c
 * Description ..... Bezier curves
 * Date ............ 22.07.2009
 * Created by ...... Paul Melis
 *
 * Student name .... Derk Barten
 * Student email ... derk.barten@student.uva.nl
 * Collegekaart .... 11043075
 * Date ............ 
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <math.h>
#include "bezier.h"
#include <stdio.h>
#include <stdlib.h>

/* Given a Bezier curve defined by the 'num_points' control points
 * in 'p' compute the position of the point on the curve for parameter
 * value 'u'.
 *
 * Return the x and y values of the point by setting *x and *y,
 * respectively.
 */

int factorial(int n) {
    int result = 1;

    // Enable negative factorials
    if (n < 0) {
        result *= -1;
        n = abs(n);
        printf("Negative Factorial\n");
    }

    for (; n > 0; n--) {
        result *= n;
    }
    return result;
}

float bernstein(int n, int i, float u) {
    return binomial(n, i) * pow(u, i) * pow(1.0 - u, n - i);
}

float binomial(int n, int k) {
    return (float)factorial(n) / (float)(factorial(k) * factorial(n - k));
}

void
evaluate_bezier_curve(float *x, float *y, control_point p[], int num_points, float u)
{
    *x = 0.0;
    *y = 0.0;
    int n = num_points - 1;

    for (int i = 0; i <= n; i++) {
        float b = bernstein(n, i, u);
        *x += b * p[i].x;
        *y += b * p[i].y; 
    }
}

/* Draw a Bezier curve defined by the control points in p[], which
 * will contain 'num_points' points.
 *
 *
 * The 'num_segments' parameter determines the "discretization" of the Bezier
 * curve and is the number of straight line segments that should be used
 * to approximate the curve.
 *
 * Call evaluate_bezier_curve() to compute the necessary points on
 * the curve.
 *
 * You will have to create a GLfloat array of the right size, filled with the
 * vertices in the appropriate format and bind this to the buffer.
 */

void
draw_bezier_curve(int num_segments, control_point p[], int num_points)
{
    double segment_size = 1.0 / num_segments;
    // Every line segment is expressed by four floats
    int array_size = sizeof(float) * 4 * num_segments;
    // The number of vertices
    int indices = num_segments * 2;
    // Memory needed to hold the vertices
    GLfloat array[array_size];

    double b = 0.0;
    // Reuse the control point struct to hold xy coordinates
    control_point p1;
    control_point p2;
    // Calculate the starting position of the curve
    evaluate_bezier_curve(&p1.x, &p1.y, p, num_points, 0.0);

    for (int i = 0; i < num_segments; i++) {
        b += segment_size;
        
        evaluate_bezier_curve(&p2.x, &p2.y, p, num_points, b);
        // Add the start and end coordinates of the line to the array
        array[i*4] = p1.x;
        array[(i*4)+1] = p1.y;
        array[(i*4)+2] = p2.x;
        array[(i*4)+3] = p2.y;

        // End point of the previous line becomes the start of the next line
        p1.x = p2.x;
        p1.y = p2.y;
    }
    GLuint buffer[1];

    // This creates the VBO and binds an array to it.
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);

    glBufferData(GL_ARRAY_BUFFER, array_size,
                 array, GL_STATIC_DRAW);

    // This tells OpenGL to draw what is in the buffer as a Line Strip.
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINE_STRIP, 0, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, buffer);
}

/* Find the intersection of a cubic Bezier curve with the line X=x.
   Return 1 if an intersection was found and place the corresponding y
   value in *y.
   Return 0 if no intersection exists.
*/

int
intersect_cubic_bezier_curve(float *y, control_point p[], float x)
{
    return 0;
}
