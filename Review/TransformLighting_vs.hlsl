//--------------------------------------------------------------------------------------
// Basic Transformation and Lighting Vertex Shader
//--------------------------------------------------------------------------------------

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Vertex shader gets vertices from the mesh one at a time. It transforms their positions
// from 3D into 2D (see lectures) and passes that position down the pipeline so pixels can
// be rendered. 
PixelShaderInput main(BasicVertex modelVertex)
{
    PixelShaderInput output; // This is the data the pixel shader requires from this vertex shader

    // Input position is x,y,z only - need a 4th element to multiply by a 4x4 matrix. Use 1 for a point (0 for a vector) - recall lectures
    float4 modelPosition = float4(modelVertex.position, 1); 

    // Multiply by the world matrix passed from C++ to transform the model vertex position into world space. 
    // In a similar way use the view matrix to transform the vertex from world space into view space (camera's point of view)
    // and then use the projection matrix to transform the vertex to 2D projection space (project onto the 2D screen)
    float4 worldPosition     = mul(gWorldMatrix,      modelPosition);
    float4 viewPosition      = mul(gViewMatrix,       worldPosition);
    output.projectedPosition = mul(gProjectionMatrix, viewPosition);

    // Also transform model normals into world space using world matrix - lighting will be calculated in world space
    float4 modelNormal = float4(modelVertex.normal, 0); // For normals add a 0 in the 4th element to indicate it is a vector
    float3 worldNormal = (float3)mul(gWorldMatrix, modelNormal); // Only needed 4th element to multiply by 4x4 matrix, not needed for lighting so discard with cast

    // Lighting equations (specular is an exercise)
    float3 lightDirection = normalize(gLightPosition - worldPosition.xyz);
    output.diffuseLight = gAmbientColour + gLightColour * max(dot(normalize(worldNormal), lightDirection), 0);

    output.specularLight = 0;

    // Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
    output.uv = modelVertex.uv;

    return output; // Ouput data sent down the pipeline (to the pixel shader)
}
