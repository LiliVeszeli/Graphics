#include "Common.hlsli" // Shaders can also use include files - note the extension



TextureCube CubeMap: register(t0); 
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

float4 main(LightingPixelShaderInput input) : SV_Target
{
    
   
    
    float4 cubeTexture = CubeMap.Sample(TexSampler, input.worldNormal);

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
    float3 light2Vector = gLight2Position - input.worldPosition;
    float light2Distance = length(light2Vector);
    float3 light2Direction = light2Vector / light2Distance;
    float3 diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Distance;

    halfway = normalize(light2Direction + cameraDirection);
    float3 specularLight2 = diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
    
     // Light 3
    float3 light3Vector = gLight3Position - input.worldPosition;
    float light3Distance = length(light2Vector);
    float3 light3Direction = light3Vector / light3Distance;
    float3 diffuseLight3 = gLight3Colour * max(dot(input.worldNormal, light3Direction), 0) / light3Distance;

    halfway = normalize(light3Direction + cameraDirection);
    float3 specularLight3 = diffuseLight3 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
    
     // Light 6
    float3 light6Vector = gLight6Position - input.worldPosition;
    float light6Distance = length(light6Vector);
    float3 light6Direction = light6Vector / light6Distance; // Quicker than normalising as we have length for attenuation
    float3 diffuseLight6 = gLight6Colour * max(dot(input.worldNormal, light6Direction), 0) / light6Distance;

    halfway = normalize(light6Direction + cameraDirection);
    float3 specularLight6 = diffuseLight6 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
    
    
    
    

  
    //light calculation  
    float3 finalColour = (gAmbientColour + diffuseLight1 + diffuseLight2 + diffuseLight3 + diffuseLight6) * cubeTexture.rgb +
                         (specularLight1 + specularLight2 + specularLight3 + specularLight6) * cubeTexture.a;
    
   
    return float4(finalColour, 1.0f);
}