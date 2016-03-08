#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <raytracing/samplers/pixelsampler.h>
#include <scene/geometry/geometry.h>
#include <scene/materials/bxdfs/bxdf.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

class Geometry;
class Material;

class Scene
{
public:
    Scene();
    QList<Geometry*> objects;
    QList<Material*> materials;
    QList<BxDF*> bxdfs;
    QList<Geometry*> lights;
    Camera camera;
    Film film;

    unsigned int sqrt_samples;//Read by MyGL and RenderThread when making PixelSamplers

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();
};
