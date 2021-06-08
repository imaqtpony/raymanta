#include "vec3.hpp"
#include "sphere.hpp"
#include "scene.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "material.hpp"
// #include "stlParser.hpp"

template <typename T>
class SceneBuilder
{
    auto quadToTri(T)(Vec3 !T[4] points)
    {
        Triangle !T[2] res = [
            Triangle !T(points[0], points[1], points[2]),
            Triangle !T(points[0], points[2], points[3])
        ];
        return res;
    }
};
