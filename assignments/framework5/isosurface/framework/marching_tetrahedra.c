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
    // Calculate the ratio of the distances from both points to the isovalue
    float ratio = ((float)isovalue - (float)v2) / ((float)v1 - (float)v2);

    return v3_add(v3_multiply(p1, ratio), v3_multiply(p2, 1.0 - ratio));
}

// Create a surface normal from a triangle
void set_triangle_normal(triangle* t) {
    vec3 norm = v3_normalize(v3_crossprod(v3_subtract(t->p[0], t->p[2]),
                                          v3_subtract(t->p[1], t->p[2])));
    t->n[0] = norm;
    t->n[1] = norm;
    t->n[2] = norm;
}


static triangle
generate_triangle_corner(unsigned char isovalue, cell c, int v0, int v1, int v2, int v3) {
    triangle t;
    t.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
    t.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
    t.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
    set_triangle_normal(&t);
    return t;
}

static triangle
generate_triangle_square(unsigned char isovalue, cell c, int v0, int v1, int v2, int v3) {
    triangle t;
    t.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
    t.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);
    t.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
    set_triangle_normal(&t);
    return t;
}

static int
generate_tetrahedron_triangles(triangle *triangles, unsigned char isovalue, cell c, int v0, int v1, int v2, int v3)
{
    unsigned char mask = 0x0;

    // Create a mask for differentiating between triangle configurations
    if (isovalue < c.value[v0])
        mask = mask | 0x1;
    if (isovalue < c.value[v1])
        mask = mask | 0x2;
    if (isovalue < c.value[v2])
        mask = mask | 0x4;
    if (isovalue < c.value[v3])
        mask = mask | 0x8;
    
    // 0000 and 1111
    if (mask == 0x0 || mask == 0xF) { 
        return 0;
    }
    // 0001 and 1110
    if (mask == 0x1 || mask == 0xE) { 
        *triangles = generate_triangle_corner(isovalue, c, v0, v1, v2, v3);
        return 1;
    }
    // 0010 and 1101
    if (mask == 0x2 || mask == 0xD) { 
        *triangles = generate_triangle_corner(isovalue, c, v1, v0, v2, v3);
        return 1;
    }
    // 0011 and 1100
    if (mask == 0x3 || mask == 0xC) {
        *triangles = generate_triangle_square(isovalue, c, v2, v0, v3, v1);
        *(triangles+1) = generate_triangle_square(isovalue, c, v3, v1, v2, v0);
        return 2;
    }
    // 0100 and 1011
    if (mask == 0x4 || mask == 0xB) {
        *triangles = generate_triangle_corner(isovalue, c, v2, v0, v1, v3);
        return 1;
    }
    // 0101 and 1010
    if (mask == 0x5 || mask == 0xA) {
        *triangles = generate_triangle_square(isovalue, c, v2, v3, v0, v1);
        *(triangles+1) = generate_triangle_square(isovalue, c, v3, v2, v1, v0);
        return 2;
    }
    // 0110 and 1001
    if (mask == 0x6 || mask == 0x9) {
        *triangles = generate_triangle_square(isovalue, c, v3, v1, v0, v2);
        *(triangles+1) = generate_triangle_square(isovalue, c, v1, v3, v2, v0);
        return 2;
    }
    // 0111 and 0100
    if (mask == 0x7 || mask == 0x8) {
        *triangles = generate_triangle_corner(isovalue, c, v3, v0, v1, v2);
        return 1;
    }
    return 0;
}

int
generate_cell_triangles(triangle *triangles, cell c, unsigned char isovalue)
{
    // The tetrahedron representation
    int t[6][4] = {
        {0, 1, 3, 7}, // T1
        {0, 2, 6, 7}, // T2
        {0, 1, 5, 7}, // T3
        {0, 2, 3, 7}, // T4
        {0, 4, 5, 7}, // T5
        {0, 4, 6, 7}  // T6
    };
    
    int n = 0;
    // Loop over the 6 tetrahedron variants
    for (int i = 0; i < 6; i++) {
        n += generate_tetrahedron_triangles(triangles + n, isovalue, c,
                                            t[i][0], t[i][1],t[i][2],t[i][3]);
    }
    return n;
}

