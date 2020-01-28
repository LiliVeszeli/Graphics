//--------------------------------------------------------------------------------------
// Common include file for all shaders
//--------------------------------------------------------------------------------------
// Using include files to define the type of data passed between the shaders


//--------------------------------------------------------------------------------------
// Shader input / output
//--------------------------------------------------------------------------------------

// The structure below describes the vertex data to be sent into the vertex shader.
// This structure should match the vertices typed into the C++ program, which used the
// structure "BasicVertex", so this structure will match that one.
struct BasicVertex
{
    float3 position : position;
    float3 colour   : colour;
    float2 uv       : uv;
};


// This structure describes what data the pixel shader receives. It typically gets whatever
// data is output from the vertex shader - i.e. the vertex shader output is the pixel shader
// input. In this example, the vertex shader outputs a projected 2D position, mesh colour
// and texture coordinates
struct PixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    float3 vertexColour : vertexColour; // In this lab the meshes have a colour per vertex, passed to the pixel shader to blend with texture colour
    float2 uv           : uv;           // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned"
                                        // to that vertex. The coordinates are 2D for 2D images. Range is (0,0)->(1,1) from top-left to bottom right
                                        // of the texture. Because the y-coordinate in graphics sometimes gets flipped from 3D to 2D there can be
                                        // confusion with the V coordinate of textures. If a texture appears upside down on a mesh then the V 
                                        // coordinate needs to be negated.
};


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

// These structures are "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the matrices used to position the camera are updated from C++ to GPU every frame.
// These variables must match exactly the gPerFrameConstants structure in Scene.cpp
cbuffer PerFrameConstants : register(b0) // The b0 gives this constant buffer the number 0 - used in the C++ code
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix; // The above two matrices multiplied together to combine their effects
}
// Note we don't need the name of the constant buffer to access the variables inside, so we can just write gViewMatrix in the shader for example


// If we have multiple models then we need to update the world matrix from C++ to GPU multiple times per frame because we
// only have one world matrix here. Because this data is updated more frequently it is kept in a different buffer for better performance.
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The b1 gives this constant buffer the number 1 - used in the C++ code
{
    float4x4 gWorldMatrix;
}
