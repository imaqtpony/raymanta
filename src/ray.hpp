#pragma once

#include <iostream>
#include "vec3.hpp"

template <typename T>
struct ray
{
    using point = vec3<T>;
    using vec = vec3<T>;

    point start;
    vec dir;

    point at(T t)
    {
        return start + dir * t;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const ray<T> &r)
{
    return os << "R{" << r.start << ", " << r.dir << "}";
}