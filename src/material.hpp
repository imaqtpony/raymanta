#pragma once

#include <iostream>
#include <algorithm>

#include "vec3.hpp"
#include "constants.hpp"

struct Material
{
    //predefined types of mat
    enum MatType
    {
        DIFFUSE,
        SPECULAR,
        REFRACT,
    };

    using color = Vec3<real_t>;
    using ubyte = unsigned char;

    color emission, diffuse;
    MatType type;
    real_t refractionAngle = (real_t)0.;

    static ubyte ctp(real_t t)
    {
        return (ubyte)std::round(std::pow(std::clamp(t, 0., 1.), 1. / GAMMA) * 255);
    }

    //returning rgb to pixel color values
    static Vec3<ubyte> colorToPixel(const color &c)
    {
        return {ctp(c.x), ctp(c.y), ctp(c.z)};
    }
};

inline std::ostream &operator<<(std::ostream &os, Material &mat)
{
    return os << "M(" << mat.diffuse << ", " << mat.emission << ", " << mat.type << ")";
}