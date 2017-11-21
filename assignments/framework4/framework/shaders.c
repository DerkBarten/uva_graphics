/* Computer Graphics and Game Technology, Assignment Ray-tracing
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "shaders.h"
#include "perlin.h"
#include "v3math.h"
#include "intersection.h"
#include "scene.h"
#include "quat.h"
#include "constants.h"

// shade_constant()
//
// Always return the same color. This shader does no real computations
// based on normal, light position, etc. As such, it merely creates
// a "silhouette" of an object.

float REFLECTION_OFFSET = 0.0001;

float
max(vec3 color)
{
    return fmax(color.x, fmax(color.y, color.z));
}

vec3
color_range(vec3 color)
{    
    vec3 v = color;
    float m = max(color);
    if (m > 1.0){
        v.x = color.x / m;
        v.y = color.y / m;
        v.z = color.z / m;
    }
    return v;
}

vec3
shade_constant(intersection_point ip)
{
    return v3_create(1, 0, 0);
}

vec3
shade_matte(intersection_point ip)
{
    vec3 color = v3_create(0, 0, 0);
    
    for (int i = 0; i < scene_num_lights; i++) {
        light light_src = scene_lights[i];
        vec3 l = v3_normalize(v3_subtract(light_src.position, ip.p));

        vec3 ray_origin = v3_add(ip.p, v3_multiply(ip.n, REFLECTION_OFFSET));
        if (shadow_check(ray_origin, l)) {
            continue;
        }

        float c = light_src.intensity * fmax(0, v3_dotprod(ip.n, l));
        color.x += c;
        color.y += c;
        color.z += c;
    }
    color.x += scene_ambient_light;
    color.y += scene_ambient_light;
    color.z += scene_ambient_light;

    return color_range(color);
}

vec3
shade_blinn_phong(intersection_point ip)
{
    vec3 cd = v3_create(1, 0, 0);
    vec3 cs = v3_create(1, 1, 1);

    float kd = 0.8;
    float ks = 0.5;
    float a = 50;

    float phong = 0.0;
    float matte = 0.0;

    for (int i = 0; i < scene_num_lights; i++) {
        light light_src = scene_lights[i];
        vec3 l = v3_normalize(v3_subtract(light_src.position, ip.p));
        vec3 e = v3_normalize(v3_subtract(scene_camera_position, ip.p));
        vec3 h = v3_normalize(v3_add(l, e));

        vec3 ray_origin = v3_add(ip.p, v3_multiply(ip.n, 0.0001));
        if (shadow_check(ray_origin, l)) {
            continue;
        }

        phong += light_src.intensity * (pow(v3_dotprod(ip.n, h), a));
        matte += light_src.intensity * fmax(0, v3_dotprod(ip.n, l));
    }

    return
    color_range(v3_add(v3_multiply(cd, scene_ambient_light + kd * matte),
           v3_multiply(cs, ks * phong)));
}

vec3
shade_reflection(intersection_point ip)
{
    float matte = 0.0;
    vec3 refl;
    for (int i = 0; i < scene_num_lights; i++) {
        light light_src = scene_lights[i];
        vec3 l = v3_normalize(v3_subtract(light_src.position, ip.p));
        matte += light_src.intensity * fmax(0, v3_dotprod(ip.n, l));
    }
    // TODO check if > 1;
    matte += scene_ambient_light;

    vec3 r = v3_subtract(v3_multiply(v3_multiply(ip.n, 2),  v3_dotprod(ip.i, ip.n)), ip.i);
    refl = ray_color(ip.ray_level + 1, v3_add(ip.p, v3_multiply(ip.n, REFLECTION_OFFSET)), r);

    return v3_add(v3_multiply(refl, 0.25), v3_multiply(v3_create(matte, matte, matte), 0.75));
}

// Returns the shaded color for the given point to shade.
// Calls the relevant shading function based on the material index.
vec3
shade(intersection_point ip)
{
  switch (ip.material)
  {
    case 0:
      return shade_constant(ip);
    case 1:
      return shade_matte(ip);
    case 2:
      return shade_blinn_phong(ip);
    case 3:
      return shade_reflection(ip);
    default:
      return shade_constant(ip);

  }
}

// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3
ray_color(int level, vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    // If this ray has been reflected too many times, simply
    // return the background color.
    if (level >= 3)
        return scene_background_color;

    // Check if the ray intersects anything in the scene
    if (find_first_intersection(&ip, ray_origin, ray_direction))
    {
        // Shade the found intersection point
        ip.ray_level = level;
        return shade(ip);
    }

    // Nothing was hit, return background color
    return scene_background_color;
}
