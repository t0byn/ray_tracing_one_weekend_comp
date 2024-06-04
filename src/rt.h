#ifndef RT_H
#define RT_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

// constants

const float pi = 3.1415926535897932385f;

// utility function

inline float degree_to_radian(const float degree)
{
    return degree * pi / 180;
}

// generate a random number in the range [0, 1)
inline float random_float()
{
    return (rand() / (RAND_MAX + 1.0f));
}

// generate a random number in the range [min, max)
inline float random_float(float min, float max)
{
    return min + (max - min) * random_float();
}

#include "vec.h"
//#include "interval.h"

#endif