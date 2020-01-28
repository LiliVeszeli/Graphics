//--------------------------------------------------------------------------------------
// Coloured Texture Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader that blends model vertex colours with a diffuse texture map

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

//****

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

//****

//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader blends model vertex colours with a diffuse texture map
float4 main(PixelShaderInput input) : SV_Target
{
    // Extract a colour for this pixel from the diffuse map (texture) using the sampler set up in the C++ code.
    // Although there is no lighting in this lab, we will stick to the conventional names for textures, with the diffuse map being
    // the main texture representing the look of a surface
   // float3 diffuseMapColour = DiffuseMap.Sample(Bilinear, input.uv).rgb;
	/*** MISSING - sample a texel using the texture and sampler declared above, and the UVs passed in from the vertex shader ***/
                              /***           use previous lab effect files to help you, but be aware that the variable names are different here        ***/

    // Extract diffuse material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    float3 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv).rgb;

    // Combine model vertex colour with texture colour - multiplicative blending is usual here.
    float4 finalPixelColour;
    finalPixelColour.rgb = input.vertexColour * diffuseMapColour;
    finalPixelColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

    return finalPixelColour;
}