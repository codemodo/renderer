#include <scene/materials/bxdfs/bxdf.h>
#include <iostream>

glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{

    glm::vec3 tmp = la::UnifSampleDisc(rand1, rand2);
    tmp.z = sqrtf(std::max(0.f, 1.f - tmp.x*tmp.x - tmp.y*tmp.y));

    if (wo.z < 0.f) tmp.z *= -1.f;

    wi_ret = tmp;
    pdf_ret = PDF(wo, wi_ret);
    return EvaluateScatteredEnergy(wo, wi_ret);
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    return glm::vec3(0);
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    if (wo.z * wi.z > 0.f) {
        return fabsf(wi.z) * INV_PI;
    }
    return 0.0f;
}
