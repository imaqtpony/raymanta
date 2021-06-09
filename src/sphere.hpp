#pragma once

#include <iostream>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"
#include "constants.hpp"

struct Sphere
{
    using Point = Vec3<real_t>;

    Point center;
    real_t radius;

    Vec3<real_t> normal(const Point &p) const
    {
        return (p - center).norm();
    }

    real_t intersect(const Ray &ray) const
    {
        auto cs = center - ray.start;
        auto b = cs.dot(ray.dir);
        auto det = b * b - cs.length2() + radius * radius;

        if (det < 0)
            return -1;

        det = sqrt(det);
        auto t1 = b - det;

        if (t1 > EPSILON)
            return t1;

        t1 = b + det;
        if (t1 > EPSILON)
            return t1;

        return -1;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Sphere &s)
{
    return os << "S(" << s.center << ", " << s.radius << ")";
}