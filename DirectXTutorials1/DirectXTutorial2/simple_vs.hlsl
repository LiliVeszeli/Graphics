//--------------------------------------------------------------------------------------
// Simple Vertex Shader
//--------------------------------------------------------------------------------------
// Shaders - we won't look at shaders until later in the module, but they are needed to
// render anything. Very simple shaders are used in this tutorial

float4 main(float4 Pos : POSITION) : SV_POSITION
{
    return Pos;
}
