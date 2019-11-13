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

#include "ColourRGBA.h" 

#include <sstream>

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

// DirectX objects controlling the vertex buffer (mesh data on GPU) and vertex layout (description of a single vertex)
ID3D11InputLayout* gSimpleVertexLayout = nullptr;
ID3D11Buffer*      gSimpleVertexBuffer = nullptr;


// These are the matrices used to position the model & camera. They updated from C++ to the GPU shaders every frame.
// We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU each frame when
// we have finished updating the scene. There is a structure in the vertex shader that exactly matches this one
struct
{
    CMatrix4x4 worldMatrix;
    CMatrix4x4 viewMatrix;
    CMatrix4x4 projectionMatrix;
} gSceneConstants;
ID3D11Buffer* gSceneConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure

ID3D11RasterizerState* gTwoSided; // This is used to make sure both sides of a triangle are drawn - useful for early tutorials


//--------------------------------------------------------------------------------------
// Geometry definitions and data
//--------------------------------------------------------------------------------------

// The content of a single vertex in the geometry to render. Currently just stores the position of the vertex (x,y,z). 
// However, we can store other information for each vertex (for example, a colour for the vertex) so we use a structure.
struct SimpleVertex
{
    CVector3 position; 
	ColourRGBA colour;
};


//****

// Describe the "input layout" //

// This describes the contents of the SimpleVertex structure above so DirectX will know what to expect when reading vertex data.
//
// There is one row in the layout array below for each data element in our SimpleVertex structure. So the first row below describes
// the fact that each vertex has a "Position" which is made of three 32-bit floats, that is offset 0 bytes from the start of the
// vertex structure. Note that in this array the floats are referred to as RGB colours even though they will hold x,y,z values.
// The "slot", and other values in the array below can be ignored in most cases.
//
// This should also match the vertex shader input structure (see Shaders\transform3Dto2D_vs.hlsl). So, in total there are three
// structures maintained to keep C++ code and shader working correctly together. Watch out for this detail when working on advanced
// code in the future.
//
D3D11_INPUT_ELEMENT_DESC gSimpleVertexDesc[] =
{
    // Data Type,  Type Index,  Data format                      Slot  Offset    Other values can be ignored for now 
    { "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    0,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "Colour",    0,           DXGI_FORMAT_R32G32B32A32_FLOAT,  0,    12,       D3D11_INPUT_PER_VERTEX_DATA,  0 },
};
int gSimpleVertexDescCount = sizeof(gSimpleVertexDesc) / sizeof(gSimpleVertexDesc[0]); // This gives a count of rows in the array above


//****

// Geometry - the mesh to draw //

// A CPU-side array of vertices for the geometry we wish to render. Each vertex is the SimpleVertex structure above.
// Each triplet of vertices defines a single triangle in 3D. This is the 12 triangles of a cube.
SimpleVertex gCubeVertices[] =
{
	CVector3{ -1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 1.0, 1.0, 1.0, 1.0 },
	CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 1.0, 1.0, 1.0, 1.0 },
	

	CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.2f, 0.0, 0.1f, 1.0 },
	CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	

	CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ 1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.2f, 0.0, 0.1f, 1.0 },
	CVector3{ 1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	

	CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{  1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.2f, 0.0, 1.0, 1.0 },
	CVector3{  1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	

	CVector3{  1.0f, -1.0f,  1.0f },ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ 1.0f,  1.0f,  1.0f },ColourRGBA{ 0.2f, 0.0, 1.0, 1.0 },
	CVector3{ -1.0f, -1.0f,  1.0f },ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	

	CVector3{  1.0f,  1.0f,  1.0f }, ColourRGBA{ 1.0, 1.0, 1.0, 1.0 },
	CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.2f, 0.0, 1.0, 1.0 },


	CVector3{ -1.0f, -1.0f,  1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ -1.0f,  1.0f,  1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ -1.0f, -1.0f, -1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	

	CVector3{ -1.0f,  1.0f,  1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ -1.0f, -1.0f, -1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ -1.0f,  1.0f, -1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },


	CVector3{ -1.0f, -1.0f, -1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ 1.0f, -1.0f, -1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	CVector3{ -1.0f, -1.0f,  1.0f },ColourRGBA{ 0.0, 0.0, 0.0, 1.0 },
	

	CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ 1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	

	CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ 1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	

	CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },
	CVector3{ 1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5, 0.0, 1.0, 1.0 },

};
int gCubeNumVertices = sizeof(gCubeVertices) / sizeof(gCubeVertices[0]); // Total number of vertices in the array above

//****


//--------------------------------------------------------------------------------------
// Initialise Scene Geometry for DirectX
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    HRESULT hr; // To hold DirectX return values
    
    
    // Create a "vertex buffer" on the GPU-side, which is just a copy the vertex array above. When rendering, data needs to be in GPU memory.
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage     = D3D11_USAGE_DEFAULT;      // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = gCubeNumVertices * sizeof(SimpleVertex); // Size of the buffer in bytes
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


    return true;
}


// Prepare the scene
// Returns true on success
bool InitScene()
 {
    HRESULT hr; // To hold DirectX return values

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


    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    if (gTwoSided)             gTwoSided->Release();
    if (gSceneConstantBuffer)  gSceneConstantBuffer->Release();
    if (gSimpleVertexBuffer)   gSimpleVertexBuffer->Release();
    if (gSimpleVertexLayout)   gSimpleVertexLayout->Release();
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
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);

    // Clear the back buffer to a fixed colour
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, ClearColor);
	gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

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
    gD3DContext->IASetVertexBuffers(0, 1, &gSimpleVertexBuffer, &stride, &offset);


    // 2a) Indicate the layout of the vertex buffer
    gD3DContext->IASetInputLayout(gSimpleVertexLayout);
    
    // 2b) Also indicate the primitive topology of the buffer. Our buffer holds a triangle list - each set of 3 vertices
    //     will be connected into a triangle. There are other topologies and we will see them shortly.
    gD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // 3) Select which shaders to use when rendering
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);


    // 4) Draw 3 vertices, starting at vertex 0. This will draw a triangle using the vertex data and shaders selected
    gD3DContext->Draw(36, 0);



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
    // Prepare matrices to position camera and model //

    // Create a matrix to position the camera - called the view (camera) matrix - we'll see this in more detail later
    gSceneConstants.viewMatrix = InverseAffine(MatrixTranslation(CVector3(0, 0, -5.0f)));

    // Create a "projection matrix" - this determines properties of the camera - again we'll see this later
    gSceneConstants.projectionMatrix = MakeProjectionMatrix(); // Using a helper function to make this special matrix

    // Create a matrix to position the triangle - here it gets a Y-rotation matrix with the angle = time, so
    // it will rotate around the Y axis
    static float yRotation = 0;
    gSceneConstants.worldMatrix = MatrixRotationX(yRotation) * MatrixRotationY(yRotation); //matrix multiplication so that it rotates around both axes
    yRotation += frameTime;




    // Send per-frame data (camera / model matrices here) over to the shaders on the GPU

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
        std::string windowTitle = "CO2409 Week 7: The Cube - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}

