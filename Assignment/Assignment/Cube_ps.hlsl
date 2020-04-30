#include "Common.hlsli"


Texture2D DiffuseSpecularMap : register(t2);
Texture2D DiffuseMap1 : register(t0); // A diffuse map is the main texture for a model.
Texture2D DiffuseMap2 : register(t1);



// The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0);



float4 main(LightingPixelShaderInput input) : SV_Target
{
	
	float3 diffuseMapColour1 = DiffuseMap1.Sample(TexSampler, input.uv).rgb;
	float3 diffuseMapColour2 = DiffuseMap2.Sample(TexSampler, input.uv).rgb;

	
	float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);
	
	
	
	///2
	float3 light2Direction = normalize(gLight2Position - input.worldPosition);
	float light2Distance = length(gLight2Position - input.worldPosition);
	float3 diffuseLight2 = (gLight2Colour * max(dot(input.worldNormal, light2Direction), 0)) / light2Distance;
	float3 halfway = normalize(light2Direction + cameraDirection);
	float3 specularLight2 = gLight2Colour * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);


	///3
	float3 light3Direction = normalize(gLight3Position - input.worldPosition);
	float light3Distance = length(gLight3Position - input.worldPosition);
	float3 diffuseLight3 = (gLight3Colour * max(dot(input.worldNormal, light3Direction), 0)) / light3Distance;
	halfway = normalize(light3Direction + cameraDirection);
	float3 specularLight3 = gLight3Colour * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);


	// Sum the effect of the lights - add the ambient at this stage rather than for each light (or we will get too much ambient)
	float3 diffuseLight = gAmbientColour  + diffuseLight2 + diffuseLight3;
	float3 specularLight = + specularLight2 + specularLight3;





	////////////////////
	// Combine lighting and textures

    // Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
	float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
	float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
	float specularMaterialColour = textureColour.a; // Specular material colour in texture A (shininess of the surface)


	
	
    // Blend texture colour with fixed per-object colour
	float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour + lerp(diffuseMapColour1, diffuseMapColour2, change);
	return float4(finalColour, 1.0f); // Always use 1.0f for output alpha - no alpha blending in this lab

	
}