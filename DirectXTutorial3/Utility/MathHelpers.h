//--------------------------------------------------------------------------------------
// Math convenience functions
//--------------------------------------------------------------------------------------

#ifndef _MATH_HELPERS_H_DEFINED_
#define _MATH_HELPERS_H_DEFINED_

#include "CMatrix4x4.h"
#include <cmath>


// Surprisingly, pi is not *officially* defined anywhere in C++
const float PI = 3.14159265359f;


// Pass an angle in degrees, returns the angle in radians
inline float ToRadians(float d)
{
    return  d * PI / 180.0f;
}

// Pass an angle in radians, returns the angle in degrees
inline float ToDegrees(float r)
{
    return  r * 180.0f / PI;
}


// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 4.0f / 3.0f, float FOVx = ToRadians(90), float nearClip = 0.1f, float farClip = 10000.0f)
{
    float tanFOVx = std::tan(FOVx * 0.5f);
    float scaleX  = 1.0f / tanFOVx;
    float scaleY  = aspectRatio / tanFOVx;
    float scaleZa = farClip / (farClip - nearClip);
    float scaleZb = -nearClip * scaleZa;

    return CMatrix4x4{ scaleX,   0.0f,    0.0f,   0.0f,
                       0.0f,   scaleY,    0.0f,   0.0f,
                       0.0f,     0.0f, scaleZa,   1.0f,
                       0.0f,     0.0f, scaleZb,   0.0f };
}


#endif // _MATH_HELPERS_H_DEFINED_
