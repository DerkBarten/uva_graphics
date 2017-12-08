/* Computer Graphics, Assignment, Volume rendering with cubes/points/isosurface
 *
 * Student name .... Derk Barten
 * Student email ... derk.barten@student.uva.nl
 * Collegekaart .... 11043075
 * Date ............
 * Comments ........
 *
 * (always fill in these fields before submitting!!)
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "marching_tetrahedra.h"


/* Compute a linearly interpolated position where an isosurface cuts
   an edge between two vertices (p1 and p2), each with their own
   scalar value (v1 and v2) */

static vec3
interpolate_points(unsigned char isovalue, vec3 p1, vec3 p2, unsigned char v1, unsigned char v2)
{
    /* Initially, simply return the midpoint between p1 and p2.
       So no real interpolation is done yet */

    return v3_add(v3_multiply(p1, 0.5), v3_multiply(p2, 0.5));
}


void set_triangle_normal(triangle* t) {
    vec3 norm = v3_normalize(v3_crossprod(v3_subtract(t->p[1], t->p[0]),
                                          v3_subtract(t->p[0], t->p[0])));
    t->n[0] = norm;
    t->n[1] = norm;
    t->n[2] = norm;
}


static int
generate_triangle_corner(triangle *triangles, vec3 *points, double *values, unsigned char isovalue) {
    // Iterate over the corners of the tetrahedron
    for (int i = 0; i < 4; i++) {
        // // Only one under, others above
        if (values[i] < isovalue && values[(i + 1) % 4] > isovalue &&
            values[(i + 2) % 4] > isovalue && values[(i + 3) % 4] > isovalue) {

            triangles->p[0] = interpolate_points(isovalue, points[i], points[(i + 1) % 4], values[i], values[(i + 1) % 4]);
            triangles->p[1] = interpolate_points(isovalue, points[i], points[(i + 2) % 4], values[i], values[(i + 2) % 4]);
            triangles->p[2] = interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
            set_triangle_normal(triangles);
            triangles++;
            return 1;
        }
    }
    return 0;
}

static int
generate_triangle_edge(triangle *triangles, vec3 *points, double *values, unsigned char isovalue) {
    for (int i = 0; i < 4; i++) {
    // Two above at same edge, others under
        if (values[i] > isovalue && values[(i + 1) % 4] > isovalue &&
            values[(i + 2) % 4] < isovalue && values[(i + 3) % 4] > isovalue) {

            triangles->p[0] = interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 2) % 4], values[(i + 1) % 4], values[(i + 2) % 4]);
            triangles->p[1] = interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 3) % 4], values[(i + 1) % 4], values[(i + 3) % 4]);
            triangles->p[2] = interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
            set_triangle_normal(triangles);
            triangles++;

            triangles->p[0] = interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 3) % 4], values[(i + 1) % 4], values[(i + 3) % 4]);
            triangles->p[1] = interpolate_points(isovalue, points[i], points[(i + 2) % 4], values[i], values[(i + 2) % 4]);
            triangles->p[2] = interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
            set_triangle_normal(triangles);
            triangles++;
            return 2;
        }
    }
    return 0;
}

static int
generate_triangle_square(triangle *triangles, vec3 *points, double *values, unsigned char isovalue) {
    // Iterate over the corners of the tetrahedron
    for (int i = 0; i < 4; i++) {
        // // If the opposite sides are above, others under
        if (values[i] > isovalue && values[(i + 1) % 4] < isovalue &&
            values[(i + 2) % 4] > isovalue && values[(i + 3) % 4] < isovalue) {

            // right triangle (in assignment)
            triangles->p[0] = interpolate_points(isovalue, points[i], points[(i + 1) % 4], values[i], values[(i + 1) % 4]);
            triangles->p[1] = interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
            triangles->p[2] = interpolate_points(isovalue, points[(i + 2) % 4], points[(i + 3) % 4], values[(i + 2) % 4], values[(i + 3) % 4]);
            set_triangle_normal(triangles);
            triangles++;

            // left triangle (in assignment)
            triangles->p[0] = interpolate_points(isovalue, points[i], points[(i + 1) % 4], values[i], values[(i + 1) % 4]);
            triangles->p[1] = interpolate_points(isovalue, points[(i + 2) % 4], points[(i + 3) % 4], values[(i + 2) % 4], values[(i + 3) % 4]);
            triangles->p[2] = interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 2) % 4], values[(i + 1) % 4], values[(i + 2) % 4]);
            set_triangle_normal(triangles);
            triangles++;
            return 2;
        }
    }
    return 0;
}

static int
generate_tetrahedron_triangles(triangle *triangles, unsigned char isovalue, cell c, int v0, int v1, int v2, int v3)
{
    vec3 points[] = {c.p[v0], c.p[v1], c.p[v2], c.p[v3]};
    double values[] = {c.value[v0], c.value[v1], c.value[v2], c.value[v3]};
    int n = 0;
    // save the starting position of the triangles array
    triangle *t = triangles;
    n += generate_triangle_corner(triangles, points, values, isovalue);
    n += generate_triangle_edge(triangles, points, values, isovalue);
    n += generate_triangle_square(triangles, points, values, isovalue);
    // point the triangle array back to its start
    triangles = t;
    printf("n: %i\n", n);
    return n;
}

int
generate_cell_triangles(triangle *triangles, cell c, unsigned char isovalue)
{
    int n = 0;
    // T1
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 3, 7);
    // T2
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 6, 7);
    // T3
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 5, 7);
    // T4
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 3, 7);
    // T5
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 5, 7);
    // T6
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 6, 7);

    return n;
}

