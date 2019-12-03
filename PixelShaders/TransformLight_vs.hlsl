//--------------------------------------------------------------------------------------
// Basic Transformation and Colour Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to render anything

#include "Common.hlsli" // Shaders can also use include files - note the extension



//--------------------------------------------------------------------------------------
// Simple constants
//--------------------------------------------------------------------------------------

// Direction to the light and light colours (used in the vertex shader), we'll cover lighting properly later
static const float3 gLightColour    = { 1.0f, 0.8f, 0.4f };
static const float3 gLightDirection = { 0.707f, 0.707f, -0.707f };
static const float3 gAmbientColour  = { 0.2f, 0.2f, 0.3f };



//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Vertex shader gets vertices from the mesh one at a time. It transforms their positions
// from 3D into 2D (see lectures) and passes that position down the pipeline so pixels can
// be rendered. 
//
// This shader also calulates the light hitting each model vertex and also
// passes that colour to the pixel shader.
PixelShaderInput main(BasicVertex modelVertex)
{
    PixelShaderInput output; // This is the data the pixel shader requires from this vertex shader

    float4 modelPosition = float4(modelVertex.position, 1); // Input position is x,y,z only - need a 4th element, add 
                                                            // a 1 to show that these are points, not vectors (see lecture)

    // Use matrices to transform the mesh vertex position to 2D (will cover this later)
    float4 worldPos          = mul(gWorldMatrix,      modelPosition);
    float4 viewPos           = mul(gViewMatrix,       worldPos);
    output.projectedPosition = mul(gProjectionMatrix, viewPos);

    // Calculate lighting on this vertex - we will cover this properly in a later lecture
    float4 modelNormal = float4(modelVertex.normal, 0); // For the model's normals, add a 0 in the 4th element
                                                        // to indicate it is a vector
    float3 worldNormal = (float3)mul(gWorldMatrix, modelNormal);
    output.colour = gAmbientColour + gLightColour * saturate(dot(normalize(worldNormal), gLightDirection));
	

    return output; // Ouput data sent down the pipeline (to the pixel shader)
}
