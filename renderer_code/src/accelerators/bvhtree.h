#pragma once
#include <la.h>
#include <scene/geometry/boundingbox.h>
#include <scene/geometry/geometry.h>
#include <accelerators/bvhnode.h>
class BvhNode;
class Geometry;

class BvhTree
{
public:
    BvhTree(const QList<Geometry*> &gVec);
    void BuildTree(const QList<BvhNode*> leaves, BvhNode* parent, int axis);
    BvhNode* root;
};
