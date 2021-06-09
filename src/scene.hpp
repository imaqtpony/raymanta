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

struct Scene
{
    using Point = Vec3<real_t>;
    using Color = Vec3<real_t>;

    struct IntersectionInfo
    {
        Point pos;
        Vec3<real_t> normal;
        Material mat;
        bool inside;
    };

    Camera cam;
    std::vector<Sphere> spheres;
    std::vector<Material> spheresMat;
    std::vector<Triangle> triangles;
    std::vector<Material> trianglesMat;

    Scene(const Camera &p_cam,
          const std::vector<Sphere> &p_spheres,
          const std::vector<Material> &p_spheresMat,
          const std::vector<Triangle> &p_tris,
          const std::vector<Material> &p_trianglesMat);

    std::pair<bool, IntersectionInfo> intersect(const Ray &ray) const;

    Color radiance(rng_t &rng, const Ray &ray, int depth, int MAX_DEPTH) const;

    std::vector<unsigned char> render(int samplesPerPixel, int seed, int maxDepth = 10) const;
};