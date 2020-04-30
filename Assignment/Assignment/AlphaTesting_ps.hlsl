

#include "Common.hlsli" // Shaders can also use include files - note the extension


Texture2D DiffuseMap : register(t0);
SamplerState TexSampler : register(s0);


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples diffuse texture map (with alpha)
float4 main(LightingPixelShaderInput input) : SV_Target
{
    float4 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv);
    
   if (diffuseMapColour.a < 0.5)
   {
       discard;
   }
  

        return diffuseMapColour;
}