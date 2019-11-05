//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

//****

// This structures is a constant buffers - a way of passing data over from C++ to the GPU every frame.
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the "constants" are the matrices used to position model & camera - updated from C++ to GPU every frame
// These variables must match exactly the gSceneConstants structure in Scene.cpp
cbuffer SceneConstants : register(b0) // The register part ensures that this constant buffer is numbered 0 - needed for C++ code
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

//****


//--------------------------------------------------------------------------------------
// Basic Transformation Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to render anything

// Vertex shader updated to transform a 3D model into 2D using matrices - we'll cover this later
float4 main(float4 modelPos : POSITION) : SV_POSITION
{
    float4 worldPos     = mul(worldMatrix,      modelPos); // Note we don't need the name of the constant buffer to access the variables inside
    float4 viewPos      = mul(viewMatrix,       worldPos);
    float4 projectedPos = mul(projectionMatrix, viewPos);
    return projectedPos;
}
