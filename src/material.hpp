#pragma once

#include <iostream>
#include <algorithm>

#include "vec3.hpp"
#include "constants.hpp"

template <typename T>
struct material
{
    enum MatType
    {
        DIFFUSE,
        SPECULAR,
        REFRACT,
    };

    using color = vec3<T>;
    using ubyte = unsigned char;

    color emission, diffuse;
    MatType type;
    T refractionAngle = (T)0.;

    static ubyte ctp(T t)
    {
        return (ubyte)std::round(std::pow(std::clamp(t, 0., 1.), 1. / GAMMA) * 255);
    }

    static vec3<ubyte> colorToPixel(const color &c)
    {
        return {ctp(c.x), ctp(c.y), ctp(c.z)};
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, material<T> &mat)
{
    return os << "M(" << mat.diffuse << ", " << mat.emission << ", " << mat.type << ")";
}