//--------------------------------------------------------------------------------------
// Simple Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader that starts out drawing all pixels the same colour

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------
// The later exercises will introduce textures although we will not look at them properly until later labs

Texture2D    DiffuseMap : register(t0); // A diffuse map is the main texture for a model - the t0 indicates it
                                        // uses slot 0. Each shader only has a fixed number of slots for textures
SamplerState Bilinear   : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic
                                        // The s0 means use slot 0. There are a fixed number of slots for samplers.


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This pixel shader combines texture and lighting colour to get a final pixel colour
float4 main(PixelShaderInput input) : SV_Target
{
    // Extract diffuse material colour for this pixel from a texture (will cover textures in next lab)
    float3 diffuseMaterial = DiffuseMap.Sample(Bilinear, input.uv).rgb;

    // Assume specular material colour is white (i.e. highlights are a full, untinted reflection of light)
    float3 specularMaterial = 1.0f;

    // Combine colours (lighting, textures) for final pixel colour
    float4 finalPixelColour;
    finalPixelColour.rgb = input.diffuseLight + input.specularLight;
    finalPixelColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

    return finalPixelColour;
}