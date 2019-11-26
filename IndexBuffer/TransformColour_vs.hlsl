//--------------------------------------------------------------------------------------
// Basic Transformation and Colour Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to render anything

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

// These structures are "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the matrices used to position the camera are updated from C++ to GPU every frame
// These variables must match exactly the gPerFrameConstants structure in Scene.cpp
cbuffer PerFrameConstants : register(b0) // The register part ensures that this constant buffer is numbered 0 - needed for C++ code
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
}
// Note we don't need the name of the constant buffer to access the variables inside, so we can just write gViewMatrix for example


// In this exercise the matrices used to position the model are updated from C++ to GPU multiple times per frame,
// Because this data is updated more frequently it is kept in a different buffer (better performance).
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The register part ensures that this constant buffer is numbered 1 - needed for C++ code
{
    float4x4 gWorldMatrix;
}


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Vertex shader gets vertices from the mesh one at a time. It transforms their positions
// from 3D into 2D (see lectures) and passes that position down the pipeline so pixels can
// be rendered. The shader also passes on the colour of each model vertex
PixelShaderInput main(SimpleVertex modelVertex)
{
    PixelShaderInput output; // This is the data the pixel shader requires from this vertex shader

    float4 modelPosition = float4(modelVertex.position, 1); // Input position is x,y,z only - add a 1 to show that
                                                            // these are points, not vectors (see lecture)

    // Use matrices to transform the mesh vertex position to 2D (will cover this later)
    float4 worldPos          = mul(gWorldMatrix,      modelPosition);
    float4 viewPos           = mul(gViewMatrix,       worldPos);
    output.projectedPosition = mul(gProjectionMatrix, viewPos);

    // Also get the colour from the mesh vertex
    output.colour = modelVertex.colour;

    return output; // Ouput data sent down the pipeline (to the pixel shader)
}
