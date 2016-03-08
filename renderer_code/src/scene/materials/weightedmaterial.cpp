#include <scene/materials/weightedmaterial.h>

WeightedMaterial::WeightedMaterial() : Material(){}
WeightedMaterial::WeightedMaterial(const glm::vec3 &color) : Material(color){}

glm::vec3 WeightedMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, float &pdf_ret, BxDFType flags) const
{

    // Weighted - find the bxdf that matches the one passed in
    int index = 0;
    for (int i = 0; i < bxdfs.size(); i++) {
        if (bxdfs[i]->type == flags) {
            index = i;
            break;
        }
    }

    glm::vec3 woL, wiL;
    glm::vec3 bit = isx.bitangent;
    glm::vec3 tan = isx.tangent;
    glm::vec3 nor = isx.normal;
    woL = glm::vec3(glm::dot(woW, tan), glm::dot(woW, bit), glm::dot(woW, nor));
    wiL = glm::vec3(glm::dot(wiW, tan), glm::dot(wiW, bit), glm::dot(wiW, nor));

    glm::vec3 energy = bxdfs[index]->EvaluateScatteredEnergy(woW, wiW);

    return base_color * isx.texture_color * energy;
}

glm::vec3 WeightedMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, const float &rand1, const float &rand2, BxDFType &type, BxDFType flags) const
{
    // Weighted - find the bxdf that matches the one passed in
    int index = 0;
    for (int i = 0; i < bxdfs.size(); i++) {
        if (bxdfs[i]->type == flags) {
            index = i;
            break;
        }
    }

    // Convert to local tangent space
    glm::vec3 bit = isx.bitangent;
    glm::vec3 tan = isx.tangent;
    glm::vec3 nor = isx.normal;
    glm::vec3 woL = glm::vec3(glm::dot(woW, tan), glm::dot(woW, bit), glm::dot(woW, nor));

    // Set up variables for return values
    glm::vec3 wiL;
    float pdf;

    // Sample
    glm::vec3 color = bxdfs[index]->SampleAndEvaluateScatteredEnergy(woL, wiL, rand1, rand2, pdf);

    // Store our sample values in the return variables
    pdf_ret = pdf;
    wiW_ret = glm::vec3(tan.x * wiL.x + bit.x * wiL.y + nor.x * wiL.z,
                         tan.y * wiL.x + bit.y * wiL.y + nor.y * wiL.z,
                         tan.z * wiL.x + bit.z * wiL.y + nor.z * wiL.z);


    return base_color * isx.texture_color * color;
}

float WeightedMaterial::PDF(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags) const
{
    glm::vec3 woL, wiL;
    glm::vec3 bit = isx.bitangent;
    glm::vec3 tan = isx.tangent;
    glm::vec3 nor = isx.normal;
    woL = glm::vec3(glm::dot(woW, tan), glm::dot(woW, bit), glm::dot(woW, nor));
    wiL = glm::vec3(glm::dot(wiW, tan), glm::dot(wiW, bit), glm::dot(wiW, nor));

    float pdfTotal;
    int matches = 0;
    for (int i = 0; i < bxdfs.size(); i++) {
        if ((bxdfs[i]->type & flags) == bxdfs[i]->type) {
            pdfTotal += bxdfs[i]->PDF(woL, wiL);
            matches++;
        }
    }
    if (matches > 0) return pdfTotal / (float)matches;
    return 0.f;
}
