#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "types.h"
#include "bvh.h"

unsigned long long num_rays_shot;
unsigned long long num_shadow_rays_shot;
unsigned long long num_triangles_tested;
unsigned long long num_bboxes_tested;

int intersection_copy(intersection_point *p1, intersection_point *p2);

int bvh_traverse(intersection_point* ip, vec3 ray_origin, vec3 ray_direction, bvh_node *current, float t_min_, float t_max_);

int     find_first_intersection(intersection_point *ip,
            vec3 ray_origin, vec3 ray_direction);

int static find_first_intersected_bvh_triangle(intersection_point *ip,
            vec3 ray_origin, vec3 ray_direction);

int     shadow_check(vec3 ray_origin, vec3 ray_direction);

#endif
