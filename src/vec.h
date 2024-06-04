#ifndef VEC_H
#define VEC_H

#include <math.h>

class Vec3
{
public:
    float x, y, z;

    inline Vec3() : x{0}, y{0}, z{0} {};
    inline Vec3(float x, float y, float z) : x{x}, y{y}, z{z} {};

    inline Vec3 operator-() const 
    {
        return Vec3(-x, -y, -z);
    }

    inline Vec3& operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline Vec3& operator*=(const float t)
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    inline Vec3& operator/=(const float t)
    {
        *this *= (1/t);
        return *this;
    }

    inline float length_squared() const
    {
        return (x*x + y*y + z*z);
    }

    inline float length() const
    {
        return sqrtf(length_squared());
    }

    inline bool near_zero() const
    {
        float s = float(1e-8);
        return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s);
    }
};

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
    return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
    return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
    return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline Vec3 operator*(const float t, const Vec3& v)
{
    return Vec3(t * v.x, t * v.y, t * v.z);
}

inline Vec3 operator*(const Vec3& v, const float t)
{
    return t * v;
}

inline Vec3 operator/(const Vec3& v, const float t)
{
    return (1/t) * v;
}

inline float dot(const Vec3& u, const Vec3& v)
{
    return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

inline Vec3 cross(const Vec3& u, const Vec3& v)
{
    return Vec3(
        (u.y * v.z) - (u.z * v.y),
        (u.z * v.x) - (u.x * v.z),
        (u.x * v.y) - (u.y * v.x)
    );
}

inline Vec3 unit_vector(const Vec3& v)
{
    return (v / v.length());
}

using Point3 = Vec3;
using Color3 = Vec3;

#endif