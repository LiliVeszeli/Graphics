//--------------------------------------------------------------------------------------
// Common include file for all shaders
//--------------------------------------------------------------------------------------
// Using include files to define the type of data passed between the shaders


//****
// The structure below describes the vertex data to be sent into the vertex shader.
// The vertex shader runs early in the Direct3D pipeline - it takes our 3D geometry, one 
// vertex at a time, and transforms it into 2D ready for rendering on the viewport.
// This structure should match the vertices typed into the C++ program, which used the
// structure "SimpleVertex". So you need to check that structure against this one.
//
// However, this shader is written in HLSL, not C++. The types and syntax are similar,
// but don't exactly match between the languages, but you should be able to work it out.
// If you compare this structure with the one in the C++, you will see that there is
// extra data here. The vertex shader is expecting us to provide it with more data from
// the cpp file. 
//
// >>> Your exercise is to update the *cpp* file to provide that extra data. You will
// >>> need to update the vertex structure and the cube vertex data in the cpp file.
// >>> No changes are actually required in this file...

struct SimpleVertex
{
    float3 position : position; // The part after the colon is the "semantic", we will see these in detail later
    float4 colour   : colour;
};


// This structure describes what data the pixel shader receives. It typically gets whatever
// data is output from the vertex shader - i.e. the vertex shader output is the pixel shader
// input. In this example, the vertex shader outputs a projected 2D position (we'll see later
// why it's a float4) and also a colour.
struct PixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, it uses the special semantic "SV_Position"
    float4 colour            : colour;
};
