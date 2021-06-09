#include "sceneBuilder.hpp"

namespace sceneBuilder
{
    //convert 2 tris to quad
    std::vector<Triangle> quadToTri(const std::vector<Vec3<real_t>> &points)
    {
        std::vector<Triangle> res;
        res.push_back(Triangle{points[0], points[1], points[2]});
        res.push_back(Triangle{points[0], points[2], points[3]});
        return res;
    }

    //the current scene to render
    Scene cornelBoxSpheres(int width, int height)
    {
        using P = Vec3<real_t>;
        using M = Material;
        using S = Sphere;

        M black{P{0., 0., 0.}, P{0., 0., 0.}, M::MatType::DIFFUSE};
        M white{P{0., 0., 0.}, P{0.75, 0.75, 0.75}, M::MatType::DIFFUSE};
        // M green{P{0., 0., 0.}, P{0.25, 0.75, 0.25}, M::MatType::DIFFUSE};
        // M red{P{0., 0., 0.}, P{0.75, 0.25, 0.25}, M::MatType::DIFFUSE};
        M orange{P{0., 0., 0.}, P{253. / 255., 203. / 255., 110. / 255.}, M::MatType::DIFFUSE};
        M electronblue{P{0., 0., 0.}, P{9. / 255., 132. / 255., 227. / 255.}, M::MatType::DIFFUSE};
        M light{P{12., 12., 12.}, P{0., 0., 0.}, M::MatType::DIFFUSE};
        M glass{P{0., 0., 0.}, P{.999, .999, .999}, M::MatType::REFRACT};
        M perfectmirror{P{0., 0., 0.}, P{.999, .999, .999}, M::MatType::SPECULAR};
        M mirror{P{0., 0., 0.}, P{.999, .999, .999}, M::MatType::SPECULAR, PI / 6.};
        M smallSpecBlue{P{0., 0., 0.}, P{56. / 255., 103. / 255., 214. / 255.}, M::MatType::SPECULAR, PI / 3.};

        auto floor = quadToTri(std::vector<P>({P{552.8, 0.0, 0.0}, P{0.0, 0.0, 0.0}, P{0.0, 0.0, 559.2}, P{549.6, 0.0, 559.2}}));
        std::vector<M> floorMat(2, white);

        auto ceiling = quadToTri(std::vector<P>({P{556.0, 548.8, 0.0}, P{556.0, 548.8, 559.2},
                                                 P{0.0, 548.8, 559.2}, P{0.0, 548.8, 0.0}}));
        std::vector<M> ceilingMat(2, white);

        auto back = quadToTri(std::vector<P>({P{549.6, 0.0, 559.2}, P{0.0, 0.0, 559.2},
                                              P{0.0, 548.8, 559.2}, P{556.0, 548.8, 559.2}}));
        std::vector<M> backMat(2, white);

        auto front = quadToTri(std::vector<P>({P{552.8 + 10000, 0.0, 0.0 - 10000.}, P{-10000, 0.0, -10000.}, P{-10000.0, 548.8, 0.0}, P{556.0 + 10000, 548.8, 0.0}}));
        std::vector<M> frontMat(2, black);

        auto right = quadToTri(std::vector<P>({P{0.0, 0.0, 559.2}, P{0.0, 0.0, 0.0}, P{0.0, 548.8, 0.0}, P{0.0, 548.8, 559.2}}));
        std::vector<M> rightMat(2, electronblue);

        auto left = quadToTri(std::vector<P>({P{552.8, 0.0, 0.0}, P{549.6, 0.0, 559.2},
                                              P{556.0, 548.8, 559.2}, P{556.0, 548.8, 0.0}}));
        std::vector<M> leftMat(2, orange);

        std::vector<S> spheres = {S{P{280., 548.8 + 1000, 280.}, 1000 + 5},
                                  S{P{120., 100., 250.}, 100.},
                                  S{P{120., 100., 250.}, 30},
                                  S{P{430., 80., 300.}, 80},
                                  S{P{275., 220., 350.}, 80}};

        std::vector<M> spheresMat = {light, glass, mirror, perfectmirror, smallSpecBlue};

        Camera cam(P{278., 273., -800.}, P{0., 0., 1.}, P{0., 1., 0.}, width, height, 35);

        std::vector<Triangle> tris;
        tris.insert(tris.end(), floor.begin(), floor.end());
        tris.insert(tris.end(), ceiling.begin(), ceiling.end());
        tris.insert(tris.end(), back.begin(), back.end());
        tris.insert(tris.end(), front.begin(), front.end());
        tris.insert(tris.end(), right.begin(), right.end());
        tris.insert(tris.end(), left.begin(), left.end());

        std::vector<M> trisMat;
        trisMat.insert(trisMat.end(), floorMat.begin(), floorMat.end());
        trisMat.insert(trisMat.end(), ceilingMat.begin(), ceilingMat.end());
        trisMat.insert(trisMat.end(), backMat.begin(), backMat.end());
        trisMat.insert(trisMat.end(), frontMat.begin(), frontMat.end());
        trisMat.insert(trisMat.end(), rightMat.begin(), rightMat.end());
        trisMat.insert(trisMat.end(), leftMat.begin(), leftMat.end());

        return Scene(cam, spheres, spheresMat, tris, trisMat);
    }
}
