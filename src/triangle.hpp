#pragma once

#include <iostream>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"

struct Triangle
{
    using point = Vec3<real_t>;
    using vec = Vec3<real_t>;

    point a, b, c;
    vec ab, ac, n;

    Triangle(const point &a, const point &b, const point &c)
        : a(a), b(b), c(c), ab(b - a), ac(c - a), n((ab ^ ac).norm())
    {
    }

    real_t intersect(const Ray &p_ray) const
    {
        vec pvec = p_ray.dir ^ ac;
        real_t det = ab.dot(pvec);
        if (std::abs(det) < EPSILON)
            return -1;

        real_t invdet = 1. / det;
        vec tvec = p_ray.start - a;
        real_t u = tvec.dot(pvec) * invdet;

        if (u < 0. || u > 1.)
            return -1;

        vec qvec = tvec ^ ab;
        real_t v = p_ray.dir.dot(qvec) * invdet;

        if (v < 0. || u + v > 1.)
            return -1;

        real_t t = ac.dot(qvec) * invdet;
        if (t < EPSILON)
            return -1;

        return t;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Triangle &t)
{
    return os << "T{" << t.a << ", " << t.b << ", " << t.c << ",n=" << t.n << "}";
}