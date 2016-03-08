#include "bvhtree.h"

BvhTree::BvhTree(const QList<Geometry*> &gVec)
{
    QList<BvhNode*> leaves;
    for (int i = 0; i < gVec.size(); i++) {
        BoundingBox* bb;
        bb = new BoundingBox();
        *bb = gVec[i]->GetBoundingBox();
        bb->create();
        BvhNode node(true);
        node.boundingBox = bb;
        node.geometry = gVec[i];
        leaves.push_back(&node);
    }

    BoundingBox* bb;
    bb = new BoundingBox();

    for (int i=0; i < leaves.size(); i++) {
        *bb = bb->Union(*bb,*leaves[i]->boundingBox);
    }
    bb->create();

    root = new BvhNode(false);
    root->boundingBox = bb;
    //root = &r;

    if (leaves.size()>0)
        BuildTree(leaves, root, 0);

}

void BvhTree::BuildTree(const QList<BvhNode*> leaves, BvhNode* parent, int axis)
{
    if (leaves.size() == 0) return;

    if (axis == 3) axis = 0;

    QList<BvhNode*> parentLeaves = leaves;
    QList<BvhNode*> leftLeaves, rightLeaves;

    for (int i = 0; i < leaves.size(); i++) {
        if (leaves[i]->boundingBox->Centroid()[axis] <= parent->boundingBox->Centroid()[axis]) {
            leftLeaves.append(leaves[i]);
        }
        else {
            rightLeaves.append(leaves[i]);
        }
    }

    if (leftLeaves.size() > 0) {
        BoundingBox *leftBB;
        leftBB = new BoundingBox();

        for (int i=0; i < leftLeaves.size(); i++) {
            *leftBB = leftBB->Union(*leftBB,*leftLeaves[i]->boundingBox);
        }
        leftBB->create();

        BvhNode* leftNode;
        leftNode = new BvhNode(false);
        leftNode->boundingBox = leftBB;
        parent->leftChild = leftNode;
        BuildTree(leftLeaves, leftNode, axis++);
    }

    if (rightLeaves.size() > 0) {
        BoundingBox *rightBB;
        rightBB = new BoundingBox();

        for (int i=0; i < rightLeaves.size(); i++) {
            *rightBB = rightBB->Union(*rightBB,*rightLeaves[i]->boundingBox);
        }
        rightBB->create();

        BvhNode* rightNode;
        rightNode = new BvhNode(false);
        rightNode->boundingBox = rightBB;
        parent->rightChild = rightNode;
        BuildTree(rightLeaves, rightNode, axis++);
    }
    return;
}

