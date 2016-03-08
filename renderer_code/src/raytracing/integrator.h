#pragma once
#include <la.h>
#include <raytracing/ray.h>
#include <raytracing/intersection.h>
#include <raytracing/intersectionengine.h>
#include <scene/scene.h>
#include <scene/materials/bxdfs/bxdf.h>

class Scene;

//The Integrator class recursively evaluates the path a ray takes throughout a scene
//and computes the color a ray becomes as it bounces.
//It samples the materials, probability density functions, and BRDFs of the surfaces the ray hits
//to do this.

class Integrator
{
public:
    Integrator();
    Integrator(Scene *s);
    glm::vec3 TraceRay(Ray r, unsigned int depth);
    glm::vec3 TraceRayTransport(Ray r, unsigned int depth);
    void SetDepth(unsigned int depth);
    BxDFType GetWeightedType(const Intersection &isx, float rand);
    glm::vec3 DirectLighting(Ray r, const Intersection &objectIsc, int samples);
    glm::vec3 EvaluateDirectLighting(const glm::vec3 &brdf, const glm::vec3 &lightEnergy, float mainPdf, float otherPdf, float absdot, int samples);

    Scene* scene;
    IntersectionEngine* intersection_engine;



// PHOTON MAPPER

    void PhotonMapper(Ray r, unsigned int depth);

    //TODO incorporate weight factor
    struct photon_t {
        photon_t(const Intersection &objectIsx, const glm::vec3 &dir) : isx(objectIsx), wi(dir){}
        Intersection isx;
        glm::vec3 wi;
    };

protected:
    unsigned int max_depth;//Default value is 5.
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;
};
