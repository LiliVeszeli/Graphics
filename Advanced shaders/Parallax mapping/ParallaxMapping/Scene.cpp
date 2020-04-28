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
Mesh* gCubeMesh;
Mesh* gFloorMesh;
Mesh* gTeapotMesh;
Mesh* gLightMesh;

Model* gCube;
Model* gFloor;
Model* gTeapot;
Model* gLight1;
Model* gLight2;

Camera* gCamera;


// Additional light information
CVector3 gLight1Colour = { 0.8f, 0.8f, 1.0f };
float    gLight1Strength = 10; // Allows the light to be stronger or weaker - also controls the light model scale

CVector3 gLight2Colour = { 1.0f, 0.8f, 0.2f };
float    gLight2Strength = 40;

CVector3 gAmbientColour = { 0.3f, 0.3f, 0.5f }; // Background level of light (slightly bluish to match the far background, which is dark blue)
float    gSpecularPower = 256; // Specular power controls shininess - same for all models in this app


// Variables controlling light1's orbiting of the cube
const float gLightOrbit = 20.0f;
const float gLightOrbitSpeed = 0.7f;



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
ID3D11Resource*           gCubeDiffuseSpecularMap    = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11ShaderResourceView* gCubeDiffuseSpecularMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
ID3D11Resource*           gCubeNormalHeightMap       = nullptr;
ID3D11ShaderResourceView* gCubeNormalHeightMapSRV    = nullptr;

ID3D11Resource*           gTeapotDiffuseSpecularMap    = nullptr;
ID3D11ShaderResourceView* gTeapotDiffuseSpecularMapSRV = nullptr;
ID3D11Resource*           gTeapotNormalHeightMap       = nullptr;
ID3D11ShaderResourceView* gTeapotNormalHeightMapSRV    = nullptr;

ID3D11Resource*           gFloorDiffuseSpecularMap    = nullptr;
ID3D11ShaderResourceView* gFloorDiffuseSpecularMapSRV = nullptr;
ID3D11Resource*           gFloorNormalHeightMap       = nullptr;
ID3D11ShaderResourceView* gFloorNormalHeightMapSRV    = nullptr;

ID3D11Resource*           gLightDiffuseMap    = nullptr;
ID3D11ShaderResourceView* gLightDiffuseMapSRV = nullptr;

float gParallaxDepth = 0.08f; // Overall depth of bumpiness for parallax mapping
bool gUseParallax    = true;  // Toggle for parallax 


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
        gCubeMesh   = new Mesh("cube.x", true); // Second parameter is to calculate tangents
        gFloorMesh  = new Mesh("hills.x", true);
        gTeapotMesh = new Mesh("teapot.x", true);
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
	//****| INFO |*******************************************************************************************//
	// In the last lab we used normal maps, with the rgb components of the colours reused to store the xyz
	// components of a normal for each texel. These normals adjusted the lighting for each texel to give an
	// *impression* of bumpiness. However, parallax mapping attempts to show the true depth of the bumpy
	// surface. So it also needs to know the "height" of the surface: the distance of the bumps above or below
	// the flat plane of the surface geometry. This height is held in the alpha channel of the normal map (in
	// a similar way that the specular map is held in the alpha channel of the diffuse map)
	//*******************************************************************************************************//
    if (!LoadTexture("TechDiffuseSpecular.dds",    &gCubeDiffuseSpecularMap,   &gCubeDiffuseSpecularMapSRV  ) ||
        !LoadTexture("TechNormalHeight.dds",       &gCubeNormalHeightMap,      &gCubeNormalHeightMapSRV     ) ||
        !LoadTexture("PatternDiffuseSpecular.dds", &gTeapotDiffuseSpecularMap, &gTeapotDiffuseSpecularMapSRV) ||
        !LoadTexture("PatternNormalHeight.dds",    &gTeapotNormalHeightMap,    &gTeapotNormalHeightMapSRV   ) ||
        !LoadTexture("CobbleDiffuseSpecular.dds",  &gFloorDiffuseSpecularMap,  &gFloorDiffuseSpecularMapSRV ) ||
        !LoadTexture("CobbleNormalHeight.dds",     &gFloorNormalHeightMap,     &gFloorNormalHeightMapSRV    ) ||
        !LoadTexture("Flare.jpg",                  &gLightDiffuseMap,          &gLightDiffuseMapSRV ))
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
    gCube   = new Model(gCubeMesh);
    gFloor  = new Model(gFloorMesh);
    gTeapot = new Model(gTeapotMesh);
    gLight1 = new Model(gLightMesh);
    gLight2 = new Model(gLightMesh);

    gCube->SetPosition({ 10, 15, -40 });

   	gTeapot->SetPosition({ 40, 10, 10 });

    gLight1->SetPosition({ 30, 15, -40 });
    gLight1->SetScale(pow(gLight1Strength, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.

    gLight2->SetPosition({ 20, 40, -20 });
    gLight2->SetScale(pow(gLight2Strength, 0.7f));


    gCamera = new Camera();
    gCamera->SetPosition({ 40, 30, -90 });
    gCamera->SetRotation({ ToRadians(8.0f), ToRadians(-18.0f), 0.0f });

    //*************

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();

    if (gLightDiffuseMapSRV)          gLightDiffuseMapSRV->Release();
    if (gLightDiffuseMap)             gLightDiffuseMap->Release();
    if (gFloorDiffuseSpecularMapSRV)  gFloorDiffuseSpecularMapSRV->Release();
    if (gFloorDiffuseSpecularMap)     gFloorDiffuseSpecularMap->Release();
    if (gFloorNormalHeightMapSRV)     gFloorNormalHeightMapSRV->Release();
    if (gFloorNormalHeightMap)        gFloorNormalHeightMap->Release();
    if (gTeapotDiffuseSpecularMapSRV) gTeapotDiffuseSpecularMapSRV->Release();
    if (gTeapotDiffuseSpecularMap)    gTeapotDiffuseSpecularMap->Release();
    if (gTeapotNormalHeightMapSRV)    gTeapotNormalHeightMapSRV->Release();
    if (gTeapotNormalHeightMap)       gTeapotNormalHeightMap->Release();
    if (gCubeDiffuseSpecularMapSRV)   gCubeDiffuseSpecularMapSRV->Release();
    if (gCubeDiffuseSpecularMap)      gCubeDiffuseSpecularMap->Release();
    if (gCubeNormalHeightMapSRV)      gCubeNormalHeightMapSRV->Release();
    if (gCubeNormalHeightMap)         gCubeNormalHeightMap->Release();

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    ReleaseShaders();

    // See note in InitGeometry about why we're not using unique_ptr and having to manually delete
    delete gCamera;  gCamera = nullptr;
    delete gCube;    gCube   = nullptr;
    delete gFloor;   gFloor  = nullptr;
    delete gLight1;  gLight1 = nullptr;
    delete gLight2;  gLight2 = nullptr;

    delete gCubeMesh;   gCubeMesh  = nullptr;
    delete gFloorMesh;  gFloorMesh = nullptr;
    delete gLightMesh;  gLightMesh = nullptr;
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
    gPerFrameConstants.parallaxDepth = (gUseParallax ? gParallaxDepth : 0);
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //-------------------------------------------------------------------------

    //// Render floor ////

    // Select which shaders to use next
    gD3DContext->VSSetShader(gNormalMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gNormalMappingPixelShader,  nullptr, 0);
    
    // Select the floor textures and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gFloorDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shader
    gD3DContext->PSSetShaderResources(1, 1, &gFloorNormalHeightMapSRV); 
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Floor states - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullBackState);

    // Render model - it will update the cube's world matrix and send it to the GPU in a constant buffer, then it will call
    // the Mesh render function, which will set up vertex & index buffer before finally calling Draw on the GPU (all that code used to be here)
    gFloor->Render();


    //-------------------------------------------------------------------------

    //// Render cube ////

    // No change to shaders and states

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gCubeDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetShaderResources(1, 1, &gCubeNormalHeightMapSRV); 
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
    gCube->Render();


    //-------------------------------------------------------------------------

    //// Render teapot ////

    // No change to shaders and states

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gTeapotDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetShaderResources(1, 1, &gTeapotNormalHeightMapSRV); 
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
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
	gCube->Control( frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );

    // Orbit the light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	static float rotate = 0.0f;
	gLight1->SetPosition( gCube->Position() + CVector3{ cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit } );
	rotate -= gLightOrbitSpeed * frameTime;

	// Control camera (will update its view matrix)
	gCamera->Control( frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );

	// Toggle parallax
	if (KeyHit( Key_1 ))
	{
		gUseParallax = !gUseParallax;
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
        std::string windowTitle = "CO2409 Week 17: ParallaxMapping - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
