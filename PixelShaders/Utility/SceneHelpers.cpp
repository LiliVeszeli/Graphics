//--------------------------------------------------------------------------------------
// Helper functions to unclutter and simplify main code (Scene.cpp/.h)
//--------------------------------------------------------------------------------------

#include "SceneHelpers.h"
#include <cmath>


// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
CMatrix4x4 MakeProjectionMatrix(float aspectRatio /*= 4.0f / 3.0f*/, float FOVx /*= ToRadians(90)*/,
                                float nearClip /*= 0.1f*/, float farClip /*= 10000.0f*/)
{
    float tanFOVx = std::tan(FOVx * 0.5f);
    float scaleX = 1.0f / tanFOVx;
    float scaleY = aspectRatio / tanFOVx;
    float scaleZa = farClip / (farClip - nearClip);
    float scaleZb = -nearClip * scaleZa;

    return CMatrix4x4{ scaleX,   0.0f,    0.0f,   0.0f,
                         0.0f, scaleY,    0.0f,   0.0f,
                         0.0f,   0.0f, scaleZa,   1.0f,
                         0.0f,   0.0f, scaleZb,   0.0f };
}
