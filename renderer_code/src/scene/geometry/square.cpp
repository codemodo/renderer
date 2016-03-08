#include <scene/geometry/square.h>


BoundingBox SquarePlane::GetBoundingBox()
{
    glm::vec3 p1 = glm::vec3(transform.T() * glm::vec4(-0.5f, -0.5f, -0.5f, 1));
    glm::vec3 p2 = glm::vec3(transform.T() * glm::vec4(0.5f, 0.5f, 0.5f, 1));
    BoundingBox* bb;
    bb =  new BoundingBox(p1, p2);

    return *bb;
}

void SquarePlane::ComputeArea()
{
    glm::vec3 scale = transform.getScale();
    area = scale.x * scale.y;
}

Intersection SquarePlane::GetSampledIntersection(float randX, float randY, const glm::vec3 &isx_normal)
{
    glm::vec4 point = glm::vec4(-0.5 + randX, -0.5 + randY, 0.f, 1.f);
    Intersection result;
    result.point = glm::vec3(transform.T() * point);
    result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(point)), 0)));
    result.object_hit = this;
    result.texture_color = Material::GetImageColor(GetUVCoordinates(glm::vec3(point)), material->texture);
    return result;
}




Intersection SquarePlane::GetIntersection(Ray r)
{
    //Transform the ray
    Ray rLocal = r.GetTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - rLocal.origin)) / glm::dot(glm::vec3(0,0,1), rLocal.direction);
    glm::vec4 P = glm::vec4(t * rLocal.direction + rLocal.origin, 1);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(P)), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        //was interp
        result.texture_color = Material::GetImageColor(GetUVCoordinates(glm::vec3(P)), material->texture);

        glm::vec3 T = glm::normalize(glm::cross(glm::vec3(0,1,0), ComputeNormal(glm::vec3(P))));
        glm::vec3 B = glm::cross(ComputeNormal(glm::vec3(P)), T);
        result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(T, 0)));
        result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(B, 0)));

        return result;
    }
    return result;
}



glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 SquarePlane::ComputeNormal(const glm::vec3 &P)
{
        return glm::vec3(0,0,1);
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}
