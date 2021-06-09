#include "camera.hpp"

Camera::Camera(const Point &pos,
               const Direction &dir,
               const Direction &updir,
               int width,
               int height,
               real_t yFov)
    : pos(pos),
      width(width),
      height(height),
      dir(dir.norm()),
      w(this->dir),
      u((w ^ updir).norm()),
      v(w ^ u),
      aspectRatio((real_t)width / height),
      clippingPlane(1. / tan(yFov * PI / 360.))
{
}

real_t Camera::getRandDif(rng_t &rng) const
{
    std::uniform_real_distribution uniform01(0., 1.);
    auto u = 2. * uniform01(rng);
    return u < 1. ? sqrt(u) - 1. : 1. - sqrt(2 - u);
}

Ray Camera::getRay(rng_t &rng, int p_x, int p_y) const
{
    auto dx = getRandDif(rng);
    auto dy = getRandDif(rng);

    real_t x = 2. * ((real_t)p_x + .5 + dx) / width - 1.;
    real_t y = 2. * ((real_t)p_y + .5 + dy) / height - 1.;

    Ray res = {pos, w * clippingPlane};
    res.dir = u * x * aspectRatio + res.dir;
    res.dir = v * y + res.dir;
    res.dir = res.dir.norm();

    return res;
}

std::ostream &operator<<(std::ostream &os, const Camera &c)
{
    return os << "Cam( pos: " << c.pos << " uvw: " << c.u << c.v << c.w << " width height: " << c.width << "|" << c.height << " aspect ratio: " << c.aspectRatio << " plane dist: " << c.clippingPlane << ")";
}