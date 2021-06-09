#include "lodepng.hpp"
#include "sceneBuilder.hpp"

int main()
{
    //instantiate the homemade cornell box
    auto scene = sceneBuilder::cornelBoxSpheres(500, 500);

    //how many rays per pixel, seed
    auto image = scene.render(1000, 42);

    //create png file
    lodepng::encode("render/test.png", image, scene.cam.width, scene.cam.height);
}
