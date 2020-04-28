//--------------------------------------------------------------------------------------
// Common include file for all shaders
//--------------------------------------------------------------------------------------
// Using include files to define the type of data passed between the shaders


//--------------------------------------------------------------------------------------
// Shader input / output
//--------------------------------------------------------------------------------------

// The structure below describes the vertex data to be sent into the all vertex shaders.
struct BasicVertex
{
    float3 position : position;
    float3 normal   : normal;
    float2 uv       : uv;
};


// The most basic pixel shader input, just the screen space position for the pixel
struct BasicPixelShaderInput
{
    float4 projectedPosition : SV_Position;
};


// This structure is similar to the above only used for the simply rendered light models, they aren't themselves lit but do use textures
struct SimplePixelShaderInput
{
    float4 projectedPosition : SV_Position;
    float2 uv : uv;
};

// Fairly standard pixel shader input when calculating lighting in the pixels shader
struct LightingPixelShaderInput
{
    float4 projectedPosition : SV_Position;
    float3 worldPosition : worldPosition; // Data required for lighting calculations in the pixel shader
    float3 worldNormal   : worldNormal;   // --"--
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

    float3   gLight1Position; // 3 floats: x, y z
    float    padding1;       // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
                             // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
                             // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
    float3   gLight1Colour;
    float    padding2;

    float3   gLight2Position;
    float    padding3;
    float3   gLight2Colour;
    float    padding4;

    float3   gAmbientColour;
    float    gSpecularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

    float3   gCameraPosition;
    float    padding5;

    float3   gOutlineColour;    // Cell shading outline colour
    float    gOutlineThickness; // Controls thickness of outlines for cell shading
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
    float    padding6;  // See notes on padding in structure above
}
