//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "Shader.h"
#include "Input.h"
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

ID3D11RasterizerState* gTwoSided; // This is used to make sure both sides of a triangle are drawn - useful for early tutorials

// The world matrix for the cube - this positions and orients the cube and is updated every frame
CMatrix4x4 gCubeMatrix;
CMatrix4x4 gCubeMatrix2;


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
} gPerModelConstants;
ID3D11Buffer* gPerModelConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure



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
    // Data Type,  Type Index,  Data format                      Slot  Offset    Other values can be ignored for now 
    { "Position",  0,           DXGI_FORMAT_R32G32B32_FLOAT,     0,    0,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "Colour",    0,           DXGI_FORMAT_R32G32B32A32_FLOAT,  0,    12,       D3D11_INPUT_PER_VERTEX_DATA,  0 },
};
int gSimpleVertexDescCount = sizeof(gSimpleVertexDesc) / sizeof(gSimpleVertexDesc[0]); // This gives a count of rows in the array above


// Geometry - the mesh to draw //

// A CPU-side array of vertices for the geometry we wish to render. Each vertex is the SimpleVertex structure above.
// Each triplet of vertices defines a single triangle in 3D. This is the 12 triangles of a cube.
SimpleVertex gCubeVertices[] =
{
    CVector3{ -1.0f, -1.0f, -1.0f }, ColourRGBA{ 1.0f, 0.3f, 0.3f, 0.0f },
    CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 1.0f, 0.5f, 0.5f, 0.0f },
    CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 1.0f, 0.6f, 0.6f, 0.0f },

    CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 1.0f, 0.5f, 0.5f, 0.0f },
    CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 1.0f, 0.6f, 0.6f, 0.0f },
    CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 1.0f, 0.8f, 0.8f, 0.0f },

    CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.3f, 1.0f, 0.3f, 0.0f },
    CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5f, 1.0f, 0.5f, 0.0f },
    CVector3{  1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.6f, 1.0f, 0.6f, 0.0f },

    CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5f, 1.0f, 0.5f, 0.0f },
    CVector3{  1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.6f, 1.0f, 0.6f, 0.0f },
    CVector3{  1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.8f, 1.0f, 0.8f, 0.0f },

    CVector3{  1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.3f, 0.3f, 1.0f, 0.0f },
    CVector3{  1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5f, 0.5f, 1.0f, 0.0f },
    CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.6f, 0.6f, 1.0f, 0.0f },

    CVector3{  1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5f, 0.5f, 1.0f, 0.0f },
    CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.6f, 0.6f, 1.0f, 0.0f },
    CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.8f, 0.8f, 1.0f, 0.0f },

    CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.5f, 0.3f, 0.3f, 0.0f },
    CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5f, 0.5f, 0.5f, 0.0f },
    CVector3{ -1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.6f, 0.6f, 0.0f },

    CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.5f, 0.5f, 0.5f, 0.0f },
    CVector3{ -1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.6f, 0.6f, 0.0f },
    CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.8f, 0.8f, 0.0f },

    CVector3{ -1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.3f, 0.5f, 0.3f, 0.0f },
    CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.5f, 0.5f, 0.0f },
    CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.6f, 0.5f, 0.6f, 0.0f },

    CVector3{  1.0f, -1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.5f, 0.5f, 0.0f },
    CVector3{ -1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.6f, 0.5f, 0.6f, 0.0f },
    CVector3{  1.0f, -1.0f,  1.0f }, ColourRGBA{ 0.8f, 0.5f, 0.8f, 0.0f },

    CVector3{ -1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.3f, 0.3f, 0.5f, 0.0f },
    CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.5f, 0.5f, 0.0f },
    CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.6f, 0.6f, 0.5f, 0.0f },

    CVector3{  1.0f,  1.0f, -1.0f }, ColourRGBA{ 0.5f, 0.5f, 0.5f, 0.0f },
    CVector3{ -1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.6f, 0.6f, 0.5f, 0.0f },
    CVector3{  1.0f,  1.0f,  1.0f }, ColourRGBA{ 0.8f, 0.8f, 0.5f, 0.0f },
};
int gCubeNumVertices = sizeof(gCubeVertices) / sizeof(gCubeVertices[0]); // Total number of vertices in the array above



//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
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

    // Create GPU-side constant buffers to match the gPerFrameConstants and gPerModelConstants structures above
    // See the comments above where these variable are declared and also the UpdateScene function
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
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
    if (gTwoSided)                gTwoSided->Release();
    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();
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
    UINT stride = sizeof(SimpleVertex); // Size of a single vertex in the buffer
    UINT offset = 0;
    gD3DContext->IASetVertexBuffers(0, 1, &gSimpleVertexBuffer, &stride, &offset);

    // Indicate the layout of our vertex buffer. Only needs to be done once unless we want to use a different buffer
    gD3DContext->IASetInputLayout(gSimpleVertexLayout);
    
    // Also indicate the primitive topology of the buffer. Our buffer holds a triangle list - each set of 3 vertices
    // will be connected into a triangle. Again, there is no need to do this more than once if you are not changing topology
    gD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Select which shaders to use when rendering. Only need to do once if you are not changing shader
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);

    
    
    //// Render cube 1 ////

    // Send the world matrix for the cube over to the shaders on the GPU
    // See the section commented as "Constant Buffers" near the top of the file for more info about the data being sent here
    // - "Map" basically opens the GPU's constant buffer for writing
    // - "memcpy" copies the C++ data over to the GPU's constant buffer
    // - "Unmap" closes the GPU's buffer again - we must do this as soon as possible
    gPerModelConstants.worldMatrix = gCubeMatrix;

    gD3DContext->Map(gPerModelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
    memcpy(cb.pData, &gPerModelConstants, sizeof(gPerModelConstants));
    gD3DContext->Unmap(gPerModelConstantBuffer, 0);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
    // If you look at the vertex shader code, there is a structure with the same content that receives the data
    // The first parameter must match constant buffer number in the shader, so this is constant buffer 0 on the vertex shader
    gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader

    // Draw the cube, 36 vertices starting at vertex 0
    gD3DContext->Draw(36, 0);


	gPerModelConstants.worldMatrix = gCubeMatrix2;
	gD3DContext->Map(gPerModelConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &cb);
	memcpy(cb.pData, &gPerModelConstants, sizeof(gPerModelConstants));
	gD3DContext->Unmap(gPerModelConstantBuffer, 0);

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS)
	// If you look at the vertex shader code, there is a structure with the same content that receives the data
	// The first parameter must match constant buffer number in the shader, so this is constant buffer 0 on the vertex shader
	gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader

																	   // Draw the cube, 36 vertices starting at vertex 0
	gD3DContext->Draw(36, 0);


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
    gPerFrameConstants.viewMatrix = InverseAffine(MatrixTranslation(CVector3 (0, 0, -5.0f)));

    // Create a "projection matrix" - this determines properties of the camera - again we'll see this later
    gPerFrameConstants.projectionMatrix = MakeProjectionMatrix(); // Using a helper function to make this special matrix

	
	static float posx = 0.0f;
	static float posy = 0.0f;
	static float posz = -5.0f;

	if (KeyHeld(Key_Up))
	{
		posy -= 2.0f *frameTime;
	}
	if (KeyHeld(Key_Down))
	{

		posy += 2.0f *frameTime;
	}

	//X
	if (KeyHeld(Key_Right))
	{

		posx -= 2.0f *frameTime;
	}
	if (KeyHeld(Key_Left))
	{

		posx += 2.0f *frameTime;
	}

	CVector3 cameraPos = { posx, posy, posz };
	
	

    //// Update cube 1 ////

    // Create a matrix to position and orientate the cube
	//rotations
    static float rotationY = 0;
	static float rotationX = 0;
	static float rotationZ = 0;
	static CVector3 scaleX = { 1.0f, 1.0f, 1.0f };
	static CVector3 translate = { 0.0f, 0.0f, 0.0f };
	static CVector3 position2 = { 0.0f, 0.0f, 10.0f }; //position of cube 2

    if (KeyHeld(Key_A))
    {
        rotationY += ToRadians(120) * frameTime;
    }
   
	if (KeyHeld(Key_D))
	{
		rotationY -= ToRadians(120) * frameTime;
	}
	

	if (KeyHeld(Key_S))
	{
		rotationX -= ToRadians(120) * frameTime;
	}
	
	if (KeyHeld(Key_W))
	{
		rotationX += ToRadians(120) * frameTime;
	}
	

	if (KeyHeld(Key_Q))
	{
		rotationZ += ToRadians(120) * frameTime;
	}
	
	if (KeyHeld(Key_E))
	{
		rotationZ -= ToRadians(120) * frameTime;
	}


	//X scaling
	if (KeyHeld(Key_2))
	{
		scaleX.x += 2.0f *frameTime;
		/*scaleX.y += 2.0f *frameTime;
		scaleX.z += 2.0f *frameTime;*/
	}
	

	if (KeyHeld(Key_1))
	{
		scaleX.x -= 2.0f *frameTime;
		/*scaleX.y -= 2.0f *frameTime;
		scaleX.z -= 2.0f *frameTime;*/
	}
	
	//matrix translation
	//Z
	if (KeyHeld(Key_I))
	{
		
		 translate.z+= 2.0f *frameTime;
	}
	if (KeyHeld(Key_K))
	{
		
		 translate.z-= 2.0f *frameTime;
	}

	//X
	if (KeyHeld(Key_L))
	{

		translate.x += 2.0f *frameTime;
	}
	if (KeyHeld(Key_J))
	{

		translate.x -= 2.0f *frameTime;
	}

	//Y
	if (KeyHeld(Key_U))
	{

		translate.y += 2.0f *frameTime;
	}
	if (KeyHeld(Key_O))
	{

		translate.y -= 2.0f *frameTime;
	}

	
	gPerFrameConstants.viewMatrix = MatrixTranslation(cameraPos); //only camera working for some reason.. 

	gCubeMatrix = MatrixScaling(scaleX)* MatrixRotationZ(rotationZ)* MatrixRotationY(rotationY)* MatrixRotationX(rotationX)*MatrixTranslation(translate);
	gCubeMatrix2 = MatrixTranslation(position2);





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
        std::string windowTitle = "CO2409 Week 8: 3D Matrices - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}

