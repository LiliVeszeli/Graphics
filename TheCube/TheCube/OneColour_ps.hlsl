//--------------------------------------------------------------------------------------
// One Colour Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader that expects colour data in its input and draws all pixels in that one colour

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Whatever colour the vertex shader output - all pixels get that colour
float4 main(PixelShaderInput input) : SV_Target
{
    return input.colour; 
}