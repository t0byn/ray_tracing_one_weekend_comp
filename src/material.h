#ifndef MATERIAL_H
#define MATERIAL_H

#define MAX_LAMBERTIAN_MATERIAL_NUMBER 500 
#define MAX_METAL_MATERIAL_NUMBER 500 
#define MAX_DIELECTRIC_MATERIAL_NUMBER 500

#include "rt.h"

enum MaterialType
{
    LAMBERTIAN = 1,
    METAL,
    DIELECTRIC
};

struct MaterialHandle
{
    int type;
    int index;
};

struct LambertianMaterial
{
    Color3 albedo;
    unsigned int padding_1;
};
struct MetalMaterial
{
    Color3 albedo;
    float fuzz;
};
struct DielectricMaterial
{
    Color3 albedo;
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    float refractive_index;
};

struct LambertianSSBO
{
    int count;
    unsigned int _padding_1;
    unsigned int _padding_2;
    unsigned int _padding_3;
    LambertianMaterial materials[MAX_LAMBERTIAN_MATERIAL_NUMBER];
};
struct MetalSSBO
{
    int count;
    unsigned int _padding_1;
    unsigned int _padding_2;
    unsigned int _padding_3;
    MetalMaterial materials[MAX_METAL_MATERIAL_NUMBER];
};
struct DielectricSSBO
{
    int count;
    unsigned int _padding_1;
    unsigned int _padding_2;
    unsigned int _padding_3;
    DielectricMaterial materials[MAX_DIELECTRIC_MATERIAL_NUMBER];
};

LambertianSSBO lambertian_buffer{};
MetalSSBO metal_buffer{};
DielectricSSBO dielectric_buffer{};

inline MaterialHandle add_lambertian(Color3 albedo)
{
    assert(lambertian_buffer.count < MAX_LAMBERTIAN_MATERIAL_NUMBER);
    lambertian_buffer.materials[lambertian_buffer.count] = {albedo, 0};
    return MaterialHandle{
        LAMBERTIAN,
        lambertian_buffer.count++
    };
}

inline MaterialHandle add_metal(Color3 albedo, float fuzz)
{
    assert(metal_buffer.count < MAX_METAL_MATERIAL_NUMBER);
    metal_buffer.materials[metal_buffer.count] = {albedo, fuzz};
    return MaterialHandle{
        METAL,
        metal_buffer.count++
    };
}

inline MaterialHandle add_dielectric(Color3 albedo, float refractive_index)
{
    assert(dielectric_buffer.count < MAX_DIELECTRIC_MATERIAL_NUMBER);
    dielectric_buffer.materials[dielectric_buffer.count] = {albedo, refractive_index};
    return MaterialHandle{
        DIELECTRIC,
        dielectric_buffer.count++
    };
}

#endif