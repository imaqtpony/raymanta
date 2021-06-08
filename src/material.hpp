#pragma once

#include <iostream>
#include <algorithm>

#include "vec3.hpp"
#include "constants.hpp"

enum MatType
{
    DIFFUSE,
    SPECULAR,
    REFRACT,
};

template <typename T>
struct material
{
    using color = vec3<T>;
    using byte = unsigned char[3];

    color emission, diffuse;
    MatType type;
    T refractionAngle = (T)0.;

    static byte ColorToPixel(const color &c)
    {
        byte ctp(T t)
        {
            return (byte)round(pow(clamp(t, 0., 1.), 1. / GAMMA) * 255);
        }
        return {ctp(c.x), ctp(c.y), ctp(c.z)};
    }
};

template <typename T>
std::ostream &operator<<(const std::ostream &os, material<T> &mat)
{
    return os << "M(" << mat.diffuse << ", " << mat.emission << ", " << mat.type << ")";
}