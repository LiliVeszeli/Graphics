//--------------------------------------------------------------------------------------
// Helper functions to unclutter and simplify main code (Scene.cpp/.h)
//--------------------------------------------------------------------------------------
// Code in .cpp file

#ifndef _SCENE_HELPERS_H_INCLUDED_
#define _SCENE_HELPERS_H_INCLUDED_

#include "CMatrix4x4.h"
#include "../Common.h"

//--------------------------------------------------------------------------------------
// Geometry creation
//--------------------------------------------------------------------------------------

// Create a "vertex layout" object that describes for DirectX what is in your geometry vertices (position, colour, normal etc.) 
// Returns a DirectX layout object on success, nullptr on failure
ID3D11InputLayout* CreateVertexLayout(D3D11_INPUT_ELEMENT_DESC* vertexDescData, int numDescElements);


// Create a vertex buffer on the GPU and copy the given data into it
// Returns a DirectX buffer object on success, nullptr on failure
ID3D11Buffer* CreateVertexBuffer(void* vertexData, int numVertices, int vertexSize);

// Create an index buffer on the GPU and copy the given data into it
// Returns a DirectX buffer object on success, nullptr on failure
ID3D11Buffer* CreateIndexBuffer(void* indexData, int numIndices);


//--------------------------------------------------------------------------------------
// Rendering helpers
//--------------------------------------------------------------------------------------

// Template function to update a constant buffer. Pass the DirectX constant buffer object and the C++ data structure
// you want to update it with. The structure will be copied in full over to the GPU constant buffer, where it will
// be available to shaders. This is used to update model and camera positions, lighting data etc.
template <class T>
void UpdateConstantBuffer(ID3D11Buffer* buffer, const T& bufferData)
{
    D3D11_MAPPED_SUBRESOURCE cb;
    gD3DContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
    memcpy(cb.pData, &bufferData, sizeof(T));
    gD3DContext->Unmap(buffer, 0);
}

// Select what geometry data will be rendered next: vertex buffer, index buffer, vertex layout and topology
// Helper function to collect together a common code sequence and simplify main code
// indexBuffer can be nullptr if not using an index buffer
void SelectGeometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer,
                    ID3D11InputLayout* vertexLayout, int vertexSize,
                    D3D_PRIMITIVE_TOPOLOGY topology);



//--------------------------------------------------------------------------------------
// Camera helpers
//--------------------------------------------------------------------------------------

// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
CMatrix4x4 MakeProjectionMatrix(float aspectRatio = 4.0f / 3.0f, float FOVx = ToRadians(60),
                                float nearClip = 0.1f, float farClip = 10000.0f);


#endif //_SCENE_HELPERS_H_INCLUDED_
