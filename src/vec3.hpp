#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

#include "constants.hpp"

template <typename T>
struct Vec3
{
    T x, y, z;

    Vec3 operator-() const
    {
        return {-x, -y, -z};
    }

    Vec3 operator+(const Vec3 &o) const
    {
        return {x + o.x, y + o.y, z + o.z};
    }

    Vec3 operator-(const Vec3 &o) const
    {
        return {x - o.x, y - o.y, z - o.z};
    }

    template <typename OT>
    Vec3 operator*(const OT o) const
    {
        return {x * o, y * o, z * o};
    }

    template <typename OT>
    Vec3 operator/(const OT o) const
    {
        return {x / o, y / o, z / o};
    }

    Vec3 operator*(const Vec3 &o) const
    {
        return {x * o.x, y * o.y, z * o.z};
    }

    Vec3 operator^(const Vec3 &o) const
    {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }

    T dot(const Vec3 &o) const
    {
        return x * o.x + y * o.y + z * o.z;
    }

    T length2() const
    {
        return dot(*this);
    }

    T length() const
    {
        return std::sqrt(length2());
    }

    Vec3 norm() const
    {
        return *this / length();
    }

    T max() const
    {
        return std::max({x, y, z});
    }

    T min() const
    {
        return std::min({x, y, z});
    }

    Vec3 reflect(const Vec3 &normal) const
    {
        return *this - normal * dot(normal) * 2;
    }

    Vec3 transmit(const Vec3 &normal, T ni, T nt) const
    {
        auto nint = ni / nt;
        auto cosi = -dot(normal);
        auto sin2t = nint * nint * (1. - cosi * cosi);
        auto res = *this * nint;
        res = normal * (nint * cosi - std::sqrt(1 - sin2t)) + res;
        return res.norm();
    }

    template <typename RNG>
    Vec3 sampleCone(RNG &rng, T theta) const
    {
        static std::uniform_real_distribution uniform01(0., 1.);
        if (theta < EPSILON)
            return *this;
        theta = theta * (1 - 2 * std::acos(uniform01(rng)) / M_PI);
        auto sintheta = std::sin(theta);
        auto phi = uniform01(rng) * 2 * M_PI;
        Vec3 u = std::abs(x) > 0.1 ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        u = (u ^ *this).norm();
        auto v = *this ^ u;
        auto res = u * (std::cos(phi) * sintheta);
        res = v * (std::sin(phi) * sintheta) + res;
        res = *this * std::cos(theta) + res;
        return res;
    }

    template <typename RNG>
    Vec3 sampleHemis(RNG &rng) const
    {
        static std::uniform_real_distribution uniform01(0., 1.);
        auto costheta2 = uniform01(rng);
        auto costheta = std::sqrt(costheta2);
        auto sintheta = std::sqrt(1 - costheta2);
        auto phi = uniform01(rng) * 2 * M_PI;
        Vec3 u = std::abs(x) > 0.1 ? Vec3{0, 1, 0} : Vec3{1, 0, 0};
        u = (u ^ *this).norm();
        auto v = *this ^ u;
        auto res = u * (std::cos(phi) * sintheta);
        res = v * (std::sin(phi) * sintheta) + res;
        res = *this * costheta + res;
        return res;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Vec3<T> &v)
{
    return os << "{" << v.x << ", " << v.y << ", " << v.z << "}";
}
