#pragma once
#include <raytracing/integrator.h>


class DirectLightingIntegrator : public Integrator
{
public:
    glm::vec3 TraceRay(Ray r, unsigned int depth);
};
