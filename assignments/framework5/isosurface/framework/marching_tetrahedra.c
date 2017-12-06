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

/* Using the given iso-value generate triangles for the tetrahedron
   defined by corner vertices v0, v1, v2, v3 of cell c.

   Store the resulting triangles in the "triangles" array.

   Return the number of triangles created (either 0, 1, or 2).

   Note: the output array "triangles" should have space for at least
         2 triangles.
*/

static int
generate_tetrahedron_triangles(triangle *triangles, unsigned char isovalue, cell c, int v0, int v1, int v2, int v3)
{
    vec3 points[] = {c.p[v0], c.p[v1], c.p[v2], c.p[v3]};
    double values[] = {c.value[v0], c.value[v1], c.value[v2], c.value[v3]};
 
    triangle tri;

    for (int i = 0; i < 4; i++) {
        // Only one under, others above
        if (values[i] < isovalue && values[(i + 1) % 4] > isovalue &&
            values[(i + 2) % 4] > isovalue && values[(i + 3) % 4] > isovalue) {
            
            tri.p[0] = interpolate_points(isovalue, points[i], points[(i + 1) % 4], values[i], values[(i + 1) % 4]);
            tri.p[1] = interpolate_points(isovalue, points[i], points[(i + 2) % 4], values[i], values[(i + 2) % 4]);
            tri.p[0] = interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
        }
        // Two above at same edge, others under
        if (values[i] > isovalue && values[(i + 1) % 4] > isovalue &&
            values[(i + 2) % 4] < isovalue && values[(i + 3) % 4] > isovalue) {
            //triangle 1
            interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 2) % 4], values[(i + 1) % 4], values[(i + 2) % 4]);
            interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 3) % 4], values[(i + 1) % 4], values[(i + 3) % 4]);
            interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
            
            interpolate_points(isovalue, points[(i + 1) % 4], points[(i + 3) % 4], values[(i + 1) % 4], values[(i + 3) % 4]);
            interpolate_points(isovalue, points[i], points[(i + 2) % 4], values[i], values[(i + 2) % 4]);
            interpolate_points(isovalue, points[i], points[(i + 3) % 4], values[i], values[(i + 3) % 4]);
        }
        // If the opposite sides are above, others under
        if (values[i] > isovalue && values[(i + 1) % 4] < isovalue &&
            values[(i + 2) % 4] > isovalue && values[(i + 3) % 4] < isovalue) {
                
        }

    }

    return 0;
}

/* Generate triangles for a single cell for the given iso-value. This function
   should produce at most 6 * 2 triangles (for which the "triangles" array should
   have enough space).

   Use calls to generate_tetrahedron_triangles().

   Return the number of triangles produced
*/

int
generate_cell_triangles(triangle *triangles, cell c, unsigned char isovalue)
{
    int n = 0;
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 3, 7);
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 1, 5, 7);
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 5, 7);
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 4, 6, 7);
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 3, 7);
    n += generate_tetrahedron_triangles(triangles, isovalue, c, 0, 2, 6, 7);

    return n;
}
