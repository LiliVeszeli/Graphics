//--------------------------------------------------------------------------------------
// Texture Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader simply samples a diffuse texture map and tints with colours from vertex shadeer

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D    DiffuseMap : register(t0); // A diffuse map is the main texture for a model. It is the base colour of
                                        // the material in question. You may see the word "albedo map" also used for
                                        // this texture but there is a slight difference, covered in the MComp.
                                        // The t0 indicates this textuere is in slot 0. Each shader has a fixed number
                                        // of slots for textures. The C++ code must load the texture into the this slot

SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic
                                        // These are prepared in the C++ code and attached to a GPU sampler slot.
                                        // The s0 means use slot 0. There are a fixed number of slots for samplers.
                                        // Texture slots and sampler slots are independent from each other, i.e. You 
                                        // don't have to use the same number for both texture slot and sampler slot.


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(PixelShaderInput input) : SV_Target
{
    // Sample diffuse material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    // Ignoring any alpha in the texture, just reading RGB
    float3 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv).rgb;

    float3 finalColour = input.diffuseLight * diffuseMapColour + input.specularLight;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no blending in this lab
}