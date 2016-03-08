#include <scene/geometry/disc.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

void Disc::ComputeArea()
{
    glm::vec3 scale = transform.getScale();
    area = M_PI * (0.5 * scale.x) * (0.5 * scale.y);
}

BoundingBox Disc::GetBoundingBox()
{
    BoundingBox* bb;
    bb =  new BoundingBox();
    return *bb;
}

Intersection Disc::GetSampledIntersection(float randX, float randY, const glm::vec3 &isx_normal)
{
    float r = sqrtf(randX);
    float theta = 2.f * M_PI * randY;
    glm::vec4 point = glm::vec4(
                        r*cosf(theta),
                        r*sinf(theta),
                        0.f,
                        1.f
                       );
    Intersection result;
    result.point = glm::vec3(transform.T() * point);
    result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(point)), 0)));
    result.object_hit = this;
    result.texture_color = Material::GetImageColor(GetUVCoordinates(glm::vec3(point)), material->texture);
    return result;
}


Intersection Disc::GetIntersection(Ray r)
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    glm::vec4 P = glm::vec4(t * r_loc.direction + r_loc.origin, 1);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 0.25f)
    {
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(P)), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        //was interp
        result.texture_color = Material::GetImageColor(GetUVCoordinates(glm::vec3(P)), material->texture);

        //Compute tangent and bitangent
        glm::vec3 T = glm::normalize(glm::cross(glm::vec3(0,1,0), ComputeNormal(glm::vec3(P))));
        glm::vec3 B = glm::cross(ComputeNormal(glm::vec3(P)), T);
        result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(T, 0)));
        result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(B, 0)));
    }
    return result;
}

glm::vec2 Disc::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 Disc::ComputeNormal(const glm::vec3 &P)
{
    return glm::vec3(0,0,1);
}

void Disc::create()
{
    GLuint idx[54];
    //18 tris, 54 indices
    glm::vec3 vert_pos[20];
    glm::vec3 vert_nor[20];
    glm::vec3 vert_col[20];

    //Fill the positions, normals, and colors
    glm::vec4 pt(0.5f, 0, 0, 1);
    float angle = 18.0f * DEG2RAD;
    glm::vec3 axis(0,0,1);
    for(int i = 0; i < 20; i++)
    {
        //Position
        glm::vec3 new_pt = glm::vec3(glm::rotate(glm::mat4(1.0f), angle * i, axis) * pt);
        vert_pos[i] = new_pt;
        //Normal
        vert_nor[i] = glm::vec3(0,0,1);
        //Color
        vert_col[i] = material->base_color;
    }

    //Fill the indices.
    int index = 0;
    for(int i = 0; i < 18; i++)
    {
        idx[index++] = 0;
        idx[index++] = i + 1;
        idx[index++] = i + 2;
    }

    count = 54;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx, 54 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(vert_pos, 20 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(vert_nor, 20 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(vert_col, 20 * sizeof(glm::vec3));
}
