//--------------------------------------------------------------------------------------
// State creation
// - Sampler state (Bilinear, trilinear etc.)
// - Blender state (Additive blending, alpha blending etc.)
// - Rasterizer state (Wireframe mode, don't cull back faces etc.)
// - Depth stencil state (How to use the depth and stencil buffer)
//--------------------------------------------------------------------------------------
#ifndef _STATE_H_INCLUDED_
#define _STATE_H_INCLUDED_


// Create all the states used in this app, returns true on success
bool CreateStates();

// Release DirectX state objects
void ReleaseStates();


#endif //_STATE_H_INCLUDED_
