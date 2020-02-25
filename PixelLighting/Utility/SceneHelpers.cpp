//--------------------------------------------------------------------------------------
// Helper functions to unclutter and simplify main code (Scene.cpp/.h)
//--------------------------------------------------------------------------------------

#include "SceneHelpers.h"
#include "../Shader.h"
#include <cmath>
#include <cctype>
#include <atlbase.h> // C-string to unicode conversion function CA2CT



//--------------------------------------------------------------------------------------
// Geometry creation
//--------------------------------------------------------------------------------------

// Create a "vertex layout" object that describes for DirectX what is in your geometry vertices (position, colour, normal etc.) 
// Returns a DirectX layout object on success, nullptr on failure
ID3D11InputLayout* CreateVertexLayout(D3D11_INPUT_ELEMENT_DESC* vertexDescData, int numDescElements)
{
    ID3D11InputLayout* vertexLayout;

    auto shaderSignature = CreateSignatureForVertexLayout(vertexDescData, numDescElements);
    HRESULT hr = gD3DDevice->CreateInputLayout(vertexDescData, numDescElements,
                                               shaderSignature->GetBufferPointer(), shaderSignature->GetBufferSize(),
                                               &vertexLayout);
    if (shaderSignature)  shaderSignature->Release();
    
    if (FAILED(hr))  return nullptr;
    return vertexLayout;
}


// Create a vertex buffer on the GPU and copy the given data into it
// Returns a DirectX buffer object on success, nullptr on failure
ID3D11Buffer* CreateVertexBuffer(void* vertexData, int numVertices, int vertexSize)
{
    ID3D11Buffer* vertexBuffer;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;          // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = numVertices * vertexSize; // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData; // Fill the new vertex buffer with the array above as initial data
    initData.pSysMem = vertexData;
    
    HRESULT hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr))  return nullptr;
    return vertexBuffer;
}


// Create an index buffer on the GPU and copy the given data into it
// Returns a DirectX buffer object on success, nullptr on failure
ID3D11Buffer* CreateIndexBuffer(void* indexData, int numIndices)
{
    ID3D11Buffer* indexBuffer;

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;          // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = numIndices * sizeof(DWORD); // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData; // Fill the new vertex buffer with the array above as initial data
    initData.pSysMem = indexData;

    HRESULT hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &indexBuffer);
    if (FAILED(hr))  return nullptr;
    return indexBuffer;
}


//--------------------------------------------------------------------------------------
// Texture Loading
//--------------------------------------------------------------------------------------

// Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify texture loading
// This function requires you to pass a ID3D11Resource* (e.g. &gTilesDiffuseMap), which manages the GPU memory for the
// texture and also a ID3D11ShaderResourceView* (e.g. &gTilesDiffuseMapSRV), which allows us to use the texture in shaders
// The function will fill in these pointers with usable data. Returns false on failure
bool LoadTexture(std::string filename, ID3D11Resource** texture, ID3D11ShaderResourceView** textureSRV)
{
    // DDS files need a different function from other files
    std::string dds = ".dds"; // So check the filename extension (case insensitive)
    if (filename.size() >= 4 &&
        std::equal(dds.rbegin(), dds.rend(), filename.rbegin(), [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); }))
    {
        return SUCCEEDED(DirectX::CreateDDSTextureFromFile(gD3DDevice, CA2CT(filename.c_str()), texture, textureSRV));
    }
    else
    {
        return SUCCEEDED(DirectX::CreateWICTextureFromFile(gD3DDevice, gD3DContext, CA2CT(filename.c_str()), texture, textureSRV));
    }
}


//--------------------------------------------------------------------------------------
// Rendering Helpers
//--------------------------------------------------------------------------------------

// Select what geometry data will be rendered next: vertex buffer, index buffer, vertex layout and topology
// Helper function to collect together a common code sequence and simplify main code
// indexBuffer can be nullptr if not using an index buffer
void SelectGeometry(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer,
                    ID3D11InputLayout* vertexLayout, int vertexSize,
                    D3D_PRIMITIVE_TOPOLOGY topology)
{
    UINT stride = vertexSize;
    UINT offset = 0;
    gD3DContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Indicate the layout of our vertex buffer. Only needs to be done once unless we want to use a different buffer
    gD3DContext->IASetInputLayout(vertexLayout);

    // Select the index buffer created above to go along with the vertex buffer - indicate it uses 32-bit integers
    if (indexBuffer != nullptr)  gD3DContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Also indicate the primitive topology of the (index) buffer. Our buffer holds a triangle list - each set of 3 indexes
    // will be connected into a triangle. Again, there is no need to do this more than once if you are not changing topology
    gD3DContext->IASetPrimitiveTopology(topology);
}



//--------------------------------------------------------------------------------------
// Camera Helpers
//--------------------------------------------------------------------------------------

// A "projection matrix" contains properties of a camera. Covered mid-module - the maths is an optional topic (not examinable).
// - Aspect ratio is screen width / height (like 4:3, 16:9)
// - FOVx is the viewing angle from left->right (high values give a fish-eye look),
// - near and far clip are the range of z distances that can be rendered
CMatrix4x4 MakeProjectionMatrix(float aspectRatio /*= 4.0f / 3.0f*/, float FOVx /*= ToRadians(60)*/,
                                float nearClip /*= 0.1f*/, float farClip /*= 10000.0f*/)
{
    float tanFOVx = std::tan(FOVx * 0.5f);
    float scaleX = 1.0f / tanFOVx;
    float scaleY = aspectRatio / tanFOVx;
    float scaleZa = farClip / (farClip - nearClip);
    float scaleZb = -nearClip * scaleZa;

    return CMatrix4x4{ scaleX,   0.0f,    0.0f,   0.0f,
                         0.0f, scaleY,    0.0f,   0.0f,
                         0.0f,   0.0f, scaleZa,   1.0f,
                         0.0f,   0.0f, scaleZb,   0.0f };
}
