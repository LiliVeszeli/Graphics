//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "Shader.h"
#include "Input.h"
#include "Common.h"

// Maths classes you have seen in Games Dev 1
#include "CVector2.h" 
#include "CVector3.h" 
#include "CMatrix4x4.h"
#include "MathHelpers.h"  // Helper functions for maths
#include "SceneHelpers.h" // Helper functions to unclutter the code here

#include "ColourRGBA.h" 

#include <sstream>

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

// DirectX objects controlling the vertex & index buffers (mesh data on GPU) and vertex layout (description of a single vertex)
ID3D11InputLayout* gSimpleVertexLayout = nullptr;
ID3D11Buffer*      gSimpleVertexBuffer = nullptr;
ID3D11Buffer*      gSimpleIndexBuffer = nullptr;

// DirectX objects controlling the texture used in this lab
ID3D11Resource*           gCubeTexture = nullptr;    // This object represents the memory used by the texture on the GPU
ID3D11ShaderResourceView* gCubeTextureSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

// The world matrix for the cube - this positions and orients the cube and is updated every frame
CMatrix4x4 gCubeMatrix;


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
// Variables sent over to the GPU each frame

// These are the matrices used to position the camera. They updated from C++ to the GPU shaders *once per frame*
// We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU each frame when
// we have finished updating the scene. There is a structure in the vertex shader that exactly matches this one
struct
{
    CMatrix4x4 viewMatrix;
    CMatrix4x4 projectionMatrix;
} gPerFrameConstants;
ID3D11Buffer* gPerFrameConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure


// This is the matrix that positions the cube in the scene. Unlike the structure above this data can be updated and
// sent to the GPU several times every frame (once per cube). However, apart from that it works in the same way.
struct
{
    CMatrix4x4 worldMatrix;
	float cubeColour;
} gPerModelConstants;
ID3D11Buffer* gPerModelConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure



//--------------------------------------------------------------------------------------
// Geometry definitions and data
//--------------------------------------------------------------------------------------

// The content of a single vertex in the geometry to render. Stores the vertex position as usual, but this time also stores a vertex
// normal (for lighting) and a texture coordinate (for textures). Lighting and textures will make working with the shaders more
// interesting - we will cover these topics in detail in the next couple of weeks.
struct BasicVertex
{
    CVector3 position; // Model space position of the vertex (x,y,z)
    CVector3 normal;   // Vertex normal for the vertex (x,y,z)
    CVector2 uv;       // Texture coordinate (aka UVs) for the vertex (u,v)
};



// Describe the "input layout" //

// This describes the contents of the SimpleVertex structure above so DirectX will know what to expect when reading vertex data.
//
// There is one row in the layout array below for each data element in our SimpleVertex structure. So the first row below describes
// the fact that each vertex has a "Position" which is made of three 32-bit floats, that is offset 0 bytes from the start of the
// vertex structure. The next row says there is a "Colour" made of four floats, offset 12 bytse from the start (i.e. after the
// position). Note that in this array the floats are referred to as RGB colours even though they will hold x,y,z values.
// The "slot", and other values in the array below can be ignored in most cases.
//
// This structure should also match the vertex shader input structure
//
D3D11_INPUT_ELEMENT_DESC gSimpleVertexDesc[] =
{
    // Data Type,  Type Index,  Data format                   Slot  Offset    Other values can be ignored for now 
    { "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,  0,    0,        D3D11_INPUT_PER_VERTEX_DATA,  0 },
    { "Normal",    0,           DXGI_FORMAT_R32G32B32_FLOAT,  0,    12,       D3D11_INPUT_PER_VERTEX_DATA,  0 },
    { "UV",        0,           DXGI_FORMAT_R32G32_FLOAT,     0,    24,       D3D11_INPUT_PER_VERTEX_DATA,  0 },
};
int gSimpleVertexDescCount = sizeof(gSimpleVertexDesc) / sizeof(gSimpleVertexDesc[0]); // This gives a count of rows in the array above



// Geometry - the mesh to draw //

// A CPU-side array of vertices for the geometry we wish to render. Each vertex is the BasicVertex structure above.
// Note, even though we're using an index buffer here, there appears to be vertex duplication (a cube with more
// than 8 vertices). This is because the newly added vertex normals and texture coordinates (see BasicVertex
// structure above) are not the same on each face
BasicVertex gCubeVertices[] =
{
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 0.0f, 0.0f }, // Front face
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 1.0f, 1.0f },
												  
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 0.0f, 0.0f }, // Right face
	CVector3{  1.0f, 1.0f, 1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f,-1.0f, 1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
																
	CVector3{  1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 0.0f, 0.0f }, // Etc.
	CVector3{ -1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{  1.0f,-1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{ -1.0f,-1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 1.0f, 1.0f },
																
	CVector3{ -1.0f, 1.0f, 1.0f }, CVector3{ -1.0f, 0.0f, 0.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{ -1.0f, 0.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f,-1.0f, 1.0f }, CVector3{ -1.0f, 0.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{ -1.0f, 0.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
																
	CVector3{ -1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{  1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
																
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{  0.0f,-1.0f, 0.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  0.0f,-1.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f,-1.0f, 1.0f }, CVector3{  0.0f,-1.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f,-1.0f, 1.0f }, CVector3{  0.0f,-1.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
};
int gCubeNumVertices = sizeof(gCubeVertices) / sizeof(gCubeVertices[0]); // Total number of vertices in the array above



// The index buffer shows how to join together the vertices above into triangles. As this is a triangle list, each triplet
// of vertices define a single triangle. This defines a cube by joining up the vertices above into triangles.
//
DWORD gCubeIndices[] =
{
     0,  1,  2,		 2,  1,  3, // Front face
     4,  5,  6,		 6,  5,  7, // Right face
     8,  9, 10,		10,  9, 11, // etc.
    12, 13, 14,		14, 13, 15,
    16, 17, 18,		18, 17, 19,
    20, 21, 22,		22, 21, 23,
};
int gCubeNumIndices = sizeof(gCubeIndices) / sizeof(gCubeIndices[0]);



//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    HRESULT hr; // To hold DirectX return values

    //// Create a vertex and index buffer on the GPU ////
    
    // This is just a way to copy the vertex array above into GPU memory. When rendering, data needs to be in GPU memory.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage     = D3D11_USAGE_DEFAULT;      // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = gCubeNumVertices * sizeof(BasicVertex); // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData; // Fill the new vertex buffer with the array above as initial data
    initData.pSysMem = gCubeVertices;
    hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &gSimpleVertexBuffer);
    if (FAILED(hr))
    {
        gLastError = "Error creating vertex buffer";
        return false;
    }

    
    // Create index buffer. Same as above code - but this copies the index array from above into GPU memory.
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // Indicate it is a index buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;         // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = gCubeNumIndices * sizeof(DWORD); // Size of the buffer in bytes - each index is a DWORD (32-bit integer)
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    initData.pSysMem = gCubeIndices;
    hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &gSimpleIndexBuffer);
    if (FAILED(hr))
    {
        gLastError = "Error creating index buffer";
        return false;
    }


    // These lines convert the vertex layout described above into an object (gSimpleVertexLayout) used when rendering
    auto shaderSignature = CreateSignatureForVertexLayout(gSimpleVertexDesc, gSimpleVertexDescCount);
    hr = gD3DDevice->CreateInputLayout(gSimpleVertexDesc, gSimpleVertexDescCount,
                                       shaderSignature->GetBufferPointer(), shaderSignature->GetBufferSize(), &gSimpleVertexLayout);
    if (shaderSignature)  shaderSignature->Release();
    if (FAILED(hr))
    {
        gLastError = "Error creating input layout";
        return false;
    }


    //// Load / prepare textures on the GPU ////

    // Load a texture and create DirectX objects for it
    // Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify file loading
    hr = DirectX::CreateWICTextureFromFile(gD3DDevice, gD3DContext, L"tiles1.jpg", &gCubeTexture, &gCubeTextureSRV);
    if (FAILED(hr))
    {
        gLastError = "Error loading texture";
        return false;
    }



    return true;
}


// Prepare the scene
// Returns true on success
bool InitScene()
 {
    // Create GPU-side constant buffers to match the gPerFrameConstants and gPerModelConstants structures above
    // See the comments above where these variable are declared and also the UpdateScene function
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
        return false;
    }

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    if (gCubeTextureSRV)          gCubeTextureSRV->Release();
    if (gCubeTexture)             gCubeTexture->Release();
    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();
    if (gSimpleIndexBuffer)       gSimpleIndexBuffer->Release();
    if (gSimpleVertexBuffer)      gSimpleVertexBuffer->Release();
    if (gSimpleVertexLayout)      gSimpleVertexLayout->Release();
}


//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Called once a frame, from the loop in Main.cpp
void RenderScene()
{
    //// Per-frame set-up ////

    // Set the "back buffer" as the target for rendering. The "back buffer" is an off-screen viewport. When
    // we have finished drawing to the back buffer it sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);
    gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Clear the back buffer to a fixed colour
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, ClearColor);

    // Setup the viewport - defines which part of the window we will render to, almost always the whole window
    D3D11_VIEWPORT vp;
    vp.Width  = static_cast<FLOAT>(gViewportWidth);
    vp.Height = static_cast<FLOAT>(gViewportHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);


    // Send per-frame data (camera matrices here) over to the shaders on the GPU
    // See the section commented as "Constant Buffers" near the top of the file for more info about the data being sent here
    // - "Map" basically opens the GPU's constant buffer for writing
    // - "memcpy" copies the C++ data over to the GPU's constant buffer
    // - "Unmap" closes the GPU's buffer again - we must do this as soon as possible
    D3D11_MAPPED_SUBRESOURCE cb;
    gD3DContext->Map(gPerFrameConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
    memcpy(cb.pData, &gPerFrameConstants, sizeof(gPerFrameConstants));
    gD3DContext->Unmap(gPerFrameConstantBuffer, 0);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    // If you look at the vertex shader code, there is a structure with the same content that receives the data
    // The first parameter must match constant buffer number in the shader, so this is constant buffer 0 on the vertex shader
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); 



    //// Prepare for cube rendering ////

    // Select the vertex buffer created with our geometry in it - D3D will now use that data for rendering
    // Only needs to be done once unless we want to use a different buffer
    UINT stride = sizeof(BasicVertex); // Size of a single vertex in the buffer
    UINT offset = 0;
    gD3DContext->IASetVertexBuffers(0, 1, &gSimpleVertexBuffer, &stride, &offset);

    // Indicate the layout of our vertex buffer. Only needs to be done once unless we want to use a different buffer
    gD3DContext->IASetInputLayout(gSimpleVertexLayout);

    // Select the index buffer created above to go along with the vertex buffer - indicate it uses 32-bit integers
    gD3DContext->IASetIndexBuffer(gSimpleIndexBuffer, DXGI_FORMAT_R32_UINT, 0);  

    // Also indicate the primitive topology of the (index) buffer. Our buffer holds a triangle list - each set of 3 indexes
    // will be connected into a triangle. Again, there is no need to do this more than once if you are not changing topology
    gD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Select which shaders to use when rendering. Only need to do once if you are not changing shader
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);

    
    
    //// Render cube ////

    // Send the world matrix for the cube over to the shaders on the GPU
    // See the section commented as "Constant Buffers" near the top of the file for more info about the data being sent here
    // - "Map" basically opens the GPU's constant buffer for writing
    // - "memcpy" copies the C++ data over to the GPU's constant buffer
    // - "Unmap" closes the GPU's buffer again - we must do this as soon as possible
    gPerModelConstants.worldMatrix = gCubeMatrix;
	gPerModelConstants.cubeColour = 0.5f;

    gD3DContext->Map(gPerModelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
    memcpy(cb.pData, &gPerModelConstants, sizeof(gPerModelConstants));
    gD3DContext->Unmap(gPerModelConstantBuffer, 0);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    // If you look at the vertex shader code, there is a structure with the same content that receives the data
    // The first parameter must match constant buffer number in the shader, so this is constant buffer 0 on the vertex shader
    gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader
	gD3DContext->PSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader
	
	// Select the texture to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gCubeTextureSRV); // First parameter must match texture slot number in the shaer
    
    // Draw the geometry using an index buffer
    gD3DContext->DrawIndexed(36, 0, 0); // Draw the first 6 indexed vertices (2 triangles in a triangle list), 
		                               // starting at the beginning of the index list (second parameter 0) and with
		                               // no offset (third parameter 0 - an advanced topic)


    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
    //// Update camera ////

    // Create a matrix to position the camera - called the view (camera) matrix - we'll see this in more detail later
    gPerFrameConstants.viewMatrix = InverseAffine(MatrixTranslation(CVector3(0, 0, -4.0f)));

    // Create a "projection matrix" - this determines properties of the camera - again we'll see this later
    gPerFrameConstants.projectionMatrix = MakeProjectionMatrix(); // Using a helper function to make this special matrix


    //// Update cube 1 ////

    // Create a matrix to position and orientate the cube
    static float rotationX = 0, rotationY = 0;
    if (KeyHeld(Key_W))
    {
        rotationX += ToRadians(120) * frameTime;
    }
    if (KeyHeld(Key_S))
    {
        rotationX -= ToRadians(120) * frameTime;
    }
    if (KeyHeld(Key_A))
    {
        rotationY += ToRadians(120) * frameTime;
    }
    if (KeyHeld(Key_D))
    {
        rotationY -= ToRadians(120) * frameTime;
    }
    gCubeMatrix = MatrixRotationX(rotationX) * MatrixRotationY(rotationY);


    // Show frame time / FPS in the window title //

    const float fpsUpdateTime = 0.5f; // How long between updates (in seconds)
    static float totalFrameTime = 0;
    static int frameCount = 0;
    totalFrameTime += frameTime;
    ++frameCount;
    if (totalFrameTime > fpsUpdateTime)
    {
        // Displays FPS rounded to nearest int, and frame time (more useful for developers) in milliseconds to 2 decimal places
        float avgFrameTime = totalFrameTime / frameCount;
        std::ostringstream frameTimeMs;
        frameTimeMs.precision(2);
        frameTimeMs << std::fixed << avgFrameTime * 1000;
        std::string windowTitle = "CO2409 Week 10: Pixel Shaders - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}

