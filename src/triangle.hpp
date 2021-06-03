#pragma once

#include <iostream>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"

template <typename T>
struct triangle
{
    using point = vec3<T>;
    using vec = vec3<T>;

    point a, b, c;
    vec ab, ac, n;

    triangle(const point &a, const point &b, const point &c)
        : a(a), b(b), c(c), ab(b - a), ac(c - a), n((ab ^ ac).norm())
    {
    }

    T intersect(const ray<T> &p_ray)
    {
        vec pvec = p_ray.dir ^ ac;
        T det = ab.dot(pvec);
        if (std::abs(det) < EPSILON)
            return -1;

        T invdet = 1. / det;
        vec tvec = p_ray.start - a;
        T u = tvec.dot(pvec) * invdet;

        if (u < 0. || u > 1.)
            return -1;

        vec qvec = tvec ^ ab;
        T v = p_ray.dir.dot(qvec) * invdet;

        if (v < 0. || u + v > 1.)
            return -1;

        T t = ac.dot(qvec) * invdet;
        if (t < EPSILON)
            return -1;

        return t;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const triangle<T> &t)
{
    return os << "T{" << t.a << ", " << t.b << ", " << t.c << ",n=" << t.n << "}";
}