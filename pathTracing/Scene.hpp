
#pragma once

#include <vector>
#include "Vector.hpp"
#include "Object.hpp"
#include "Light.hpp"
#include "AreaLight.hpp"
#include "BVH.hpp"
#include "Ray.hpp"


class Scene
{
public:
    // setting up options
    int width = 1280;
    int height = 960;
    double fov = 40;
    Vector3f backgroundColor = Vector3f(0.235294, 0.67451, 0.843137);
    int maxDepth = 1;
    float RussianRoulette = 0.9;

    Scene(int w, int h) : width(w), height(h)
    {}

    void Add(Object *object) { objects.push_back(object); }
    void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

    const std::vector<Object*>& get_objects() const { return objects; }
    const std::vector<std::unique_ptr<Light> >&  get_lights() const { return lights; }
    // 该函数调用场景bvh类中的求交函数
    Intersection intersect(const Ray& ray) const;
    // 场景中的 bvh， 用来划分 obj
    BVHAccel *bvh;
    void buildBVH();
    Vector3f castRay(const Ray &ray, int depth) const;
    void sampleLight(Intersection &pos, float &pdf) const;

    // creating the scene (adding objects and lights)
    std::vector<Object* > objects;               //模型指针集合
    std::vector<std::unique_ptr<Light> > lights; //光源指针集合


//     // Compute Fresnel equation
// //
// // \param I is the incident view direction
// //
// // \param N is the normal at the intersection point
// //
// // \param ior is the material refractive index
// //
// // \param[out] kr is the amount of light reflected
//     void fresnel(const Vector3f &I, const Vector3f &N, const float &ior, float &kr) const
//     {
//         float cosi = clamp(-1, 1, dotProduct(I, N));
//         float etai = 1, etat = ior;
//         if (cosi > 0) {  std::swap(etai, etat); }
//         // Compute sini using Snell's law
//         float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
//         // Total internal reflection
//         if (sint >= 1) {
//             kr = 1;
//         }
//         else {
//             float cost = sqrtf(std::max(0.f, 1 - sint * sint));
//             cosi = fabsf(cosi);
//             float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
//             float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
//             kr = (Rs * Rs + Rp * Rp) / 2;
//         }
//         // As a consequence of the conservation of energy, transmittance is given by:
//         // kt = 1 - kr;
//     }
};