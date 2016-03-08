#include <scene/materials/bxdfs/specularreflectionBxDF.h>
#include <iostream>

//glm::vec3 SpecularReflectionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
//{
//    float theta1, theta2;
//    theta1 = glm::dot(wo,glm::vec3(0.f,0.f,1.f));
//    theta2 = glm::dot(wi,glm::vec3(0.f,0.f,1.f));
//    //if(fabsf(theta1-theta2) < 0.001) {
//        //std::cout << "gets here";
//        //std::cout << glm::to_string(this->reflection_color);
//    glm::vec3 perfectReflection(-wo.x, -wo.y, wo.z);
//    float actual, expected;
//    actual = glm::dot(glm::vec3(1.f), wi);
//    expected = glm::dot(glm::vec3(1.f), perfectReflection);
//    if (fabsf(actual-expected) > 0.00001) {
//        //std::cout << "\nshould get here";
//        return glm::vec3(0.f);
//    }

//    return this->reflection_color;
//    //}
//    //return glm::vec3(0);
//}

glm::vec3 SpecularReflectionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return glm::vec3(0.f);
}

glm::vec3 SpecularReflectionBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const
{

    wi_ret = glm::vec3(-wo.x, -wo.y, wo.z);

    pdf_ret = 1.f;

    // physically incorrect but an acceptable simplification (pg 439)
    return reflection_color;
}

float SpecularReflectionBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return 0.f;

}

