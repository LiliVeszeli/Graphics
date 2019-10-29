//--------------------------------------------------------------------------------------
// Functions for loading GPU shaders
//--------------------------------------------------------------------------------------
#ifndef _SHADER_H_INCLUDED_
#define _SHADER_H_INCLUDED_

#include <d3d11.h>
#include <string>
#include <vector>

ID3D11VertexShader* LoadVertexShader(std::string shaderName);
ID3D11PixelShader*  LoadPixelShader (std::string shaderName);

ID3DBlob* CreateSignatureForVertexLayout(const D3D11_INPUT_ELEMENT_DESC vertexLayout[], int numElements);

#endif //_SHADER_H_INCLUDED_
