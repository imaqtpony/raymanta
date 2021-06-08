#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <execution>
#include <mutex>

#include "vec3.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "constants.hpp"

template <typename T, typename RNG>
struct Scene
{
    using Point = vec3<T>;
    using Color = vec3<T>;

    struct IntersectionInfo
    {
        Point pos;
        vec3<T> normal;
        material<T> mat;
        bool inside;
    };

    Camera<T> cam;
    std::vector<sphere<T>> spheres;
    std::vector<material<T>> spheresMat;
    std::vector<triangle<T>> triangles;
    std::vector<material<T>> trianglesMat;

    Scene(const Camera<T> &p_cam,
          const std::vector<sphere<T>> &p_spheres,
          const std::vector<material<T>> &p_spheresMat,
          const std::vector<triangle<T>> &p_tris,
          const std::vector<material<T>> &p_trianglesMat)
        : cam(p_cam),
          spheres(p_spheres),
          spheresMat(p_spheresMat),
          triangles(p_tris),
          trianglesMat(p_trianglesMat)
    {
    }

    std::pair<bool, IntersectionInfo> Intersect(const Ray<T> &ray) const
    {
        T minDist = std::numeric_limits<T>::infinity();
        size_t minId;
        for (size_t i = 0; i < spheres.size(); i++)
        {
            auto inter = spheres[i].Intersect(ray);
            if (inter != -1 && inter < minDist)
            {
                minDist = inter;
                minId = i;
            }
        }

        bool isTri = false;
        for (size_t i = 0; i < triangles.size(); i++)
        {
            auto inter = triangles[i].Intersect(ray);
            if (inter != -1 && inter < minDist)
            {
                minDist = inter;
                minId = i;
                isTri = true;
            }
        }

        if (minDist == std::numeric_limits<T>::infinity())
        {
            IntersectionInfo res;
            return {false, res};
        }

        vec3<T> interPos = ray.at(minDist);
        vec3<T> normal = isTri ? triangles[minId].n : spheres[minId].normal(interPos);
        auto inside = ray.dir.dot(normal) > 0;

        if (inside)
            normal = -normal;

        return {true, IntersectionInfo{interPos, normal, isTri ? trianglesMat[minId] : spheresMat[minId], inside}};
    }

    Color Radiance(RNG &rng, const Ray<T> &ray, int depth, int MAX_DEPTH) const
    {
        static std::uniform_real_distribution uniform01(0., 1.);

        if (depth > MAX_DEPTH)
            return Color{0, 0, 0};

        auto [intersection, interInfo] = Intersect(ray);
        if (!intersection)
            return Color{0, 0, 0};

        material mat = interInfo.mat;

        if (depth > RUSSIAN_ROULETTE_MIN_DEPTH)
        {
            auto p = mat.diffuse.max();
            if (uniform01(rng) < p)
                mat.diffuse = mat.diffuse / p;
            else
                return mat.emission;
        }

        vec3<T> normal = interInfo.normal;
        Ray<T> reflectionRay;
        Color radiance;

        switch (mat.type)
        {
        case material<T>::MatType::DIFFUSE:
            reflectionRay = Ray<T>{interInfo.pos, normal.sampleHemis(rng)};
            radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            break;
        case material<T>::MatType::SPECULAR:
            reflectionRay = Ray<T>{interInfo.pos, normal.sampleCone(rng, mat.refractionAngle)};
            radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            break;
        case material<T>::MatType::REFRACT:
            auto ni = interInfo.inside ? 1.5 : 1.;
            auto nt = interInfo.inside ? 1. : 1.5;

            auto nint = ni / nt;
            auto cosi = -ray.dir.dot(normal);
            auto cos2t = 1 - nint * nint * (1 - cosi * cosi);

            if (cos2t > 0)
            {
                auto cost = sqrt(cos2t);
                auto rPer = pow((ni * cosi - nt * cost) / (ni * cosi + nt * cost), 2);
                auto rPar = pow((nt * cosi - ni * cost) / (nt * cosi + ni * cost), 2);
                auto p = (rPer + rPar) / 2.;

                if (uniform01(rng) < 1 - p)
                {
                    vec3<T> tDir = ray.dir * nint;
                    tDir = normal * (nint * cosi - cost) + tDir;
                    Ray<T> tranRay = Ray<T>{interInfo.pos, tDir};
                    radiance = Radiance(rng, tranRay, depth + 1, MAX_DEPTH);
                }
                else
                {
                    reflectionRay = Ray<T>{interInfo.pos, ray.dir.reflect(normal)};
                    radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
                }
            }
            else
            {
                reflectionRay = Ray<T>{interInfo.pos, ray.dir.reflect(normal)};
                radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            }
        }

        return mat.diffuse * radiance + mat.emission;
    }

    std::vector<unsigned char> Render(int samplesPerPixel, int seed, int maxDepth = 10) const
    {
        std::vector<unsigned char> res(cam.width * cam.height * 4);

        std::vector<int> ys(cam.height);
        std::iota(ys.begin(), ys.end(), 0);

        int lines_done = 0;
        std::mutex mut;

        std::for_each(
            std::execution::par_unseq,
            ys.begin(),
            ys.end(),
            [this, &res, seed, samplesPerPixel, maxDepth, &lines_done, &mut](int y)
            {
                RNG rng(y * y + seed);
                for (int x = 0; x < cam.width; ++x)
                {
                    Color pixelColor = {0., 0., 0.};
                    for (int i = 0; i < samplesPerPixel; ++i)
                    {
                        auto ray = cam.GetRay(rng, x, y);
                        auto rad = Radiance(rng, ray, 0, maxDepth);
                        pixelColor = pixelColor + rad;
                    }

                    pixelColor = pixelColor / (T)samplesPerPixel;
                    auto pixelPos = (y * cam.width + x) * 4;
                    auto pixel = material<T>::colorToPixel(pixelColor);
                    res[pixelPos + 0] = pixel.x;
                    res[pixelPos + 1] = pixel.y;
                    res[pixelPos + 2] = pixel.z;
                    res[pixelPos + 3] = 255;
                }

                {
                    std::unique_lock lock(mut);
                    ++lines_done;
                    constexpr int total_chars = 50;
                    int done_chars = (total_chars * lines_done) / cam.height;
                    int todo_chars = total_chars - done_chars;
                    std::cerr << "\r  [";
                    for (int ii = 0; ii < done_chars; ++ii)
                        std::cerr << "#";
                    for (int ii = 0; ii < todo_chars; ++ii)
                        std::cerr << " ";
                    std::cerr << "] -- " << (int)(100. * lines_done / (double)cam.height) << " %   ";
                }
            });

        std::cerr << "\033[0;32mPOGGERS\033[0;0m" << std::endl;

        // for (int y = 0; y < height; y++)
        // {
        //     for (int x = 0; x < width; ++x)
        //     {
        //         Color pixelColor = {0., 0., 0.};
        //         for (int i = 0; i < samplesPerPixel; ++i)
        //         {
        //             auto ray = cam.GetRay(rng, x, y);
        //             auto rad = Radiance(rng, ray, 0, maxDepth);
        //             pixelColor = pixelColor + rad;
        //         }

        //         pixelColor = pixelColor / (T)samplesPerPixel;
        //         auto pixelPos = y * width + x;
        //         res[pixelPos] = material<T>::colorToPixel(pixelColor);
        //     }

        //     std::cout << y << std::endl;
        //     // rowsDone++;
        //     // auto prop = 100. * rowsDone / height;
        //     // auto nok = (int)round(prop * PROGRESS_CHARS / 100.);
        //     // auto nnok = PROGRESS_CHARS - nok;
        //     //Log
        // }
        return res;
    }
};