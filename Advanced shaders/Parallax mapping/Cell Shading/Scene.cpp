//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "State.h"
#include "Shader.h"
#include "Input.h"
#include "Common.h"

#include "CVector2.h" 
#include "CVector3.h" 
#include "CMatrix4x4.h"
#include "MathHelpers.h"     // Helper functions for maths
#include "GraphicsHelpers.h" // Helper functions to unclutter the code here

#include "ColourRGBA.h" 

#include <sstream>
#include <memory>


//--------------------------------------------------------------------------------------
// Scene Data
//--------------------------------------------------------------------------------------
// Addition of Mesh, Model and Camera classes have greatly simplified this section
// Geometry data has gone to Mesh class. Positions, rotations, matrices have gone to Model and Camera classes

// Constants controlling speed of movement/rotation (measured in units per second because we're using frame time)
const float ROTATION_SPEED = 2.0f;  // 2 radians per second for rotation
const float MOVEMENT_SPEED = 50.0f; // 50 units per second for movement (what a unit of length is depends on 3D model - i.e. an artist decision usually)


// Meshes, models and cameras, same meaning as TL-Engine. Meshes prepared in InitGeometry function, Models & camera in InitScene
Mesh* gTrollMesh;
Mesh* gTeapotMesh;
Mesh* gLightMesh;

Model* gTroll;
Model* gTeapot;
Model* gLight1;
Model* gLight2;

Camera* gCamera;


// Additional light information
CVector3 gLight1Colour = { 0.8f, 0.8f, 1.0f };
float    gLight1Strength = 10; // Allows the light to be stronger or weaker - also controls the light model scale

CVector3 gLight2Colour = { 1.0f, 0.8f, 0.2f };
float    gLight2Strength = 40;

CVector3 gAmbientColour = { 0.3f, 0.3f, 0.3f }; // Background level of light (slightly bluish to match the far background, which is dark blue)
float    gSpecularPower = 256; // Specular power controls shininess - same for all models in this app


// Variables controlling light1's orbiting of the cube
const float gLightOrbit = 20.0f;
const float gLightOrbitSpeed = 0.7f;


// Cell shading data
CVector3 OutlineColour = {0, 0, 0};
float    OutlineThickness = 0.015f;



//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
// Variables sent over to the GPU each frame
// The structures are now in Common.h
// IMPORTANT: Any new data you add in C++ code (CPU-side) is not automatically available to the GPU
//            Anything the shaders need (per-frame or per-model) needs to be sent via a constant buffer

PerFrameConstants gPerFrameConstants;      // The constants that need to be sent to the GPU each frame (see common.h for structure)
ID3D11Buffer*     gPerFrameConstantBuffer; // The GPU buffer that will recieve the constants above

PerModelConstants gPerModelConstants;      // As above, but constant that change per-model (e.g. world matrix)
ID3D11Buffer*     gPerModelConstantBuffer; // --"--



//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

// DirectX objects controlling textures used in this lab
ID3D11Resource*           gTrollDiffuseMap    = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11ShaderResourceView* gTrollDiffuseMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

ID3D11Resource*           gTeapotDiffuseMap    = nullptr;
ID3D11ShaderResourceView* gTeapotDiffuseMapSRV = nullptr;

ID3D11Resource*           gCellMap    = nullptr;
ID3D11ShaderResourceView* gCellMapSRV = nullptr;

ID3D11Resource*           gLightDiffuseMap    = nullptr;
ID3D11ShaderResourceView* gLightDiffuseMapSRV = nullptr;


//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    //*************

    // Load mesh geometry data, just like TL-Engine this doesn't create anything in the scene. Create a Model for that.
    // IMPORTANT NOTE: Will only keep the first object from the mesh - multipart objects will have parts missing - see later lab for more robust loader
    try 
    {
        gTrollMesh  = new Mesh("troll.x");
        gTeapotMesh = new Mesh("teapot.x");
        gLightMesh  = new Mesh("light.x");
    }
    catch (std::runtime_error e)
    {
        gLastError = e.what(); // This picks up the error message put in the exception (see Mesh.cpp)
        return false;
    }

    //*************


    // Load the shaders required for the geometry we will use (see Shader.cpp / .h)
    if (!LoadShaders())
    {
        gLastError = "Error loading shaders";
        return false;
    }


    // Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
    // These allow us to pass data from CPU to shaders such as lighting information or matrices
    // See the comments above where these variable are declared and also the UpdateScene function
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
        return false;
    }


    //// Load / prepare textures on the GPU ////

    // Load textures and create DirectX objects for them
    // The LoadTexture function requires you to pass a ID3D11Resource* (e.g. &gCubeDiffuseMap), which manages the GPU memory for the
    // texture and also a ID3D11ShaderResourceView* (e.g. &gCubeDiffuseMapSRV), which allows us to use the texture in shaders
    // The function will fill in these pointers with usable data. The variables used here are globals found near the top of the file.
    if (!LoadTexture("Green.png",        &gTrollDiffuseMap,  &gTrollDiffuseMapSRV  ) ||
        !LoadTexture("Red.png",          &gTeapotDiffuseMap, &gTeapotDiffuseMapSRV ) ||
        !LoadTexture("CellGradient.png", &gCellMap,          &gCellMapSRV          ) ||
        !LoadTexture("Flare.jpg",        &gLightDiffuseMap,  &gLightDiffuseMapSRV ))
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
    //*************

    // The new model class encapsulates position, rotation, scaling and world matrix
    // Scene preperation is now more intuitive
    // See note in InitGeometry about why we're not using unique_ptr
    gTroll  = new Model(gTrollMesh);
    gTeapot = new Model(gTeapotMesh);
    gLight1 = new Model(gLightMesh);
    gLight2 = new Model(gLightMesh);

    gTroll->SetPosition({ 30, 10, 30 });
	gTroll->SetScale( 4.0f );

   	gTeapot->SetPosition({ 0, 10, 0 });

    gLight1->SetPosition({ 30, 10, 0 });
    gLight1->SetScale(pow(gLight1Strength, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.

    gLight2->SetPosition({ -20, 30, 50 });
    gLight2->SetScale(pow(gLight2Strength, 0.7f));


    gCamera = new Camera();
    gCamera->SetPosition({ 20, 20, 60 });
    gCamera->SetRotation({ ToRadians(15.0f), ToRadians(180.0f), 0.0f });

    //*************

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();

    if (gLightDiffuseMapSRV)   gLightDiffuseMapSRV->Release();
    if (gLightDiffuseMap)      gLightDiffuseMap->Release();
    if (gCellMapSRV)           gCellMapSRV->Release();
    if (gCellMap)              gCellMap->Release();
    if (gTeapotDiffuseMapSRV)  gTeapotDiffuseMapSRV->Release();
    if (gTeapotDiffuseMap)     gTeapotDiffuseMap->Release();
    if (gTrollDiffuseMapSRV)   gTrollDiffuseMapSRV->Release();
    if (gTrollDiffuseMap)      gTrollDiffuseMap->Release();

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    ReleaseShaders();

    // See note in InitGeometry about why we're not using unique_ptr and having to manually delete
    delete gCamera;  gCamera = nullptr;
    delete gTroll;   gTroll  = nullptr;
    delete gTeapot;  gTeapot = nullptr;
    delete gLight1;  gLight1 = nullptr;
    delete gLight2;  gLight2 = nullptr;

    delete gTrollMesh;  gTrollMesh  = nullptr;
    delete gTeapotMesh; gTeapotMesh = nullptr;
    delete gLightMesh;  gLightMesh  = nullptr;
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


    // Send per-frame data (camera matrices and lighting info) over to the shaders on the GPU
    // See the section commented as "Constant Buffers" near the top of the file and in common.h for more info about the data here
    // The DirectX code for this has been moved to a helper function in Utility/SceneHelpers.cpp
    gPerFrameConstants.viewMatrix           = gCamera->ViewMatrix();
    gPerFrameConstants.projectionMatrix     = gCamera->ProjectionMatrix();
    gPerFrameConstants.viewProjectionMatrix = gCamera->ViewProjectionMatrix();
    gPerFrameConstants.light1Colour   = gLight1Colour * gLight1Strength;
    gPerFrameConstants.light1Position = gLight1->Position();
    gPerFrameConstants.light2Colour   = gLight2Colour * gLight2Strength;
    gPerFrameConstants.light2Position = gLight2->Position();
    gPerFrameConstants.ambientColour  = gAmbientColour;
    gPerFrameConstants.specularPower  = gSpecularPower;
    gPerFrameConstants.cameraPosition = gCamera->Position();
    gPerFrameConstants.outlineColour    = OutlineColour;
    gPerFrameConstants.outlineThickness = OutlineThickness;
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //-------------------------------------------------------------------------

    //// Render troll and teapot - first pass ////
    // Draw models inside out, slightly bigger and black

    // Outline drawing - slightly scales object and draws black
    gD3DContext->VSSetShader(gCellShadingOutlineVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCellShadingOutlinePixelShader,  nullptr, 0);

    // States - no blending, normal depth buffer. However, use front culling to draw *inside* of model
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullFrontState);

    // No textures needed, draws outline in plain colour

    // Render models, no GPU changes needed between rendering them in this case
    gTroll->Render();
    gTeapot->Render();


    //-------------------------------------------------------------------------

    //// Render troll and teapot - second pass ////
    // Draw models right way round, normal sized and with cell shading

    // Main cell shading shaders
    gD3DContext->VSSetShader(gCellShadingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCellShadingPixelShader,  nullptr, 0);

    // Switch back to the usual back face culling (not inside out)
    gD3DContext->RSSetState(gCullBackState);
    
    // Select the troll texture and sampler
    gD3DContext->PSSetShaderResources(0, 1, &gTrollDiffuseMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Also, cell shading uses a special 1D "cell map", which uses point sampling
    gD3DContext->PSSetShaderResources(1, 1, &gCellMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(1, 1, &gPointSampler);

    // Render troll model
    gTroll->Render();


    // Switch to teapot texture (no need to set sampler or cell map again)
    gD3DContext->PSSetShaderResources(0, 1, &gTeapotDiffuseMapSRV); // First parameter must match texture slot number in the shaer

    // Render teapot model
    gTeapot->Render();


    //-------------------------------------------------------------------------
   
    //// Render lights ////

    // Select which shaders to use next
    gD3DContext->VSSetShader(gLightModelVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gLightModelPixelShader,  nullptr, 0);

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gLightDiffuseMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // States - additive blending, read-only depth buffer and no culling (standard set-up for blending
    gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
    gPerModelConstants.objectColour = gLight1Colour; // Set any per-model constants apart from the world matrix just before calling render (light colour here)
    gLight1->Render();

    // The shaders, texture and states are the same, so no need to set them again to draw the second light
    gPerModelConstants.objectColour = gLight2Colour;
    gLight2->Render();


    //-------------------------------------------------------------------------

    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// Update models and camera. frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
	// Control cube (will update its world matrix)
	gTroll->Control( frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );

    // Orbit the light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	static float rotate = 0.0f;
	gLight1->SetPosition( gTroll->Position() + CVector3{ cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit } );
	rotate -= gLightOrbitSpeed * frameTime;

	// Control camera (will update its view matrix)
	gCamera->Control( frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );


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
        std::string windowTitle = "CO2409 Week 17: Cell Shading - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
