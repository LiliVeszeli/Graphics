//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "State.h"
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
// Global Scene Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

// Constants controlling speed of movement/rotation. [Note for Games Dev 1 students: measured in units per second because we're using frame time]
const float kRotationSpeed = 4.0f;  // 4 radians per second for rotation
const float kMovementSpeed = 10.0f; // 10 units per second for movement (what a unit of length is depends on 3D model - i.e. an artist decision usually)
const float kScaleSpeed    = 2.0f;  // 2x or 1/2x scaling each second - this is used as a multiplier/divider

// Store the cube's position, rotation and scale as D3DXVECTOR (x,y,z for each). These values are convenient to work with. The function UpdateCube will
// combine these together each frame into a world matrix for the cube - the same information as used in the rendering pipeline / vertex shader
CVector3    gCubePosition = { 0.0f, 3.0f, 0.0f };
CVector3    gCubeRotation = { 0.0f, 0.0f, 0.0f };
CVector3    gCubeScale    = { 1.0f, 1.0f, 1.0f }; // Scaling is a multiplier, so default is 1.0, not 0.0 or the cube would disappear!
CMatrix4x4  gCubeWorldMatrix;


// We're not going to move/rotate the floor, so just prepare a world matrix for rendering - it will never change
// However, the key point is that each model has its own world matrix
CMatrix4x4 gFloorWorldMatrix;


// Position and rotation for the camera. We will treat the camera like a model (just like the cube above) and create a "camera world matrix".
// But the rendering pipeline actually needs a view matrix, but that's just the inverse of the camera world matrix - easily made (look in UpdateCamera function)
// This approach is convenient as we can use almost the same code for the camera as for the cube
// Scaling is not especially useful for a camera
CVector3    gCameraPosition = { 0.0f, 5.0f, -8.0f };
CVector3    gCameraRotation = { 0.3f, 0.0f,  0.0f };
CMatrix4x4  gCameraWorldMatrix;
CMatrix4x4  gCameraViewMatrix;

// Camera also has a second matrix, the projection matrix, that is used to set the near/far clip distances and the field of view (camera settings)
CMatrix4x4 gCameraProjectionMatrix;


// Light information
CVector3 gAmbientColour = { 0.2f, 0.2f, 0.3f };     // Background level of light (slightly bluish to match the far background, which is dark blue)
CVector3 gLightColour   = { 1.0f, 0.8f, 0.4f };     // Light colour - warm yellow
CVector3 gLightPosition = { 30.0f, 50.0f, -70.0f }; // Place the light high up behind and to the right
float gSpecularPower = 64.0f;                       // Specular power controls shininess



//--------------------------------------------------------------------------------------
// GPU-side Geometry variables
//--------------------------------------------------------------------------------------

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


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
// Variables sent over to the GPU each frame

// Data that remains constant for an entire frame, updated from C++ to the GPU shaders *once per frame*
// We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU each frame when
// we have finished updating the scene. There is a structure in the shader code that exactly matches this one
struct
{
    // These are the matrices used to position the camera
    CMatrix4x4 viewMatrix;
    CMatrix4x4 projectionMatrix;
    CMatrix4x4 viewProjectionMatrix; // The above two matrices multiplied together to combine their effects

    CVector3   lightPosition; // 3 floats: x, y z
    float      padding1;      // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
                              // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
                              // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
    CVector3   lightColour;
    float      padding2;

    CVector3   ambientColour;
    float      specularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

    CVector3   cameraPosition;
    float      padding3;
} gPerFrameConstants;
ID3D11Buffer* gPerFrameConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure


// This is the matrix that positions the next thing to be rendered in the scene. Unlike the structure above this data can be
// updated and sent to the GPU several times every frame (once per model). However, apart from that it works in the same way.
struct
{
    CMatrix4x4 worldMatrix;
} gPerModelConstants;
ID3D11Buffer* gPerModelConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure



//--------------------------------------------------------------------------------------
// Geometry definitions and data
//--------------------------------------------------------------------------------------

// The content of a single vertex in the geometry to render. Stores the vertex position as usual as well a texture coordinate (for textures).
struct BasicVertex
{
    CVector3 position; // Model space position of the vertex (x,y,z)
    CVector3 normal;   // Model space vertex normal (for lighting) (x,y,z)
    CVector2 uv;       // Texture coordinate (aka UVs) for the vertex (u,v)
};



// Describe the "input layout" //

// This describes the contents of the BasicVertex structure above so DirectX will know what to expect when reading vertex data.
// See earlier lab exercise comments for details. This structure should also match the vertex shader input structure
D3D11_INPUT_ELEMENT_DESC gBasicVertexDesc[] =
{
    // Data Type,  Type Index,  Data format                   Slot  Offset    Other values can be ignored for now 
    { "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,  0,    0,        D3D11_INPUT_PER_VERTEX_DATA,  0 }, // Position has 3 floats starting at offset 0
    { "Normal",    0,           DXGI_FORMAT_R32G32B32_FLOAT,  0,    12,       D3D11_INPUT_PER_VERTEX_DATA,  0 }, // Normal has 3 floats starting at offset 12 (= size of the earlier 3 floats)
    { "UV",        0,           DXGI_FORMAT_R32G32_FLOAT,     0,    24,       D3D11_INPUT_PER_VERTEX_DATA,  0 }, // UV has 3 floats starting at offset 24 (= size of the earlier 6 floats)
};
int gBasicVertexDescCount = sizeof(gBasicVertexDesc) / sizeof(gBasicVertexDesc[0]); // This gives a count of rows in the array above



// Geometry - the mesh to draw //

// A CPU-side array of vertices for a cube. Each vertex is the BasicVertex structure above.
BasicVertex gCubeVertices[] =
{
	CVector3{ -1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 0.0f, 1.0f }, // Front face
	CVector3{  1.0f, 1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 0.0f, 0.0f },
	CVector3{ -1.0f,-1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 1.0f, 1.0f },
	CVector3{  1.0f,-1.0f,-1.0f }, CVector3{  0.0f, 0.0f,-1.0f }, CVector2{ 1.0f, 0.0f },
									 
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
	CVector3{ -100.0f, 0.0f, 100.0f },  CVector3{  0.0f, 1.0f, 0.0f }, CVector2{  0.0f,  0.0f },
	CVector3{  100.0f, 0.0f, 100.0f },  CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 20.0f,  0.0f },
	CVector3{ -100.0f, 0.0f,-100.0f },  CVector3{  0.0f, 1.0f, 0.0f }, CVector2{  0.0f, 20.0f },
	CVector3{  100.0f, 0.0f,-100.0f },  CVector3{  0.0f, 1.0f, 0.0f }, CVector2{ 20.0f, 20.0f },
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
    // Load the shaders required for the geometry we will use
    if (!LoadShaders())
    {
        gLastError = "Error loading shaders";
        return false;
    }



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


    //// Load / prepare textures on the GPU ////

    // Load textures and create DirectX objects for them
    // Using Microsoft's open source DirectX Tool Kit (DirectXTK) to simplify file loading
    // This function requires you to pass a ID3D11Resource* (e.g. &gTilesDiffuseMap), which manages the GPU memory for the
    // texture and also a ID3D11ShaderResourceView* (e.g. &gTilesDiffuseMapSRV), which allows us to use the texture in shaders
    // The function will fill in these pointers with usable data. The variables are globals and can be found near the top of the file.
    if (FAILED(DirectX::CreateWICTextureFromFile(gD3DDevice, gD3DContext, L"brick1.jpg", &gCubeDiffuseMap,  &gCubeDiffuseMapSRV )) ||
        FAILED(DirectX::CreateWICTextureFromFile(gD3DDevice, gD3DContext, L"tech02.jpg", &gFloorDiffuseMap, &gFloorDiffuseMapSRV)))
    {
        gLastError = "Error loading textures";
        return false;
    }


	// Create all filtering modes, blending modes etc. used by the app (see State.cpp/.h)
	if (!CreateStates())
	{
		gLastError = "Error creating states";
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
    ReleaseStates();

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    if (gFloorDiffuseMapSRV)  gFloorDiffuseMapSRV->Release();
    if (gFloorDiffuseMap)     gFloorDiffuseMap->Release();
    if (gCubeDiffuseMapSRV)   gCubeDiffuseMapSRV->Release();
    if (gCubeDiffuseMap)      gCubeDiffuseMap->Release();

    if (gFloorIndexBuffer)   gFloorIndexBuffer->Release();
    if (gFloorVertexBuffer)  gFloorVertexBuffer->Release();
                             
    if (gCubeIndexBuffer)    gCubeIndexBuffer->Release();
    if (gCubeVertexBuffer)   gCubeVertexBuffer->Release();
    if (gBasicVertexLayout)  gBasicVertexLayout->Release();
}


//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

void RenderModel()
{

}

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


    // Send per-frame data (camera matrices and lighting info) over to the shaders on the GPU
    // See the section commented as "Constant Buffers" near the top of the file for more info about the data being sent here
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    gPerFrameConstants.viewMatrix           = gCameraViewMatrix;
    gPerFrameConstants.projectionMatrix     = gCameraProjectionMatrix;
    gPerFrameConstants.viewProjectionMatrix = gPerFrameConstants.viewMatrix * gPerFrameConstants.projectionMatrix;
	gPerFrameConstants.ambientColour = gAmbientColour;
	gPerFrameConstants.lightColour = gLightColour;
    gPerFrameConstants.lightPosition  = gLightPosition;
    gPerFrameConstants.specularPower  = gSpecularPower;
    gPerFrameConstants.cameraPosition = gCameraWorldMatrix.GetPosition();
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 


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
    gPerModelConstants.worldMatrix = gFloorWorldMatrix;
    UpdateConstantBuffer(gPerModelConstantBuffer, gPerModelConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader

    // Select the floor texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gFloorDiffuseMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gTrilinearSampler);

    // Floor states - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullBackState);

    // Draw the geometry using an index buffer
    gD3DContext->DrawIndexed(6, 0, 0); // Floor is square: 6 vertices starting at 0


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
    gPerModelConstants.worldMatrix = gCubeWorldMatrix; // Update C++ side constant buffer
    UpdateConstantBuffer(gPerModelConstantBuffer, gPerModelConstants); // Send to GPU

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader

    // Select the cube texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gCubeDiffuseMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gTrilinearSampler);

    // Cube states - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Draw the geometry using an index buffer
    gD3DContext->DrawIndexed(36, 0, 0); // Cube: 36 vertices starting at 0


    //-------------------------------------------------------------------------


    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// Update the position, rotation and scaling of the cube and use these to create its world matrix. Done every frame, before rendering, allowing us to control the cube.
// The float parameter is the time passed since the last frame. This is used to decide how much movement/rotation/scaling to perform
void UpdateCube(float frameTime)
{
	//////////////////////////
	// Update global variables (look at top of file) that control the model matrices

	//**** ROTATION ****
	if (KeyHeld(Key_K))
	{
		gCubeRotation.x += kRotationSpeed * frameTime; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(Key_I))
	{
		gCubeRotation.x -= kRotationSpeed * frameTime;
	}
	if (KeyHeld(Key_L))
	{
		gCubeRotation.y += kRotationSpeed * frameTime;
	}
	if (KeyHeld(Key_J))
	{
		gCubeRotation.y -= kRotationSpeed * frameTime;
	}
	if (KeyHeld(Key_U))
	{
		gCubeRotation.z += kRotationSpeed * frameTime;
	}
	if (KeyHeld(Key_O))
	{
		gCubeRotation.z -= kRotationSpeed * frameTime;
	}

	//**** SCALING ****
	if (KeyHeld(Key_X)) // Use X as a modifier to apply scaling
	{
		if (KeyHeld(Key_H))
		{
			gCubeScale.x *= powf( kScaleSpeed, frameTime ); // Different way of using frame time if using a multiplie/divider [a point for Games Dev 1 students]
		}
		if (KeyHeld(Key_F))
		{
			gCubeScale.x /= powf( kScaleSpeed, frameTime );
		}
		if (KeyHeld(Key_T))
		{
			gCubeScale.y *= powf( kScaleSpeed, frameTime );
		}
		if (KeyHeld(Key_G))
		{
			gCubeScale.y /= powf( kScaleSpeed, frameTime );
		}
		if (KeyHeld(Key_R))
		{
			gCubeScale.z *= powf( kScaleSpeed, frameTime );
		}
		if (KeyHeld(Key_Y))
		{
			gCubeScale.z /= powf( kScaleSpeed, frameTime );
		}
	}

	//**** WORLD MOVEMENT ****
	else if (KeyHeld(Key_Z)) // Use Z key as a modifier to select world movement
	{
		if (KeyHeld(Key_H))
		{
			gCubePosition.x += kMovementSpeed * frameTime; // Compare how the x,y and z components map onto the keys differently for movement and rotation
		}
		if (KeyHeld(Key_F))
		{
			gCubePosition.x -= kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_R))
		{
			gCubePosition.y += kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_Y))
		{
			gCubePosition.y -= kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_T))
		{
			gCubePosition.z += kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_G))
		{
			gCubePosition.z -= kMovementSpeed * frameTime;
		}
	}

	//**** LOCAL MOVEMENT ****
	else // Do local movement if not pressing Z or X
	{
		if (KeyHeld(Key_H))
		{
			gCubePosition.x += kMovementSpeed * frameTime * gCubeWorldMatrix.e00; // For local movement we extract the local axes from the
			gCubePosition.y += kMovementSpeed * frameTime * gCubeWorldMatrix.e01; // world matrix. The first row of the world matrix is the
			gCubePosition.z += kMovementSpeed * frameTime * gCubeWorldMatrix.e02; // local X direction. So just add that vector to the position.
		}
		if (KeyHeld(Key_F))
		{
			gCubePosition.x -= kMovementSpeed * frameTime * gCubeWorldMatrix.e00;
			gCubePosition.y -= kMovementSpeed * frameTime * gCubeWorldMatrix.e01;
			gCubePosition.z -= kMovementSpeed * frameTime * gCubeWorldMatrix.e02;
		}
		if (KeyHeld(Key_R))
		{
			gCubePosition.x += kMovementSpeed * frameTime * gCubeWorldMatrix.e10; // Use second row from world matrix = local y direction
			gCubePosition.y += kMovementSpeed * frameTime * gCubeWorldMatrix.e11;
			gCubePosition.z += kMovementSpeed * frameTime * gCubeWorldMatrix.e12;
		}
		if (KeyHeld(Key_Y))
		{
			gCubePosition.x -= kMovementSpeed * frameTime * gCubeWorldMatrix.e10;
			gCubePosition.y -= kMovementSpeed * frameTime * gCubeWorldMatrix.e11;
			gCubePosition.z -= kMovementSpeed * frameTime * gCubeWorldMatrix.e12;
		}
		if (KeyHeld(Key_T))
		{
			gCubePosition.x += kMovementSpeed * frameTime * gCubeWorldMatrix.e20; // Use second row from world matrix = local y direction
			gCubePosition.y += kMovementSpeed * frameTime * gCubeWorldMatrix.e21;
			gCubePosition.z += kMovementSpeed * frameTime * gCubeWorldMatrix.e22;
		}
		if (KeyHeld(Key_G))
		{
			gCubePosition.x -= kMovementSpeed * frameTime * gCubeWorldMatrix.e20;
			gCubePosition.y -= kMovementSpeed * frameTime * gCubeWorldMatrix.e21;
			gCubePosition.z -= kMovementSpeed * frameTime * gCubeWorldMatrix.e22;
		}
	}

	// Create a matrix for each transform and multiply them all to get the effect of them all combined into a single matrix, the "world matrix"
  // **Order of multiplication is important, get slightly different control mechanism depending on order**
  gCubeWorldMatrix = MatrixScaling(gCubeScale) * MatrixRotationZ(gCubeRotation.z) * MatrixRotationX(gCubeRotation.x) * MatrixRotationY(gCubeRotation.y) *
                      MatrixTranslation(gCubePosition);
}



// Update the camera's position and rotation. We treat the camera like a model, and create a world matrix for it just like the function above.
// We then convert that into the view matrix that the rendering pipeline needs. Also create the projection matrix, a second matrix that only cameras need
void UpdateCamera(float frameTime)
{
	//////////////////////////
	// Update global variables (look at top of file) that control the camera matrices

	//**** ROTATION ****
	if (KeyHeld(Key_Down))
	{
		gCameraRotation.x += kRotationSpeed * frameTime; // Use of frameTime to ensure same speed on different machines
	}
	if (KeyHeld(Key_Up))
	{
		gCameraRotation.x -= kRotationSpeed * frameTime;
	}
	if (KeyHeld(Key_Right))
	{
		gCameraRotation.y += kRotationSpeed * frameTime;
	}
	if (KeyHeld(Key_Left))
	{
		gCameraRotation.y -= kRotationSpeed * frameTime;
	}

	//**** WORLD MOVEMENT ****
	if (KeyHeld(Key_Z)) // Use Z key as a modifier to select world movement
	{
		if (KeyHeld(Key_D))
		{
			gCameraPosition.x += kMovementSpeed * frameTime; // Compare how the x,y and z components map onto the keys differently for movement and rotation
		}
		if (KeyHeld(Key_A))
		{
			gCameraPosition.x -= kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_Q))
		{
			gCameraPosition.y += kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_E))
		{
			gCameraPosition.y -= kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_W))
		{
			gCameraPosition.z += kMovementSpeed * frameTime;
		}
		if (KeyHeld(Key_S))
		{
			gCameraPosition.z -= kMovementSpeed * frameTime;
		}
	}

	//**** LOCAL MOVEMENT ****
	else // Do local movement if not pressing Z
	{
		if (KeyHeld(Key_D))
		{
			gCameraPosition.x += kMovementSpeed * frameTime * gCameraWorldMatrix.e00; // See comments on local movement in UpdateCube code above
			gCameraPosition.y += kMovementSpeed * frameTime * gCameraWorldMatrix.e01; 
			gCameraPosition.z += kMovementSpeed * frameTime * gCameraWorldMatrix.e02; 
		}
		if (KeyHeld(Key_A))
		{
			gCameraPosition.x -= kMovementSpeed * frameTime * gCameraWorldMatrix.e00;
			gCameraPosition.y -= kMovementSpeed * frameTime * gCameraWorldMatrix.e01;
			gCameraPosition.z -= kMovementSpeed * frameTime * gCameraWorldMatrix.e02;
		}
		if (KeyHeld(Key_Q))
		{
			gCameraPosition.x += kMovementSpeed * frameTime * gCameraWorldMatrix.e10;
			gCameraPosition.y += kMovementSpeed * frameTime * gCameraWorldMatrix.e11;
			gCameraPosition.z += kMovementSpeed * frameTime * gCameraWorldMatrix.e12;
		}
		if (KeyHeld(Key_E))
		{
			gCameraPosition.x -= kMovementSpeed * frameTime * gCameraWorldMatrix.e10;
			gCameraPosition.y -= kMovementSpeed * frameTime * gCameraWorldMatrix.e11;
			gCameraPosition.z -= kMovementSpeed * frameTime * gCameraWorldMatrix.e12;
		}
		if (KeyHeld(Key_W))
		{
			gCameraPosition.x += kMovementSpeed * frameTime * gCameraWorldMatrix.e20;
			gCameraPosition.y += kMovementSpeed * frameTime * gCameraWorldMatrix.e21;
			gCameraPosition.z += kMovementSpeed * frameTime * gCameraWorldMatrix.e22;
		}
		if (KeyHeld(Key_S))
		{
			gCameraPosition.x -= kMovementSpeed * frameTime * gCameraWorldMatrix.e20;
			gCameraPosition.y -= kMovementSpeed * frameTime * gCameraWorldMatrix.e21;
			gCameraPosition.z -= kMovementSpeed * frameTime * gCameraWorldMatrix.e22;
		}
	}

	// Make matrices for position and rotations, then multiply together to get a "camera world matrix"
    gCameraWorldMatrix = MatrixRotationZ(gCameraRotation.z) * MatrixRotationX(gCameraRotation.x) * MatrixRotationY(gCameraRotation.y) * MatrixTranslation(gCameraPosition);

    // The rendering pipeline actually needs the inverse of the camera world matrix - called the view matrix. Using a maths helper to create the inverse
    gCameraViewMatrix = InverseAffine(gCameraWorldMatrix);

    // Initialize the projection matrix. This determines viewing properties of the camera such as field of view (FOV) and near
    // clip distance (see lecture notes). Note that the FOV angle is measured in radians. Recall that radians = degrees * PI/180
    // Using a helper function to make this special matrix, using default values.
    gCameraProjectionMatrix = MakeProjectionMatrix(); 
}



// frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
    // Update camera matrices (view matrix, projection matrix) each frame
    UpdateCamera( frameTime );

    // Update the cube's matrices
    UpdateCube( frameTime );

    // The floor doesn't need an update function because it doesn't move. So just set its world matrix to the identity, which will place it at the origin
    gFloorWorldMatrix = MatrixIdentity();


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
        std::string windowTitle = "CO2409 Week 14: Review - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
