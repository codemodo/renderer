#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <iostream>
glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    float woCosTheta, wiCosTheta;
    woCosTheta = fabsf(wo.z);  wiCosTheta = fabsf(wi.z);
    glm::vec3 wh = glm::normalize(wo + wi);

    if (woCosTheta == 0.f || wiCosTheta == 0.f || glm::length(wh) == 0.f) return glm::vec3(0.f);
    float whCosTheta = fabsf(wh.z);


    return this->reflection_color *
                (exponent + 2.f) * (float)(1.f/(2.f*M_PI)) * powf(whCosTheta, exponent) *
                std::min(1.f,
                    std::min( (2.f * wiCosTheta * whCosTheta / whCosTheta),(2.f * woCosTheta * whCosTheta / whCosTheta) )
                ) /
                (4.f * wiCosTheta * woCosTheta);



    //return glm::vec3(this->reflection_color * (this->exponent + 2.f) * (float)(1.f/(2.f * M_PI)));
    //return glm::vec3(this->reflection_color * (this->exponent + 2.f) * (float)(1.f/(2.f*M_PI)) * powf(fabsf(glm::dot(wh,glm::vec3(0,0,1))), this->exponent) );

// Black when I use current approach, colorful when I use other unindented approach
}

glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{
    float costheta = powf(rand1, 1.f / (exponent+1));
    float sintheta = sqrtf(std::max(0.f, 1.f - costheta*costheta));
    float phi = rand2 * 2.f * M_PI;
    glm::vec3 wh(sintheta * cosf(phi), sintheta * sinf(phi), costheta);
    if (wo.z * wh.z < 0.f) wh = -wh;

    wi_ret = -wo + 2.f * glm::dot(wo, wh) * wh;


    pdf_ret = PDF(wo, wi_ret);

    return EvaluateScatteredEnergy(wo, wi_ret);

}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 wh = glm::normalize(wo + wi);
    float costheta = fabsf(wh.z);
    float pdf = ((exponent + 1.f) * powf(costheta, exponent)) /
                      (2.f * M_PI * 4.f * glm::dot(wo, wh));
    if (glm::dot(wo, wh) <= 0.f) pdf = 0.f;

    return pdf;
}
