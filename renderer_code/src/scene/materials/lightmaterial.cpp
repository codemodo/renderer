#include <scene/materials/lightmaterial.h>

glm::vec3 LightMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags) const

{
    //std::cout << "here";
    if(glm::dot(wiW, isx.normal) > 0.0f) {
        //std::cout << "here";
        return (this->base_color * isx.texture_color * this->intensity);
    }
    else return glm::vec3(0.0f);
}

//glm::vec3 LightMaterial::SamplePhotonDirection(float rand1, float rand2) const
//{
//    //TODO
//    glm::vec3 tmp = la::UnifSampleDisc(rand1, rand2);
////    tmp.z = sqrtf(std::max(0.f, 1.f - tmp.x*tmp.x - tmp.y*tmp.y));

////   // may need to address z direction
////    glm::vec3 bit = isx.bitangent;
////    glm::vec3 tan = isx.tangent;
////    glm::vec3 nor = isx.normal;
////    tmp = glm::vec3(tan.x * tmp.x + bit.x * tmp.y + nor.x * tmp.z,
////                         tan.y * tmp.x + bit.y * tmp.y + nor.y * tmp.z,
////                         tan.z * tmp.x + bit.z * tmp.y + nor.z * tmp.z);
//    return tmp;
//}
