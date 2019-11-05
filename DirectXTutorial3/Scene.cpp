//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "Shader.h"
#include "Common.h"

// Maths classes you have seen in Games Dev 1
#include "CVector3.h" 
#include "CMatrix4x4.h"
#include "MathHelpers.h" // Some additional helper functions for maths - have a look


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11Buffer*      gVertexBuffer = nullptr;


//****

// These are the matrices used to position the model & camera. They updated from C++ to the GPU shaders every
// frame. We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU 
// each frame when we have finished updating the scene. 
// Read the vertex shader (transform3Dto2D_vs.hlsl) for more info - it has a structure that exactly matches this one
struct
{
    CMatrix4x4 worldMatrix;
    CMatrix4x4 viewMatrix;
    CMatrix4x4 projectionMatrix;
} gSceneConstants;
ID3D11Buffer* gSceneConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure

ID3D11RasterizerState* gTwoSided; // This is used to make sure both sides of a triangle are drawn - useful for early tutorials

//****


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

// The content of a single vertex in the geometry that we will render. Currently it just stores
// the position of the vertex. Later we will store additional data here.
struct SimpleVertex
{
    CVector3 Position; 
};



//--------------------------------------------------------------------------------------
// Scene Geometry & Layout
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    HRESULT hr; // To hold DirectX return values
    
    
    // Define the input layout
    // This define the contents of the SimpleVertex structure so DirectX will knows what to expect when reading vertex data.
    D3D11_INPUT_ELEMENT_DESC vertexLayout[] =
    {
        // Data Type,  Type Index,  Data format                  Other values can be ignored for now 
        { "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
        // Vertex contains position, which has three floats. Note that the floats are referred to as RGB colours even though they will hold x,y,z
    };
    UINT numElements = sizeof(vertexLayout) / sizeof(vertexLayout[0]); // This gives a count of rows in the array above

    // These lines convert the array above into an object (gVertexLayout) used when rendering
    auto shaderSignature = CreateSignatureForVertexLayout(vertexLayout, numElements);
    hr = gD3DDevice->CreateInputLayout(vertexLayout, numElements, shaderSignature->GetBufferPointer(), shaderSignature->GetBufferSize(), &gVertexLayout);
    if (shaderSignature)  shaderSignature->Release();
    if (FAILED(hr))
    {
        gLastError = "Error creating input layout";
        return false;
    }



    // Create an CPU-side array of vertices for our geometry using the SimpleVertex structure defined above.
    // This is going to be used as a triangle list - each set of three vertices will be joined up into a triangle
    // In this tutorial the x & y coordinates are from -1 to 1, and the z coordinate should always be 0.0f
    SimpleVertex vertices[] =
    {
        CVector3{  0.0f,  0.5f, 0.0f },
        CVector3{  0.5f, -0.5f, 0.0f },
        CVector3{ -0.5f, -0.5f, 0.0f },
    };
    int numVertices = sizeof(vertices) / sizeof(SimpleVertex); // A count of the number of vertices in the array above

    // Create a "vertex buffer" on the GPU-side, which is just a copy the vertex array above. When rendering data needs to be in GPU memory.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage     = D3D11_USAGE_DEFAULT;      // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = numVertices * sizeof(SimpleVertex); // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData; // Fill the new vertex buffer with the array above as initial data
    initData.pSysMem = vertices;
    hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &gVertexBuffer);
    if (FAILED(hr))
    {
        gLastError = "Error creating vertex buffer";
        return false;
    }

    return true;
}


// Prepare the scene
// Returns true on success
bool InitScene()
 {
    HRESULT hr; // To hold DirectX return values

    //****

    // Create a GPU-side constant buffer to match the gSceneConstants structure above
    // See the comments above where gSceneConstants is declared and also the UpdateScene function
    gSceneConstantBuffer = CreateConstantBuffer(sizeof(gSceneConstants));
    if (gSceneConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffer";
        return false;
    }


    // By default only one side of a triangle is visible, this code creates a "state" for the GPU
    // that will show both sides - which is convenient in these early labs
    // We'll see states in more detail later
    D3D11_RASTERIZER_DESC rasteriserState = {};
    rasteriserState.FillMode = D3D11_FILL_SOLID;
    rasteriserState.CullMode = D3D11_CULL_NONE;
    rasteriserState.DepthClipEnable = true;
    hr = gD3DDevice->CreateRasterizerState(&rasteriserState, &gTwoSided);
    if (FAILED(hr))
    {
        gLastError = "Error creating two-sided state";
        return false;
    }
    // Immediately enable this"two-sided" state
    gD3DContext->RSSetState(gTwoSided);

    //****

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    if (gTwoSided)             gTwoSided->Release();
    if (gSceneConstantBuffer)  gSceneConstantBuffer->Release();
    if (gVertexBuffer)         gVertexBuffer->Release();
    if (gVertexLayout)         gVertexLayout->Release();
}


//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Called once a frame, from the loop in Main.cpp
void RenderScene()
{
    // Per-frame set-up //

    // Set the "back buffer" as the target for rendering. The "back buffer" is an off-screen viewport. When
    // we have finished drawing to the back buffer it sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, nullptr);

    // Clear the back buffer to a fixed colour
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, ClearColor);

    // Setup the viewport - defines which part of the window we will render to, almost always the whole window
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(gViewportWidth);
    vp.Height = static_cast<FLOAT>(gViewportHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);



    // Scene rendering //

    // 1) Select the vertex buffer we created above - D3D will now use that data for rendering
    UINT stride = sizeof(SimpleVertex); // Size of a single vertex in the buffer
    UINT offset = 0;
    gD3DContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &stride, &offset);


    // 2a) Indicate the layout of the vertex buffer
    gD3DContext->IASetInputLayout(gVertexLayout);
    
    // 2b) Also indicate the primitive topology of the buffer. Our buffer holds a triangle list - each set of 3 vertices
    //     will be connected into a triangle. There are other topologies and we will see them shortly.
    gD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // 3) Select which shaders to use when rendering
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);


    // 4) Draw 3 vertices, starting at vertex 0. This will draw a triangle using the vertex data and shaders selected
    gD3DContext->Draw(3, 0);



    // Scene completion //

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
    //****

    // Create a matrix to position the camera - called the view (camera) matrix - we'll see this in more detail later
    gSceneConstants.viewMatrix = InverseAffine(MatrixTranslation(CVector3(0, 0, -1.5f)));

    // Create a "projection matrix" - this determines properties of the camera - again we'll see this later
    gSceneConstants.projectionMatrix = MakeProjectionMatrix(); // Using a helper function to make this special matrix

    // Create a matrix to position the triangle - here it gets a Y-rotation matrix with the angle = time, so
    // it will rotate around the Y axis
    static float yRotation = 0;
    gSceneConstants.worldMatrix = MatrixRotationY(yRotation);
    yRotation += frameTime;


    // Pass the matrices we've just prepared over to the shaders (on the GPU) so they can be used in the rendering
    // They get sent to a "constant buffer" held on the GPU. Communicating from CPU to GPU takes a few steps:
    // - "Map" basically opens the GPU's constant buffer for writing
    // - "memcpy" copies the C++ data over to the GPU's constant buffer
    // - "Unmap" closes the GPU's buffer again - we must do this as soon as possible
    D3D11_MAPPED_SUBRESOURCE cb;
    gD3DContext->Map(gSceneConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
    memcpy(cb.pData, &gSceneConstants, sizeof(gSceneConstants));
    gD3DContext->Unmap(gSceneConstantBuffer, 0);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    // If you look at the vertex shader code, there is a structure with the same three matrices that receives the data
    gD3DContext->VSSetConstantBuffers(0, 1, &gSceneConstantBuffer);

    //****
}

