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

// GPU-side geometry data //

// DirectX objects controlling the vertex & index buffers (mesh data on GPU) and vertex layout (description of a single vertex)
ID3D11InputLayout* gBasicVertexLayout = nullptr;
ID3D11Buffer*      gCubeVertexBuffer  = nullptr;
ID3D11Buffer*      gFloorVertexBuffer = nullptr;
ID3D11Buffer*      gCubeIndexBuffer   = nullptr;
ID3D11Buffer*      gFloorIndexBuffer  = nullptr;


// GPU-side texture data //

// DirectX objects controlling textures used in this lab
ID3D11Resource*           gCubeDiffuseMap     = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11ShaderResourceView* gCubeDiffuseMapSRV  = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
ID3D11Resource*           gFloorDiffuseMap    = nullptr;
ID3D11ShaderResourceView* gFloorDiffuseMapSRV = nullptr;

// A sampler object represents a way to filter textures, such as bilinear or trilinear. We have one object for each method we want to use
ID3D11SamplerState* gTextureSampler = nullptr;


// Model data //

// The world matrix for each model - these position and orient the models and are updated every frame for moving models.
CMatrix4x4 gCubeMatrix;
CMatrix4x4 gFloorMatrix;



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
    CMatrix4x4 viewProjectionMatrix; // The above two matrices multiplied together to combine their effects
} gPerFrameConstants;
ID3D11Buffer* gPerFrameConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure


// This is the matrix that positions the cube in the scene. Unlike the structure above this data can be updated and
// sent to the GPU several times every frame (once per cube). However, apart from that it works in the same way.
struct
{
    CMatrix4x4 worldMatrix;
} gPerModelConstants;
ID3D11Buffer* gPerModelConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure



//--------------------------------------------------------------------------------------
// Geometry definitions and data
//--------------------------------------------------------------------------------------

// The content of a single vertex in the geometry to render. Stores the vertex position as usual as well
// as a per-vertex colour and a texture coordinate (for textures).
struct BasicVertex
{
    CVector3 position; // Model space position of the vertex (x,y,z)
    CVector3 colour;   // Colour for this vertex (r,g,b)
    CVector2 uv;       // Texture coordinate (aka UVs) for the vertex (u,v)
};



// Describe the "input layout" //

// This describes the contents of the BasicVertex structure above so DirectX will know what to expect when reading vertex data.
// See earlier lab exercise comments for details. This structure should also match the vertex shader input structure
D3D11_INPUT_ELEMENT_DESC gBasicVertexDesc[] =
{
    // Data Type,  Type Index,  Data format                   Slot  Offset    Other values can be ignored for now 
    { "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,  0,    0,        D3D11_INPUT_PER_VERTEX_DATA,  0 }, // Position has 3 floats starting at offset 0
    { "Colour",    0,           DXGI_FORMAT_R32G32B32_FLOAT,  0,    12,       D3D11_INPUT_PER_VERTEX_DATA,  0 }, // Colour has 3 floats starting at offset 12 (= size of the earlier 3 floats)
    { "UV",        0,           DXGI_FORMAT_R32G32_FLOAT,     0,    24,       D3D11_INPUT_PER_VERTEX_DATA,  0 }, // UV has 3 floats starting at offset 24 (= size of the earlier 6 floats)
};
int gBasicVertexDescCount = sizeof(gBasicVertexDesc) / sizeof(gBasicVertexDesc[0]); // This gives a count of rows in the array above



// Geometry - the mesh to draw //

// A CPU-side array of vertices for a cube. Each vertex is the BasicVertex structure above.
BasicVertex gCubeVertices[] =
{
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{  1.0f, 1.0f, 1.0f }, CVector2{ 0.0f, 0.0f }, // Front face
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  1.0f, 1.0f, 1.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{  1.0f, 1.0f, 1.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  1.0f, 1.0f, 1.0f }, CVector2{ 1.0f, 1.0f },
												
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 0.0f, 0.0f }, // Right face
	CVector3{  1.0f, 1.0f, 1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f,-1.0f, 1.0f }, CVector3{  1.0f, 0.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
															
	CVector3{  1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 0.0f, 0.0f }, // Etc.
	CVector3{ -1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{  1.0f,-1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{ -1.0f,-1.0f, 1.0f }, CVector3{  0.0f, 0.0f, 1.0f }, CVector2{ 1.0f, 1.0f },
															
	CVector3{ -1.0f, 1.0f, 1.0f }, CVector3{ -1.0f, 1.0f, 0.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{ -1.0f, 1.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f,-1.0f, 1.0f }, CVector3{ -1.0f, 1.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{ -1.0f, 1.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
															
	CVector3{ -1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{  1.0f, 1.0f, 1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 1.0f, 1.0f },
															
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{  1.0f, 0.0f, 1.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  1.0f, 0.0f, 1.0f }, CVector2{ 1.0f, 0.0f },
	CVector3{ -1.0f,-1.0f, 1.0f }, CVector3{  1.0f, 0.0f, 1.0f }, CVector2{ 0.0f, 1.0f },
	CVector3{  1.0f,-1.0f, 1.0f }, CVector3{  1.0f, 0.0f, 1.0f }, CVector2{ 1.0f, 1.0f },
};
int gCubeNumVertices = sizeof(gCubeVertices) / sizeof(gCubeVertices[0]); // Total number of vertices in the array above


// An index buffer for the cube mesh above (shows how to join together the vertices into triangles). This is a triangle list
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



// A CPU-side array of vertices for thee floor. Each vertex is the BasicVertex structure above.
BasicVertex gFloorVertices[] =
{
	CVector3{ -100.0f, 0.0f, 100.0f }, CVector3{ 1.0f, 1.0f, 1.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{  100.0f, 0.0f, 100.0f }, CVector3{ 1.0f, 1.0f, 1.0f }, CVector2{ 5.0f, 0.0f },
	CVector3{ -100.0f, 0.0f,-100.0f }, CVector3{ 1.0f, 1.0f, 1.0f }, CVector2{ 0.0f, 5.0f },
	CVector3{  100.0f, 0.0f,-100.0f }, CVector3{ 1.0f, 1.0f, 1.0f }, CVector2{ 5.0f, 5.0f },
};
int gFloorNumVertices = sizeof(gFloorVertices) / sizeof(gFloorVertices[0]);


// An index buffer for the floor mesh above (shows how to join together the vertices into triangles). This is a triangle list
DWORD gFloorIndices[] =
{
    0,  1,  2,		 2,  1,  3
};
int gFloorNumIndices = sizeof(gFloorIndices) / sizeof(gFloorIndices[0]);



//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    //// Create vertex and index buffers on the GPU ////
    
    // Create GPU-side vertex buffers and copy our arrays of vertices typed in above into them
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    gCubeVertexBuffer  = CreateVertexBuffer(gCubeVertices,  gCubeNumVertices,  sizeof(gCubeVertices[0]));  // Pass pointer to vertex data, number of...
    gFloorVertexBuffer = CreateVertexBuffer(gFloorVertices, gFloorNumVertices, sizeof(gFloorVertices[0])); // ...vertices and size of a single vertex in bytes
    if (gCubeVertexBuffer == nullptr || gFloorVertexBuffer == nullptr)
    {
        gLastError = "Error creating vertex buffers";
        return false;
    }

    // Create GPU-side index buffers and copy our array of indices typed in above into them.
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    gCubeIndexBuffer  = CreateIndexBuffer(gCubeIndices,  gCubeNumIndices);  // Pass pointer to index data and number of indices
    gFloorIndexBuffer = CreateIndexBuffer(gFloorIndices, gFloorNumIndices);
    if (gCubeIndexBuffer == nullptr || gFloorIndexBuffer == nullptr)
    {
        gLastError = "Error creating index buffers";
        return false;
    }


    // These lines create a "vertex layout" to describe to DirectX what is in a BasicVertex (position, normals, colours etc.)
    // We type the description in the gBasicVertexDesc array above, and this code converts it to an object the GPU can use.
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    gBasicVertexLayout = CreateVertexLayout(gBasicVertexDesc, gBasicVertexDescCount);
    if (gBasicVertexLayout == nullptr)
    {
        gLastError = "Error creating vertex layout";
        return false;
    }



    //****

    //// Load / prepare textures on the GPU ////

    // Load textures and create DirectX objects for them
    // Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify file loading
    // This function requires you to pass a ID3D11Resource* (e.g. &gTilesDiffuseMap), which manages the GPU memory for the
    // texture and also a ID3D11ShaderResourceView* (e.g. &gTilesDiffuseMapSRV), which allows us to use the texture in shaders
    // The function will fill in these pointers with usable data. The variables are globals and can be found near the top of the file.
    if (FAILED(DirectX::CreateWICTextureFromFile(gD3DDevice, gD3DContext, L"tiles1.jpg", &gCubeDiffuseMap,  &gCubeDiffuseMapSRV )) ||
        FAILED(DirectX::CreateWICTextureFromFile(gD3DDevice, gD3DContext, L"lines.png",  &gFloorDiffuseMap, &gFloorDiffuseMapSRV)))
    {
        gLastError = "Error loading textures";
        return false;
    }


    //// Create a texture sampler to define how the shaders will filter the texture at different sizes ////

    // Most of your work for this lab will be here
    // First set up the description of the sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; // Filtering method - see lecture
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;   // Addressing mode for texture coordinates outside 0->1 - see lecture
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;   // --"--

    samplerDesc.MaxAnisotropy = 10;                        // Number of samples used for anisotropic filtering, more is better, but max value depends on GPU

    samplerDesc.MaxLOD = FLT_MAX; // Controls how much mip-mapping can be used
    samplerDesc.MinLOD = 0; // --"--

    // Then create a DirectX object for your description that can be used by a shader
    if (FAILED(gD3DDevice->CreateSamplerState(&samplerDesc, &gTextureSampler)))
    {
        gLastError = "Error creating sampler";
        return false;
    }


    //****


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
    if (gFloorDiffuseMapSRV)      gFloorDiffuseMapSRV->Release();
    if (gFloorDiffuseMap)         gFloorDiffuseMap->Release();
    if (gCubeDiffuseMapSRV)       gCubeDiffuseMapSRV->Release();
    if (gCubeDiffuseMap)          gCubeDiffuseMap->Release();
    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();
    if (gFloorIndexBuffer)        gFloorIndexBuffer->Release();
    if (gFloorVertexBuffer)       gFloorVertexBuffer->Release();
    if (gCubeIndexBuffer)         gCubeIndexBuffer->Release();
    if (gCubeVertexBuffer)        gCubeVertexBuffer->Release();
    if (gBasicVertexLayout)       gBasicVertexLayout->Release();
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


    // Send per-frame data (camera matrices) over to the shaders on the GPU
    // See the section commented as "Constant Buffers" near the top of the file for more info about the data being sent here
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 


    //-------------------------------------------------------------------------


    //// Prepare for cube rendering ////

    // Tell DirectX what geometry data will be used next: vertex buffer, index buffer, vertex layout, size of a single vertex
    // and topology (list or strip). Using a new helper function in Utility/SceneHelpers.cpp for this common code sequence
    SelectGeometry(gCubeVertexBuffer, gCubeIndexBuffer,
                   gBasicVertexLayout, sizeof(BasicVertex), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Select which shaders to use next
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);

    
    //// Render cube ////

    // Send the world matrix for the cube over to the shaders on the GPU (using a constant buffer)
    // See the section commented as "Constant Buffers" near the top of the file for more info about the data being sent here
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    gPerModelConstants.worldMatrix = gCubeMatrix; // Update C++ side constant buffer
    UpdateConstantBuffer(gPerModelConstantBuffer, gPerModelConstants); // Send to GPU

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader

    // Select the cube texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gCubeDiffuseMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gTextureSampler);
    
    // Draw the geometry using an index buffer
    gD3DContext->DrawIndexed(36, 0, 0); // Cube: 36 vertices starting at 0


    //-------------------------------------------------------------------------


    //// Prepare for floor rendering ////

    // Tell DirectX what geometry data will be used next: vertex buffer, index buffer, vertex layout, size of a single vertex
    // and topology (list or strip). Using a new helper function in Utility/SceneHelpers.cpp for this common code sequence
    SelectGeometry(gFloorVertexBuffer, gFloorIndexBuffer,
                   gBasicVertexLayout, sizeof(BasicVertex), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Select which shaders to use next
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);

    
    //// Render floor ////

    // Send the world matrix for the floor over to the shaders on the GPU (using a constant buffer)
    gPerModelConstants.worldMatrix = gFloorMatrix;
    UpdateConstantBuffer(gPerModelConstantBuffer, gPerModelConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader

    // Select the floor texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gFloorDiffuseMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gTextureSampler);

    // Draw the geometry using an index buffer
    gD3DContext->DrawIndexed(6, 0, 0); // Floor is square: 6 vertices starting at 0


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
    //// Update cube ////

    static CVector3 cubePosition = { 0, 2, 0 };

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
    gCubeMatrix = MatrixRotationX(rotationX) * MatrixRotationY(rotationY) * MatrixTranslation(cubePosition);



    //// Update floor ////

    gFloorMatrix = MatrixIdentity(); // Floor stays at the origin



    //// Update camera ////

    static CVector3 cameraPosition = {0, 3, -5};
    CMatrix4x4 cameraMatrix = MatrixTranslation(cameraPosition);
    cameraMatrix.FaceTarget(cubePosition); // Camera always faces cube

    // Create camera matrices
    gPerFrameConstants.viewMatrix = InverseAffine(cameraMatrix);
    gPerFrameConstants.projectionMatrix = MakeProjectionMatrix(); // Using a helper function to make this special matrix
    gPerFrameConstants.viewProjectionMatrix = gPerFrameConstants.viewMatrix * gPerFrameConstants.projectionMatrix;

    // Move camera forward and back
    if (KeyHeld(Key_Up))
    {
        cameraPosition.y -=  5.0f * frameTime;
        cameraPosition.z += 10.0f * frameTime;
    }
    if (KeyHeld(Key_Down))
    {
        cameraPosition.y +=  5.0f * frameTime;
        cameraPosition.z -= 10.0f * frameTime;
    }



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
        std::string windowTitle = "CO2409 Week 12: Textureing - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
