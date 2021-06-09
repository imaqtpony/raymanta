#include "scene.hpp"

Scene::Scene(const Camera &p_cam,
             const std::vector<Sphere> &p_spheres,
             const std::vector<Material> &p_spheresMat,
             const std::vector<Triangle> &p_tris,
             const std::vector<Material> &p_trianglesMat)
    : cam(p_cam),
      spheres(p_spheres),
      spheresMat(p_spheresMat),
      triangles(p_tris),
      trianglesMat(p_trianglesMat)
{
}

std::pair<bool, Scene::IntersectionInfo> Scene::intersect(const Ray &ray) const
{
    real_t minDist = std::numeric_limits<real_t>::infinity();
    size_t minId;
    for (size_t i = 0; i < spheres.size(); i++)
    {
        auto inter = spheres[i].intersect(ray);
        if (inter != -1 && inter < minDist)
        {
            minDist = inter;
            minId = i;
        }
    }

    bool isTri = false;
    for (size_t i = 0; i < triangles.size(); i++)
    {
        auto inter = triangles[i].intersect(ray);
        if (inter != -1 && inter < minDist)
        {
            minDist = inter;
            minId = i;
            isTri = true;
        }
    }

    if (minDist == std::numeric_limits<real_t>::infinity())
    {
        IntersectionInfo res;
        return {false, res};
    }

    Vec3<real_t> interPos = ray.at(minDist);
    Vec3<real_t> normal = isTri ? triangles[minId].n : spheres[minId].normal(interPos);
    auto inside = ray.dir.dot(normal) > 0;

    if (inside)
        normal = -normal;

    return {true, IntersectionInfo{interPos, normal, isTri ? trianglesMat[minId] : spheresMat[minId], inside}};
}

Scene::Color Scene::radiance(rng_t &rng, const Ray &ray, int depth, int MAX_DEPTH) const
{
    static std::uniform_real_distribution uniform01(0., 1.);

    if (depth > MAX_DEPTH)
        return Color{0, 0, 0};

    auto [intersection, interInfo] = intersect(ray);
    if (!intersection)
        return Color{0, 0, 0};

    Material mat = interInfo.mat;

    if (depth > RUSSIAN_ROULETTE_MIN_DEPTH)
    {
        auto p = mat.diffuse.max();
        if (uniform01(rng) < p)
            mat.diffuse = mat.diffuse / p;
        else
            return mat.emission;
    }

    Vec3<real_t> normal = interInfo.normal;
    Ray reflectionRay;
    Color rad;

    switch (mat.type)
    {
    case Material::MatType::DIFFUSE:
        reflectionRay = Ray{interInfo.pos, normal.sampleHemis(rng)};
        rad = radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
        break;
    case Material::MatType::SPECULAR:
        reflectionRay = Ray{interInfo.pos, normal.sampleCone(rng, mat.refractionAngle)};
        rad = radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
        break;
    case Material::MatType::REFRACT:
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
                Vec3<real_t> tDir = ray.dir * nint;
                tDir = normal * (nint * cosi - cost) + tDir;
                Ray tranRay = Ray{interInfo.pos, tDir};
                rad = radiance(rng, tranRay, depth + 1, MAX_DEPTH);
            }
            else
            {
                reflectionRay = Ray{interInfo.pos, ray.dir.reflect(normal)};
                rad = radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
            }
        }
        else
        {
            reflectionRay = Ray{interInfo.pos, ray.dir.reflect(normal)};
            rad = radiance(rng, reflectionRay, depth + 1, MAX_DEPTH);
        }
    }

    return mat.diffuse * rad + mat.emission;
}

std::vector<unsigned char> Scene::render(int samplesPerPixel, int seed, int maxDepth) const
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
            rng_t rng(y * y + seed);
            for (int x = 0; x < cam.width; ++x)
            {
                Color pixelColor = {0., 0., 0.};
                for (int i = 0; i < samplesPerPixel; ++i)
                {
                    auto ray = cam.getRay(rng, x, y);
                    auto rad = radiance(rng, ray, 0, maxDepth);
                    pixelColor = pixelColor + rad;
                }

                pixelColor = pixelColor / (real_t)samplesPerPixel;
                auto pixelPos = (y * cam.width + x) * 4;
                auto pixel = Material::colorToPixel(pixelColor);
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

    std::cerr << std::endl;

    /* Solo threading */

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
    // }

    return res;
}