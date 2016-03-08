#pragma once
#include <la.h>
#include <scene/geometry/boundingbox.h>
#include <scene/geometry/geometry.h>
#include <accelerators/bvhtree.h>

class BoundingBox;
class BvhTree;
class Geometry;


class BvhNode
{
public:
    BvhNode(){leftChild = NULL; rightChild = NULL; boundingBox = NULL;}
    BvhNode(bool leaf): isLeaf(leaf) {leftChild = NULL; rightChild = NULL; boundingBox = NULL;}
    BoundingBox* boundingBox;
    bool isLeaf;
    Geometry* geometry;
    BvhNode* leftChild;
    BvhNode* rightChild;

    void SetLeft(const BvhNode &l);
    void SetRight(BvhNode r) const;
};

