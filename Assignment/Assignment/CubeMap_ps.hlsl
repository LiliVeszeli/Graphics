#include "Common.hlsli" // Shaders can also use include files - note the extension


//Texture2D DiffuseSpecularMap : register(t0); // Diffuse map (main colour) in rgb and specular map (shininess level) in alpha - C++ must load this into slot 0
TextureCube CubeMap: register(t0); // Normal map in rgb - C++ must load this into slot 1
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

float4 main(LightingPixelShaderInput input) : SV_Target
{
    //float3 vec = gCameraPosition - (float3)gWorldMatrix[3];
	// Get the texture normal from the normal map. The r,g,b pixel values actually store x,y,z components of a normal. However, r,g,b
	// values are stored in the range 0->1, whereas the x, y & z components should be in the range -1->1. So some scaling is needed
    //float3 textureNormal = CubeMap.Sample(SkyboxSampler, input.worldPosition).rgb; // Scale from 0->1 to -1->1

    
    float4 cubeTexture = CubeMap.Sample(TexSampler, input.worldPosition);

	///////////////////////
	// Calculate lighting

   // Lighting equations
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

    // Light 1
    float3 light1Vector = gLight1Position - input.worldPosition;
    float light1Distance = length(light1Vector);
    float3 light1Direction = light1Vector / light1Distance; // Quicker than normalising as we have length for attenuation
    float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, light1Direction), 0) / light1Distance;

    float3 halfway = normalize(light1Direction + cameraDirection);
    float3 specularLight1 = diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);


    // Light 2
    float3 light2Vector = gLight4Position - input.worldPosition;
    float light2Distance = length(light2Vector);
    float3 light2Direction = light2Vector / light2Distance;
    float3 diffuseLight2 = gLight4Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Distance;

    halfway = normalize(light2Direction + cameraDirection);
    float3 specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

    float3 finalColour = (gAmbientColour + diffuseLight1+ diffuseLight2) * cubeTexture.rgb +
                         (specularLight1+ specularLight2) * cubeTexture.a;
    
   // float3 finalColour = cubeTexture.rgb;

    return float4(finalColour, 1.0f);
}