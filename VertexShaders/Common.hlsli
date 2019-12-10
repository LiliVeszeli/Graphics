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
    float3 position : position; // The part after the colon is the "semantic" - they used to have special
    float3 normal   : normal;   // values but in modern shaders most of these can be anything - I use the
    float2 uv       : uv;       // same name as the variable in most cases
};


// This structure describes what data the pixel shader receives. It typically gets whatever
// data is output from the vertex shader - i.e. the vertex shader output is the pixel shader
// input. In this example, the vertex shader outputs a projected 2D position, lighting colours
// and texture coordinates
struct PixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    float3 diffuseLight  : diffuseLight;
    float3 specularLight : specularLight;
    float2 uv            : uv;
};


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

//**** 

// These structures are "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the matrices used to position the camera, and the lighting information are updated from C++
// to GPU every frame. These variables must match exactly the gPerFrameConstants structure in Scene.cpp
cbuffer PerFrameConstants : register(b0) // The b0 gives this constant buffer the number 0 - used in the C++ code
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix; // The above two matrices multiplied together to combine their effects

    float3   lightPosition;
    float    padding1;      // IMPORTANT technical point: shaders work with float4 values. If our variables don't align to
                            // the size of a float4 then HLSL will insert padding. I find it better to do this manually by
                            // adding these unused padding variables. Put the same variables in the C++ structure so they match
    float3   lightColour;
    float    padding2;

    float3   ambientColour;
    float    specularPower;  // IMPORTANT: In this case we actually have a useful float variable that we can use to pad to a float4

    float3   cameraPosition;
    float    padding3;
}
// Note we don't need the name of the constant buffer to access the variables inside, so we can just write gViewMatrix in the shader for example


// If we have multiple models then we need to update the world matrix from C++ to GPU multiple times per frame because we
// only have one world matrix here. Because this data is updated more frequently it is kept in a different buffer for better performance.
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The b1 gives this constant buffer the number 1 - used in the C++ code
{
    float4x4 gWorldMatrix;
}

//**** 
