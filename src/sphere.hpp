#pragma once

#include <iostream>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"
#include "constants.hpp"

template <typename T>
struct sphere
{
    using Point = vec3<T>;

    Point center;
    T radius;

    vec3<T> normal(const Point &p) const
    {
        return (p - center).norm();
    }

    T Intersect(const Ray<T> &ray) const
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

template <typename T>
std::ostream &operator<<(std::ostream &os, const sphere<T> &s)
{
    return os << "S(" << s.center << ", " << s.radius << ")";
}