#pragma once

#include <iostream>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"

struct Camera
{
    using Point = Vec3<real_t>;
    using Direction = Vec3<real_t>;

    Point pos;
    int width, height;
    Direction dir;
    Point w, u, v;
    real_t aspectRatio;
    real_t clippingPlane;

    Camera(const Point &pos,
           const Direction &dir,
           const Direction &updir,
           int width,
           int height,
           real_t yFov);

    real_t getRandDif(rng_t &rng) const;

    Ray getRay(rng_t &rng, int p_x, int p_y) const;
};

std::ostream &operator<<(std::ostream &os, const Camera &c);