#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <random>

#include "lodepng.hpp"
#include "sceneBuilder.hpp"

using pixel = unsigned char;

int main()
{
    auto scene = sceneBuilder::cornelBoxSpheres(500, 500);

    auto image = scene.render(1000, 42);
    lodepng::encode("render/test.png", image, scene.cam.width, scene.cam.height);
}
