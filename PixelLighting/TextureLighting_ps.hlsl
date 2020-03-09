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
Texture2D    DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular
                                                // map (shininess level) in their alpha channel. Repurposing the alpha channel means we can't use alpha blending
                                                // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
	float3 lightDirection = normalize(gLightPosition - input.worldPosition.xyz);
	float3 diffuseLight = gLightColour * max(dot(input.worldNormal, lightDirection), 0);


	float3 lightDirection2 = normalize(gLight2Position - input.worldPosition.xyz);
	float3 diffuseLight2 =  gLight2Colour * max(dot(input.worldNormal, lightDirection2), 0);

	float3 cameraDirection = normalize(gCameraPosition - input.worldPosition.xyz);

	float3 halfway = normalize(lightDirection + cameraDirection);
	float3 specularLight = gLightColour * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

	halfway = normalize(lightDirection2 + cameraDirection);
	float3 specularLight2 = gLight2Colour * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

    // Sample diffuse material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    // Ignoring any alpha in the texture, just reading RGB
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv).rgba;
    float3 diffuseMaterialColour = textureColour;
    float specularMaterialColour = DiffuseSpecularMap.Sample(TexSampler, input.uv).a;

    float3 finalColour = (gAmbientColour + diffuseLight + diffuseLight2) * diffuseMaterialColour + (specularLight + specularLight2) * specularMaterialColour;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}