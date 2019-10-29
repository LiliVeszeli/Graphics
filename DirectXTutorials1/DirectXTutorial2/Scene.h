#ifndef _SCENE_H_INCLUDED_
#define _SCENE_H_INCLUDED_

//--------------------------------------------------------------------------------------
// Scene Geometry
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitScene();

// Release the geometry resources created above
void ReleaseScene();


//--------------------------------------------------------------------------------------
// Scene Render and Update
//--------------------------------------------------------------------------------------

void RenderScene();

// frameTime is the time passed since the last frame
void UpdateScene(float frameTime);


#endif //_SCENE_H_INCLUDED_
