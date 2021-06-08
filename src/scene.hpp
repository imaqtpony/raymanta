#include <iostream>
#include <array>

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
    sphere<T> *spheres;
    material<T> *spheresMat;
    triangle<T> *triangles;
    material<T> *trianglesMat;
    // nah
    //vectors ?
    Scene(Camera<T> &p_cam, sphere<T> p_spheres[], material<T> p_spheresMat[], sphere<T> p_tris[], material<T> p_trianglesMat[])
        : cam(p_cam), spheres(&p_spheres), spheresMat(&p_spheresMat), triangles(&p_tris), trianglesMat(&p_trianglesMat)
    {
    }

    IntersectionInfo Intersect(const Ray<T> &ray) const
    {
        T minDist = T.infinity;
        auto minId;
        for (int i = 0; i < spheres.Length; i++)
        {
            auto inter = spheres[i].Intersect(ray);
            if (inter < minDist)
            {
                minDist = inter;
                minId = i;
            }
        }

        bool isTri = false;
        for (int i = 0; i < triangles.Length; i++)
        {
            auto inter = triangles[i].Intersect(ray);
            // !inter.isNull
            if (inter < minDist)
            {
                minDist = inter;
                minId = i;
                isTri = true;
            }
        }

        IntersectionInfo res;
        if (minDist == T.Infinity)
            return res;

        vec3<T> interPos = ray.at(minDist);
        vec3<T> normal = isTri ? triangles[minId].n : spheres[minId].normal(interPos);
        auto inside = ray.dir.dot(normal) > 0;

        if (inside)
            normal = -normal;

        res = IntersectionInfo(interPos, normal, isTri ? trianglesMat[minId] : spheresMat[minId], inside);
        return res;
    }

    Color Radiance(RNG &rng, const ray<T> &ray, int depth, int MAX_DEPTH) const
    {
        static std::uniform_real_distribution uniform01(0., 1.);

        if (depth > MAX_DEPTH)
            return Color(0, 0, 0);

        auto interInfo = Intersect(ray);
        if (!interInfo) //== nullptr)
            return Color(0, 0, 0);

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
        case MatType.DIFFUSE:
            reflectionRay = Ray<T>(interInfo.pos, normal.sampleHemis(rng));
            radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            break;
        case MatType.SPECULAR:
            reflectionRay = Ray<T>(interInfo.pos, normal.sampleCone(rng, mat.refractionAngle));
            radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            break;
        case MatType.REFRACT:
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
                    Ray<T> tranRay = Ray<T>(interInfo.pos, tDir);
                    radiance = Radiance(rng, tranRay, depth + 1, MAX_DEPTH);
                }
                else
                {
                    reflectionRay = Ray<T>(interInfo.pos, ray.dir.reflect(normal));
                    radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
                }
            }
            else
            {
                reflectionRay = Ray<T>(interInfo.pos, ray.dir.reflect(normal));
                radiance = Radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            }
            break;
        default:
            break;
        }

        return mat.diffuse * radiance + mat.emission;
    }

    unsigned char[] Render(int samplesPerPixel, int seed, int maxDepth = 10) const
    {
        int width = cam.width;
        int height = cam.height;
        unsigned char res[] = new unsigned char[width * height * 3];
        int rowsDone = 0;

        for (y here) //? (y; parallel(iota(0, h)))
        {
            // RNG rng = RNG(y*y + seed); //?
            for (int x = 0; x < width; ++x)
            {
                Color pixelColor = {0., 0., 0.};
                for (int i; i < samplesPerPixel; ++i)
                {
                    auto ray = cam.getRay(rng, x, y);
                    auto rad = Radiance(rng, ray, 0, maxDepth);
                    pixelColor = pixelColor + rad;
                }

                pixelColor = pixelColor / (T)samplesPerPixel;
                auto pixelPos = (y * w + x) * 3;
                // res[pixelPos..pixelPos + 3][] = material<T>::colorToPixel(pixelColor)[];
            }

            rowsDone++;
            auto prop = 100. * rowsDone / h;
            auto nok = (int)round(prop * PROGRESS_CHARS / 100.);
            auto nnok = PROGRESS_CHARS - nok;
            //Log
        }
        return res;
    }
};

template <typename T, typename RNG>
std::ostream &operator<<(const std::ostream &os, Scene<T, RNG> &sc)
{
    return os;
};