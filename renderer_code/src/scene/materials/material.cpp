
#include <scene/materials/material.h>
#include <QColor>
#include <math.h>
#include <iostream>
#include <time.h>
#include <ctime>

Material::Material() :
    Material(glm::vec3(0.5f, 0.5f, 0.5f))
{
}

Material::Material(const glm::vec3 &color):
    name("MATERIAL"),
    bxdfs(),
    is_light_source(false),
    base_color(color),
    intensity(0),
    is_weighted(false),
    bxdf_weights()
{
    texture = NULL;
    normal_map = NULL;

}

glm::vec3 Material::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags) const
{
    glm::vec3 woL, wiL;
    glm::vec3 bit = isx.bitangent;
    glm::vec3 tan = isx.tangent;
    glm::vec3 nor = isx.normal;
    woL = glm::vec3(glm::dot(woW, tan), glm::dot(woW, bit), glm::dot(woW, nor));
    wiL = glm::vec3(glm::dot(wiW, tan), glm::dot(wiW, bit), glm::dot(wiW, nor));

    glm::vec3 energy(0.f);
    for (int i = 0; i < bxdfs.size(); i++) {
        if ((bxdfs[i]->type & flags) == bxdfs[i]->type) {
            energy += bxdfs[i]->EvaluateScatteredEnergy(woW, wiW);
        }
    }

    return base_color * isx.texture_color * energy;
}

float Material::PDF(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags) const
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

glm::vec3 Material::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, const float &rand1, const float &rand2, BxDFType &type, BxDFType flags) const
{
    // Non-weighted, so pick a random bxdf to sample
    int index = rand() % bxdfs.length();
    type = bxdfs[index]->type;

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

glm::vec3 Material::EvaluateHemisphereScatteredEnergy(const Intersection &isx, const glm::vec3 &wo, int num_samples, BxDFType flags) const
{
    //TODO
    return glm::vec3(0);
}

float Material::GetPDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    int index = rand() % bxdfs.length();
    return bxdfs[index]->PDF(wo, wi);
}








glm::vec3 Material::GetImageColor(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL || uv_coord.x < 0 || uv_coord.y < 0 || uv_coord.x >= 1.0f || uv_coord.y >= 1.0f)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue())/255.0f;
    }
}

glm::vec3 Material::GetImageColorInterp(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL || uv_coord.x < 0 || uv_coord.y < 0 || uv_coord.x >= 1.0f || uv_coord.y >= 1.0f)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        //Use bilinear interp.

        float X = image->width() * uv_coord.x;
        float Y = image->height() * (1.0f - uv_coord.y);

        glm::vec2 floors = glm::vec2(floor(X), floor(Y));
        glm::vec2 ceils = glm::vec2(ceil(X), ceil(Y));
        ceils = glm::min(ceils, glm::vec2(image->width()-1, image->height()-1));
        QColor qll = image->pixel(floors.x, floors.y); glm::vec3 ll(qll.red(), qll.green(), qll.blue());
        QColor qlr = image->pixel(ceils.x, floors.y); glm::vec3 lr(qlr.red(), qlr.green(), qlr.blue());
        QColor qul = image->pixel(floors.x, ceils.y); glm::vec3 ul(qul.red(), qul.green(), qul.blue());
        QColor qur = image->pixel(ceils.x, ceils.y); glm::vec3 ur(qur.red(), qur.green(), qur.blue());

        float distX = (X - floors.x);
        glm::vec3 color_low = ll * (1-distX) + lr * distX;
        glm::vec3 color_high = ul * (1-distX) + ur * distX;

        float distY = (Y - floors.y);

        glm::vec3 result = (color_low * (1 - distY) + color_high * distY)/255.0f;

        return result;


        glm::vec2 XY(X,Y);
        //Want floor and ceil of both X and Y
        //Do square interp of <X,Y>

        float dist_ll = glm::distance(XY, floors);
        float dist_lr = glm::distance(XY, glm::vec2(ceils.x, floors.y));
        float dist_ul = glm::distance(XY, glm::vec2(floors.x, ceils.y));
        float dist_ur = glm::distance(XY, ceils);
        float sum = dist_ll + dist_lr + dist_ul + dist_ur;

        float llc = (1 - dist_ll/sum);
        float lrc = (1 - dist_lr/sum);
        float ulc = (1 - dist_ul/sum);
        float urc = (1 - dist_ur/sum);

        float checksum = llc + lrc + ulc + urc;

        glm::vec3 final_color = llc * ll + lrc * lr + ulc * ul + urc * ur;
        return final_color/255.0f;
    }
}
