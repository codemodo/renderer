#include <scene/geometry/geometry.h>

float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //TODO
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(isx.object_hit == NULL)
    {
        return 0;
    }

    ComputeArea();
    float pdf = std::max(0.f,
                         (glm::distance2(ray.origin, isx.point) /
                        (fabsf(glm::dot(isx.normal, -ray.direction)) * area)));
    return pdf;
}

glm::vec3 Geometry::SamplePhotonDirection(Intersection &isx, float rand1, float rand2) const
{
    glm::vec3 tmp = la::UnifSampleDisc(rand1, rand2);
    tmp.z = sqrtf(std::max(0.f, 1.f - tmp.x*tmp.x - tmp.y*tmp.y));

   // may need to address z direction
    glm::vec3 bit = isx.bitangent;
    glm::vec3 tan = isx.tangent;
    glm::vec3 nor = isx.normal;
    tmp = glm::vec3(tan.x * tmp.x + bit.x * tmp.y + nor.x * tmp.z,
                         tan.y * tmp.x + bit.y * tmp.y + nor.y * tmp.z,
                         tan.z * tmp.x + bit.z * tmp.y + nor.z * tmp.z);
    return tmp;
}

