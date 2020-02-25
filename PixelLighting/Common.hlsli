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
    float3 normal   : normal;
    float2 uv       : uv;
};


// This structure describes what data the lighting pixel shader receives from the vertex shader.
// The projected position is a requieed output from all vertex shaders - where the vertex is on the screeen
//*** The diffuse and specular light are calculated in the vertex shader at the start of this lab, and so they
//    are passed on to the pixel shader via this structure where they will affect the colours of the pixels
// The texture coordinates (uv) are passed from vertex shader to pixel shader unchanged to allow textures to be sampled
struct LightingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition; 
    float3 worldNormal : worldNormal; 
    
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
};



// This structure is similar to the one above but for the light model, which isn't itself lit. Leave this structure alone.
struct SimplePixelShaderInput
{
    float4 projectedPosition : SV_Position;
    float2 uv : uv;
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

    float3   gLightPosition; // 3 floats: x, y z
    float    padding1;       // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
                             // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
                             // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
    float3   gLightColour;
    float    padding2;

    float3   gAmbientColour;
    float    gSpecularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

    float3   gCameraPosition;
    float    padding3;
}
// Note we don't need the name of the constant buffer to access the variables inside, so we can just write gViewMatrix in the shader for example


// If we have multiple models then we need to update the world matrix from C++ to GPU multiple times per frame because we
// only have one world matrix here. Because this data is updated more frequently it is kept in a different buffer for better performance.
// We also keep other data that changes per-model here
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The b1 gives this constant buffer the number 1 - used in the C++ code
{
    float4x4 gWorldMatrix;

    float3   gObjectColour;
    float    padding4;  // See notes on padding in structure above
}
