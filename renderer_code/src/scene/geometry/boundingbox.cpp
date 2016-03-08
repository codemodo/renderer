#include "boundingbox.h"
static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

BoundingBox::~BoundingBox(){}
BoundingBox::BoundingBox()
{
    pMin = glm::vec3(INFIN);
    pMax = glm::vec3(-INFIN);
}

BoundingBox::BoundingBox(const glm::vec3 &p1, const glm::vec3 &p2)
{
    pMin = glm::vec3(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
    pMax = glm::vec3(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
}

BoundingBox BoundingBox::Union(const BoundingBox &b1, const BoundingBox &b2)
{
    BoundingBox bu;
    bu.pMin = glm::vec3(std::min(b1.pMin.x, b2.pMin.x), std::min(b1.pMin.y, b2.pMin.y), std::min(b1.pMin.z, b2.pMin.z));
    bu.pMax = glm::vec3(std::min(b1.pMax.x, b2.pMax.x), std::min(b1.pMax.y, b2.pMax.y), std::min(b1.pMax.z, b2.pMax.z));
    return bu;
}

glm::vec3 BoundingBox::Centroid() const
{
    return 0.5f * pMin + 0.5f * pMax;
}


void BoundingBox::create()
{

    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    //createCubeVertexPositions(cub_vert_pos);
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMax.y, pMax.z);
    //LR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMin.y, pMax.z);
    //LL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMin.y, pMax.z);
    //UL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMax.y, pMax.z);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMax.y, pMin.z);
    //LR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMin.y, pMin.z);
    //LL
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMin.y, pMax.z);
    //UL
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMax.y, pMax.z);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMax.y, pMax.z);
    //LR
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMin.y, pMax.z);
    //LL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMin.y, pMin.z);
    //UL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMax.y, pMin.z);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMax.y, pMin.z);
    //LR
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMin.y, pMin.z);
    //LL
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMin.y, pMin.z);
    //UL
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMax.y, pMin.z);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMax.y, pMin.z);
    //LR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMax.y, pMax.z);
    //LL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMax.y, pMax.z);
    //UL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMax.y, pMin.z);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMin.y, pMax.z);
    //LR
    cub_vert_pos[idx++] = glm::vec3(pMax.x, pMin.y, pMin.z);
    //LL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMin.y, pMin.z);
    //UL
    cub_vert_pos[idx++] = glm::vec3(pMin.x, pMin.y, pMax.z);









    //createCubeVertexNormals(cub_vert_nor);
    idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }








    //createCubeIndices(cub_idx);
    idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }


    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = glm::vec3(0.4f, 0.5f, 0.5f);
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));
}


GLenum BoundingBox::drawMode(){return GL_LINES;}

