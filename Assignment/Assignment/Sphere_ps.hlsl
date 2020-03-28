#include "Common.hlsli"

Texture2D DiffuseMap : register(t0); // A diffuse map is the main texture for a model.
                                        // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0);

float4 main(SimplePixelShaderInput input) : SV_Target
{
	
	
	float3 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv).rgb;

    // Blend texture colour with fixed per-object colour
	float3 finalColour = diffuseMapColour;

	return float4(finalColour, 1.0f); // Always use 1.0f for alpha -
}