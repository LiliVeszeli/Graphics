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
                                        // is in slot 0 (each shader can have only a fixed number of textures)
SamplerState Bilinear   : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic
                                        // The s0 means use slot 0. There are a fixed number of slots for samplers.


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// Most basic pixel shader functionality to start with
float4 main(PixelShaderInput input) : SV_Target
{
	float sinY = sin(input.uv.y * radians(360.0f) + Wiggle);
	input.uv.x += 0.1f * sinY;

	float sinX = sin(input.uv.x * radians(360.0f) + Wiggle);
	input.uv.y += 0.1f * sinX;


	float3 textureColour = DiffuseMap.Sample(Bilinear, input.uv);

    float4 colour;
    colour.r = 1.0f;
    colour.g = 1.0f;
    colour.b = 0.0f;
    colour.a = 1.0f;

	
    return float4 (textureColour*input.colour.rgb, 1);
}