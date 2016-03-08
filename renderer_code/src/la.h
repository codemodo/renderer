#pragma once

// NOTE: This #define forces GLM to use radians (not degrees) for ALL of its
// angle arguments. The documentation may not always reflect this fact.
#define GLM_FORCE_RADIANS

#define GLM_CIS277  // Don't copy this include!
// Primary GLM library
#    include <glm/glm.hpp>
// For glm::translate, glm::rotate, and glm::scale.
#    include <glm/gtc/matrix_transform.hpp>
// For glm::to_string.
#    include <glm/gtx/string_cast.hpp>
// For glm::value_ptr.
#    include <glm/gtc/type_ptr.hpp>
#undef GLM_CIS277

#include <QMatrix4x4>


static const float PI = 3.14159265358979323846f;
static const float TWO_PI = 2 * PI;
static const float INV_PI = 1.0f / PI;
static const float DEG2RAD = PI / 180.f;
static const float RAD2DEG = 180.f / PI;
static const float INFIN = (float)999999999;

/// 277 linear algebra namespace
namespace la
{
using namespace glm;

inline glm::vec3 UnifSampleDisc(float u1, float u2)
{
    float sx = 2 * u1 - 1.0f;
    float sy = 2 * u2 - 1.0f;
    float r, theta;

    if (sx == 0.0 && sy == 0.0)
    {
        return glm::vec3(0,0,0);
    }
    if (sx >= -sy)
    {
        if (sx > sy)
        {
            // Handle first region of disk
            r = sx;
            if (sy > 0.0) theta = sy/r;
            else          theta = 8.0f + sy/r;
        }
        else
        {
            // Handle second region of disk
            r = sy;
            theta = 2.0f - sx/r;
        }
    }
    else
    {
        if (sx <= sy)
        {
            // Handle third region of disk
            r = -sx;
            theta = 4.0f - sy/r;
        }
        else
        {
            // Handle fourth region of disk
            r = -sy;
            theta = 6.0f + sx/r;
        }
    }
    theta *= PI / 4.f;
    float dx = r * cosf(theta);
    float dy = r * sinf(theta);
    return glm::vec3(dx, dy, 0);
}




inline QMatrix4x4 to_qmat(const mat4 &m)
{
    return QMatrix4x4(value_ptr(transpose(m)));
}
}

/// Float approximate-equality comparison
template<typename T>
inline bool fequal(T a, T b, T epsilon = 0.0001){
    if (a == b) {
        // Shortcut
        return true;
    }

    const T diff = std::abs(a - b);
    if (a * b == 0) {
        // a or b or both are zero; relative error is not meaningful here
        return diff < (epsilon * epsilon);
    }

    return diff / (std::abs(a) + std::abs(b)) < epsilon;
}
