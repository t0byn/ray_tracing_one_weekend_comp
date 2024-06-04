#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "rt.h"

#include "material.h"

#define MAX_SPHERE_NUMBER 500

struct Sphere
{
    Point3 center;
    float radius;
    MaterialHandle material;
    unsigned int _padding_1;
    unsigned int _padding_2;
};

struct SphereSSBO
{
    int count;
    unsigned int _padding_1;
    unsigned int _padding_2;
    unsigned int _padding_3;
    Sphere sphere[MAX_SPHERE_NUMBER];
};

SphereSSBO sphere_buffer{};
inline void add_sphere(Point3 center, float radius, MaterialHandle material)
{
    assert(sphere_buffer.count < MAX_SPHERE_NUMBER);
    sphere_buffer.sphere[sphere_buffer.count] = {center, radius, material, 0, 0};
    sphere_buffer.count++;
}

#endif