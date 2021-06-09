#pragma once

#include <iostream>
#include "vec3.hpp"

struct Ray
{
    using point = Vec3<real_t>;
    using vec = Vec3<real_t>;

    point start;
    vec dir;

    point at(real_t t) const
    {
        return start + dir * t;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r)
{
    return os << "R{" << r.start << ", " << r.dir << "}";
}