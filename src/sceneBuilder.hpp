#include "vec3.hpp"
#include "sphere.hpp"
#include "scene.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "material.hpp"

#include <array>
#include <cmath>

namespace sceneBuilder
{

    std::vector<Triangle> quadToTri(const std::vector<Vec3<real_t>> &points);

    Scene cornelBoxSpheres(int width, int height);

}
