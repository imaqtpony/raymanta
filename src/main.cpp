#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <random>

#include "lodepng.hpp"
// #include "vec3.hpp"
// #include "triangle.hpp"
// #include "sphere.hpp"
// #include "material.hpp"
// #include "camera.hpp"
// #include "scene.hpp"
#include "sceneBuilder.hpp"

using namespace std;
using pixel = unsigned char;

template <typename T>
std::ostream &operator<<(std::ostream &os, const vector<T> &v)
{
    for (auto &e : v)
        os << e << ", ";
    return os;
}

int main()
{
    /****************************
     * TODO faire main.cpp
     * debugger scene
     * tester scene.hpp, material, camera
     * faire une scene sur sceneBuilder
     * renommer un max les vars
     * commenter
     ************************* */

    auto scene = scene_builder::cornelBoxSpheres<double, minstd_rand>(500, 500);

    auto image = scene.Render(1000, 42);
    lodepng::encode("yes2.png", image, scene.cam.width, scene.cam.height);

    // string filename = "yolo.png";
    // constexpr uint width = 1048;
    // constexpr uint height = 720;
    // vector<pixel> image(width * height * 4);

    // for (uint i = 0; i < height; i++)
    // {
    //     for (uint j = 0; j < width; j++)
    //     {
    //         image[4 * (width * i + j) + 0] = 255;
    //         image[4 * (width * i + j) + 1] = 0;
    //         image[4 * (width * i + j) + 2] = i > height / 2 ? 255 : 38;
    //         image[4 * (width * i + j) + 3] = 255;
    //     }
    // }

    // lodepng::encode(filename, image, width, height);

    // random_device rd;
    // mt19937 rng(rd());

    // using vec = vec3<double>;
    // // vec3<double> a = {1, -1, 0};
    // vec a = {0, 0, 0};
    // vec b = {1, 0, 0};
    // vec c = {1, 1, 0};

    // Camera<double> cam = {a, b, c, 480, 480, 60};

    // cout << cam << endl;

    // triangle<double> t(a, b, c);
    // ray<double> r{vec{0.5, 0.4, 1}, vec{0, 0, -1}};
    // ray<double> mr{vec{0, 0, 1}, vec{0, 0, 1}};

    // auto ti = t.intersect(r);

    // assert(-a == vec3<double>(-1, -2, -3));
    // assert(a * 2 == vec3<double>(1 * 2, 2 * 2, 3 * 2));
    // assert(a / 4. == vec3<double>(1 / 4., 2 / 4., 3 / 4.));

    // assert(a + b == vec3<double>(1 - 2, 2 + 1, 3 + 6));
    // assert(b + a == a + b);
    // assert((a ^ b) == vec3<double>(9, -12, 5));
    // assert((b ^ a) == vec3<double>(-9, 12, -5));

    // assert((a ^ b) + b == vec3<double>(9 - 2, -12 + 1, 5 + 6));

    // assert(a.length2() == 14);
    // assert(a.length() == sqrt(14.));
    // assert(a.dot(b) == 18);
}