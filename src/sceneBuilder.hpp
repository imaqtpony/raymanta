#include "vec3.hpp"
#include "sphere.hpp"
#include "scene.hpp"
#include "triangle.hpp"
#include "camera.hpp"
#include "material.hpp"

#include <array>
// #include "stlParser.hpp"

namespace scene_builder
{

    template <typename T>
    std::vector<triangle<T>> quadToTri(const std::vector<vec3<T>> &points)
    {
        using Tri = triangle<T>;
        std::vector<Tri> res;
        res.push_back(Tri{points[0], points[1], points[2]});
        res.push_back(Tri{points[0], points[2], points[3]});
        return res;
    }

    template <typename T, typename RNG>
    Scene<T, RNG> cornelBoxSpheres(int width, int height)
    {
        using P = vec3<T>;
        using M = material<T>;
        using S = sphere<T>;

        M black{P{0., 0., 0.}, P{0., 0., 0.}, M::MatType::DIFFUSE};
        M white{P{0., 0., 0.}, P{0.75, 0.75, 0.75}, M::MatType::DIFFUSE};
        // M green{P{0., 0., 0.}, P{0.25, 0.75, 0.25}, M::MatType::DIFFUSE};
        // M red{P{0., 0., 0.}, P{0.75, 0.25, 0.25}, M::MatType::DIFFUSE};
        M orange{P{0., 0., 0.}, P{253. / 255., 203. / 255., 110. / 255.}, M::MatType::DIFFUSE};
        M electronblue{P{0., 0., 0.}, P{9. / 255., 132. / 255., 227. / 255.}, M::MatType::DIFFUSE};
        M light{P{12., 12., 12.}, P{0., 0., 0.}, M::MatType::DIFFUSE};
        M glass{P{0., 0., 0.}, P{.999, .999, .999}, M::MatType::REFRACT};
        M perfectmirror{P{0., 0., 0.}, P{.999, .999, .999}, M::MatType::SPECULAR};
        M mirror{P{0., 0., 0.}, P{.999, .999, .999}, M::MatType::SPECULAR, M_PI / 6.};
        M smallSpecBlue{P{0., 0., 0.}, P{56. / 255., 103. / 255., 214. / 255.}, M::MatType::SPECULAR, M_PI / 3.};

        auto floor = quadToTri(std::vector<P>({P{552.8, 0.0, 0.0}, P{0.0, 0.0, 0.0}, P{0.0, 0.0, 559.2}, P{549.6, 0.0, 559.2}}));
        std::vector<M> floor_mat(2, white);

        auto ceiling = quadToTri(std::vector<P>({P{556.0, 548.8, 0.0}, P{556.0, 548.8, 559.2},
                                                 P{0.0, 548.8, 559.2}, P{0.0, 548.8, 0.0}}));
        std::vector<M> ceiling_mat(2, white);

        auto back = quadToTri(std::vector<P>({P{549.6, 0.0, 559.2}, P{0.0, 0.0, 559.2},
                                              P{0.0, 548.8, 559.2}, P{556.0, 548.8, 559.2}}));
        std::vector<M> back_mat(2, white);

        auto front = quadToTri(std::vector<P>({P{552.8 + 10000, 0.0, 0.0 - 10000.}, P{-10000, 0.0, -10000.}, P{-10000.0, 548.8, 0.0}, P{556.0 + 10000, 548.8, 0.0}}));
        std::vector<M> front_mat(2, black);

        auto right = quadToTri(std::vector<P>({P{0.0, 0.0, 559.2}, P{0.0, 0.0, 0.0}, P{0.0, 548.8, 0.0}, P{0.0, 548.8, 559.2}}));
        std::vector<M> right_mat(2, electronblue);

        auto left = quadToTri(std::vector<P>({P{552.8, 0.0, 0.0}, P{549.6, 0.0, 559.2},
                                              P{556.0, 548.8, 559.2}, P{556.0, 548.8, 0.0}}));
        std::vector<M> left_mat(2, orange);

        std::vector<S> spheres = {S{P{280., 548.8 + 1000, 280.}, 1000 + 5},
                                  S{P{120., 100., 250.}, 100.},
                                  S{P{120., 100., 250.}, 30},
                                  S{P{430., 80., 300.}, 80},
                                  S{P{275., 120., 430.}, 12}};

        std::vector<M> spheresMat = {light, glass, mirror, perfectmirror, smallSpecBlue};

        Camera<T> cam(P{278., 273., -800.}, P{0., 0., 1.}, P{0., 1., 0.}, width, height, 35);

        std::vector<triangle<T>> tris;
        tris.insert(tris.end(), floor.begin(), floor.end());
        tris.insert(tris.end(), ceiling.begin(), ceiling.end());
        tris.insert(tris.end(), back.begin(), back.end());
        tris.insert(tris.end(), front.begin(), front.end());
        tris.insert(tris.end(), right.begin(), right.end());
        tris.insert(tris.end(), left.begin(), left.end());

        std::vector<M> trisMat;
        trisMat.insert(trisMat.end(), floor_mat.begin(), floor_mat.end());
        trisMat.insert(trisMat.end(), ceiling_mat.begin(), ceiling_mat.end());
        trisMat.insert(trisMat.end(), back_mat.begin(), back_mat.end());
        trisMat.insert(trisMat.end(), front_mat.begin(), front_mat.end());
        trisMat.insert(trisMat.end(), right_mat.begin(), right_mat.end());
        trisMat.insert(trisMat.end(), left_mat.begin(), left_mat.end());

        return Scene<T, RNG>(cam, spheres, spheresMat, tris, trisMat);
    }

}
