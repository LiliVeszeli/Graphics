//--------------------------------------------------------------------------------------
// Basic Transformation and Colour Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to render anything

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

	float4 normalPosition = float4(modelVertex.normal, 0);

    // Multiply by the world matrix passed from C++ to transform the model vertex position into world space. 
    float4 worldPosition = mul(gWorldMatrix, modelPosition);

	float4 worldNormal = mul( gWorldMatrix, normalPosition);
	worldNormal = normalize(worldNormal);

    // In a similar way use the view matrix to transform the vertex from world space into view space (camera's point of view)
    // and then use the projection matrix to transform the vertex to 2D projection space
    float4 viewPosition      = mul(gViewMatrix, worldPosition); // Read the comments, ah, but if you're reading this then you already knew that... //missing
    output.projectedPosition = mul(gProjectionMatrix, viewPosition); //missing

    // Temporary lines, replace each with the full lighting code as you do the lab exercise
    output.diffuseLight  = 1.0f;
    output.specularLight = 0.0f;

    // Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
    output.uv = modelVertex.uv;

    return output; // Ouput data sent down the pipeline (to the pixel shader)
}
