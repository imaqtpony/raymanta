#pragma once

#include <iostream>
#include <cmath>

#include "vec3.hpp"
#include "ray.hpp"

template <typename T>
struct Camera
{
    using Point = vec3<T>;
    using Direction = vec3<T>;

    Point pos;
    int width, height;
    Direction dir;
    Point w, u, v;
    T aspectRatio;
    T clippingPlane;

    Camera(const Point &pos,
           const Direction &dir,
           const Direction &updir,
           int width,
           int height,
           T yFov)
        : pos(pos),
          width(width),
          height(height),
          dir(dir.norm()),
          w(this->dir),
          u((w ^ updir).norm()),
          v(w ^ u),
          aspectRatio((T)width / height),
          clippingPlane(1. / tan(yFov * M_PI / 360.))
    {
    }

    template <typename RNG>
    T GetRandDif(RNG &rng) const
    {
        std::uniform_real_distribution uniform01(0., 1.);
        auto u = 2. * uniform01(rng);
        return u < 1. ? sqrt(u) - 1. : 1. - sqrt(2 - u);
    }

    template <typename RNG>
    Ray<T> GetRay(RNG &rng, int p_x, int p_y) const
    {
        auto dx = GetRandDif(rng);
        auto dy = GetRandDif(rng);

        T x = 2. * ((T)p_x + .5 + dx) / width - 1.;
        T y = 2. * ((T)p_y + .5 + dy) / height - 1.;

        Ray<T> res = {pos, w * clippingPlane};
        res.dir = u * x * aspectRatio + res.dir;
        res.dir = v * y + res.dir;
        res.dir = res.dir.norm();

        return res;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Camera<T> &c)
{
    return os << "Cam( pos: " << c.pos << " uvw: " << c.u << c.v << c.w << " width height: " << c.width << "|" << c.height << " aspect ratio: " << c.aspectRatio << " plane dist: " << c.clippingPlane << ")";
}