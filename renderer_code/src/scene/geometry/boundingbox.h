#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <la.h>
#include <scene/materials/material.h>
#include <raytracing/intersection.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/transform.h>
#include <math.h>

class BoundingBox : public Drawable
{
public:
    BoundingBox();
    BoundingBox(const glm::vec3 &p1, const glm::vec3 &p2);
    ~BoundingBox();
    glm::vec3 pMin, pMax;
    BoundingBox Union(const BoundingBox &b1, const BoundingBox &b2);
    glm::vec3 Centroid() const;
    void create();
    virtual GLenum drawMode();




};

#endif // BOUNDINGBOX_H
