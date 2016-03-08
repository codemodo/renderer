#include <scene/materials/bxdfs/lambertBxDF.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

glm::vec3 LambertBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return this->diffuse_color * (float)(1.f / M_PI);
}

glm::vec3 LambertBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}
