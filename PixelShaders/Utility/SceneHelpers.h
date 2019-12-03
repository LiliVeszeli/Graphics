//--------------------------------------------------------------------------------------
// Helper functions to unclutter and simplify main code (Scene.cpp/.h)
//--------------------------------------------------------------------------------------
// Code in .cpp file

#ifndef _SCENE_HELPERS_H_INCLUDED_
#define _SCENE_HELPERS_H_INCLUDED_

#include "CMatrix4x4.h"

// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 4.0f / 3.0f, float FOVx = ToRadians(90),
                                float nearClip = 0.1f, float farClip = 10000.0f);


#endif //_SCENE_HELPERS_H_INCLUDED_
