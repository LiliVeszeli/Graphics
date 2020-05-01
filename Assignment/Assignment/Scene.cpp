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
Mesh* gCharacterMesh;
Mesh* gCrateMesh;
Mesh* gGroundMesh;
Mesh* gLightMesh;
Mesh* gTeaPotMesh;
Mesh* gSphereMesh;
Mesh* gCubeMesh;
Mesh* gCubeMeshNormal;
Mesh* gParallaxMesh;
Mesh* gPortalMesh;

Model* gCharacter;
Model* gCrate;
Model* gGround;
Model* gTeaPot;
Model* gSphere;
Model* gCube;
Model* gCubeNormal;
Model* gParallax;
Model* gSpecular;
Model* gMul;
Model* gAdd;
Model* gAlphaTest;
Model* gCubeMap; //:(
Model* gSecret;
Model* gChangeModel;

Camera* gCamera;
float wiggle; //speed of the sphere wiggle
float change; //speed of the texture changing
float wiggleDirection = 1.0f;

// Store lights in an array in this exercise
const int NUM_LIGHTS = 5;
struct Light
{
    Model*   model;
    CVector3 colour;
    float    strength;
};
Light gLights[NUM_LIGHTS]; 


// Additional light information
CVector3 gAmbientColour = { 0.6f, 0.4f, 0.6f }; // Background level of light (slightly bluish to match the far background, which is dark blue)
float    gSpecularPower = 250.0f; // Specular power controls shininess - same for all models in this app

ColourRGBA gBackgroundColor = { 0.2f, 0.2f, 0.3f, 1.0f };

// Variables controlling light1's orbiting of the cube
const float gLightOrbit = 20.0f;
const float gLightOrbitSpeed = 0.7f;

// Spotlight data - using spotlights in this lab because shadow mapping needs to treat each light as a camera, which is easy with spotlights
float gSpotlightConeAngle = 90.0f; // Spot light cone angle (degrees), like the FOV (field-of-view) of the spot light

float gWiggle = 0;

float gChange = 0;

//--------------------------------------------------------------------------------------
//**** Shadow Texture  ****//
//--------------------------------------------------------------------------------------
// This texture will have the scene from the point of view of the light renderered on it. This texture is then used for shadow mapping

// Dimensions of shadow map texture - controls quality of shadows
int gShadowMapSize  = 1024;

// The shadow texture - effectively a depth buffer of the scene **from the light's point of view**
//                      Each frame it is rendered to, then the texture is used to help the per-pixel lighting shader identify pixels in shadow
ID3D11Texture2D*          gShadowMap1Texture      = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11DepthStencilView*   gShadowMap1DepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
ID3D11ShaderResourceView* gShadowMap1SRV          = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)


//*********************//



//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------


PerFrameConstants gPerFrameConstants;      // The constants that need to be sent to the GPU each frame (see common.h for structure)
ID3D11Buffer*     gPerFrameConstantBuffer; // The GPU buffer that will recieve the constants above

PerModelConstants gPerModelConstants;      // As above, but constant that change per-model (e.g. world matrix)
ID3D11Buffer*     gPerModelConstantBuffer; // --"--



//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

// DirectX objects controlling textures used in this lab
ID3D11Resource*           gCharacterDiffuseSpecularMap    = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11ShaderResourceView* gCharacterDiffuseSpecularMapSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

ID3D11Resource*           gCrateDiffuseSpecularMap    = nullptr;
ID3D11ShaderResourceView* gCrateDiffuseSpecularMapSRV = nullptr;

ID3D11Resource*           gGroundDiffuseSpecularMap    = nullptr;
ID3D11ShaderResourceView* gGroundDiffuseSpecularMapSRV = nullptr;

ID3D11Resource* gTeaPotDiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gTeaPotDiffuseSpecularMapSRV = nullptr;

ID3D11Resource* gSphereDiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gSphereDiffuseSpecularMapSRV = nullptr;

ID3D11Resource* gCube1DiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gCube1DiffuseSpecularMapSRV = nullptr;
ID3D11Resource* gCube2DiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gCube2DiffuseSpecularMapSRV = nullptr;

ID3D11Resource*           gLightDiffuseMap    = nullptr;
ID3D11ShaderResourceView* gLightDiffuseMapSRV = nullptr;

ID3D11Resource* gCubeNormalDiffuseSpecularMap = nullptr; 
ID3D11ShaderResourceView* gCubeNormalDiffuseSpecularMapSRV = nullptr; 
ID3D11Resource* gCubeNormalMap = nullptr;
ID3D11ShaderResourceView* gCubeNormalMapSRV = nullptr;

ID3D11Resource* gParallaxDiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gParallaxDiffuseSpecularMapSRV = nullptr;
ID3D11Resource* gParallaxNormalHeightMap = nullptr;
ID3D11ShaderResourceView* gParallaxNormalHeightMapSRV = nullptr;

ID3D11Resource* gSpecularDiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gSpecularDiffuseSpecularMapSRV = nullptr;

ID3D11Resource* gMulDiffuseMap = nullptr;
ID3D11ShaderResourceView* gMulDiffuseMapSRV = nullptr;

ID3D11Resource* gAddDiffuseMap = nullptr;
ID3D11ShaderResourceView* gAddDiffuseMapSRV = nullptr;

ID3D11Resource* gAlphaTestDiffuseMap = nullptr;
ID3D11ShaderResourceView* gAlphaTestDiffuseMapSRV = nullptr;

ID3D11Resource* gSecretDiffuseMap = nullptr;
ID3D11ShaderResourceView* gSecretDiffuseMapSRV = nullptr;

ID3D11Resource* gChange1DiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gChange1DiffuseSpecularMapSRV = nullptr;
ID3D11Resource* gChange2DiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gChange2DiffuseSpecularMapSRV = nullptr;
ID3D11Resource* gChangeNormalDiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gChangeNormalDiffuseSpecularMapSRV = nullptr;
ID3D11Resource* gChangeNormalMap = nullptr;
ID3D11ShaderResourceView* gChangeNormalMapSRV = nullptr;


ID3D11Resource* gCubeMapDiffuseSpecularMap = nullptr;
ID3D11ShaderResourceView* gCubeMapDiffuseSpecularMapSRV = nullptr;




//--------------------------------------------------------------------------------------
// Light Helper Functions
//--------------------------------------------------------------------------------------

// Get "camera-like" view matrix for a spotlight
CMatrix4x4 CalculateLightViewMatrix(int lightIndex)
{
    return InverseAffine(gLights[lightIndex].model->WorldMatrix());
}

// Get "camera-like" projection matrix for a spotlight
CMatrix4x4 CalculateLightProjectionMatrix(int lightIndex)
{
    return MakeProjectionMatrix(1.0f, ToRadians(gSpotlightConeAngle)); // Helper function in Utility\GraphicsHelpers.cpp
}


//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    // Load mesh geometry data, just like TL-Engine this doesn't create anything in the scene. Create a Model for that.
   
    try 
    {
        gCharacterMesh = new Mesh("Troll.x");
        gCrateMesh     = new Mesh("CargoContainer.x");
        gGroundMesh    = new Mesh("Ground.x");
        gLightMesh     = new Mesh("Light.x");
        gTeaPotMesh = new Mesh("Teapot.x");
        gSphereMesh = new Mesh("Sphere.x");
        gCubeMesh = new Mesh("Cube.x");
        gCubeMeshNormal = new Mesh("Cube.x", true);
        gParallaxMesh = new Mesh("Cube.x", true);
        gPortalMesh = new Mesh("Portal.x");
       
        
    }
    catch (std::runtime_error e)  // Constructors cannot return error messages so use exceptions to catch mesh errors (fairly standard approach this)
    {
        gLastError = e.what(); // This picks up the error message put in the exception (see Mesh.cpp)
        return false;
    }


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
    if (!LoadTexture("porcelain.jpg", &gCharacterDiffuseSpecularMap, &gCharacterDiffuseSpecularMapSRV) ||
        !LoadTexture("CargoA.dds",               &gCrateDiffuseSpecularMap,     &gCrateDiffuseSpecularMapSRV    ) ||
        !LoadTexture("GrassDiffuseSpecular1.dds", &gGroundDiffuseSpecularMap,    &gGroundDiffuseSpecularMapSRV   ) ||
        !LoadTexture("porcelain.jpg", &gTeaPotDiffuseSpecularMap, &gTeaPotDiffuseSpecularMapSRV) ||
        !LoadTexture("holo.jpg", &gSphereDiffuseSpecularMap, &gSphereDiffuseSpecularMapSRV) ||
        !LoadTexture("mosaic.jpg", &gCube1DiffuseSpecularMap, &gCube1DiffuseSpecularMapSRV) ||
        !LoadTexture("purple.jpg", &gCube2DiffuseSpecularMap, &gCube2DiffuseSpecularMapSRV) ||
        !LoadTexture("PatternDiffuseSpecular.dds", &gCubeNormalDiffuseSpecularMap, &gCubeNormalDiffuseSpecularMapSRV) ||
        !LoadTexture("PatternNormal.dds", &gCubeNormalMap, &gCubeNormalMapSRV) ||
        !LoadTexture("PatternDiffuseSpecular.dds", &gParallaxDiffuseSpecularMap, &gParallaxDiffuseSpecularMapSRV) ||
        !LoadTexture("PatternNormalHeight.dds", &gParallaxNormalHeightMap, &gParallaxNormalHeightMapSRV) ||
        !LoadTexture("StoneDiffuseSpecular.dds", &gSpecularDiffuseSpecularMap, &gSpecularDiffuseSpecularMapSRV) ||
        !LoadTexture("Glass.jpg", &gMulDiffuseMap, &gMulDiffuseMapSRV) ||
        !LoadTexture("FireAdd.png", &gAddDiffuseMap, &gAddDiffuseMapSRV) ||
        !LoadTexture("wizard.png", &gAlphaTestDiffuseMap, &gAlphaTestDiffuseMapSRV) ||
        !LoadTexture("secret.png", &gSecretDiffuseMap, &gSecretDiffuseMapSRV) ||
        !LoadTexture("beach.dds", &gCubeMapDiffuseSpecularMap, &gCubeMapDiffuseSpecularMapSRV) ||
        !LoadTexture("PatternDiffuseSpecular.dds", &gChangeNormalDiffuseSpecularMap, &gChangeNormalDiffuseSpecularMapSRV) ||
        !LoadTexture("PatternDiffuseSpecular.dds", &gChange1DiffuseSpecularMap, &gChange1DiffuseSpecularMapSRV) ||
        !LoadTexture("PatternYellowDiffuseSpecular.dds", &gChange2DiffuseSpecularMap, &gChange2DiffuseSpecularMapSRV) ||
        !LoadTexture("PatternNormal.dds", &gChangeNormalMap, &gChangeNormalMapSRV) ||
        !LoadTexture("Flare.jpg",                &gLightDiffuseMap,             &gLightDiffuseMapSRV))
    {
        gLastError = "Error loading textures";
        return false;
    }



	//**** Create Shadow Map texture ****//

	// We also need a depth buffer to go with our portal
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width  = gShadowMapSize; // Size of the shadow map determines quality / resolution of shadows
	textureDesc.Height = gShadowMapSize;
	textureDesc.MipLevels = 1; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	if (FAILED(gD3DDevice->CreateTexture2D(&textureDesc, NULL, &gShadowMap1Texture) ))
	{
		gLastError = "Error creating shadow map texture";
		return false;
	}

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
    dsvDesc.Flags = 0;
	if (FAILED(gD3DDevice->CreateDepthStencilView(gShadowMap1Texture, &dsvDesc, &gShadowMap1DepthStencil) ))
	{
		gLastError = "Error creating shadow map depth stencil view";
		return false;
	}

   
 	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
                                           // but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	if (FAILED(gD3DDevice->CreateShaderResourceView(gShadowMap1Texture, &srvDesc, &gShadowMap1SRV) ))
	{
		gLastError = "Error creating shadow map shader resource view";
		return false;
	}



   //*****************************//


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
    //// Set up scene ////

    gCharacter = new Model(gCharacterMesh);
    gCrate     = new Model(gCrateMesh);
    gGround    = new Model(gGroundMesh);
    gTeaPot = new Model(gTeaPotMesh);
    gSphere = new Model(gSphereMesh);
    gCube = new Model(gCubeMesh);
    gCubeNormal = new Model(gCubeMeshNormal);
    gParallax = new Model(gParallaxMesh);
    gSpecular = new Model(gCubeMesh);
    gMul = new Model(gCubeMesh);
    gAdd = new Model(gCubeMesh);
    gCubeMap = new Model(gSphereMesh);
    gAlphaTest = new Model(gCubeMesh);
    gSecret = new Model(gPortalMesh);
    gChangeModel = new Model(gCubeMeshNormal);

	// Initial positions, scalinG and rotation
	gCharacter->SetPosition({ 20, 0, 0 });
    gCharacter->SetScale(6);
    gCharacter->SetRotation({ 0, ToRadians(215.0f), 0 });
	gCrate-> SetPosition({ 40, 0, 30 });
	gCrate-> SetScale(6);
	gCrate-> SetRotation({ 0.0f, ToRadians(-20.0f), 0.0f });
    gTeaPot->SetPosition({ 10, 0, 40 });
    gSphere->SetPosition({ 10, 5, -30 });
    gSphere->SetScale(0.5f);
    gCube->SetPosition({ -13, 5, 10 });
    gCubeNormal->SetPosition({ -40, 5, 45 });
    gParallax->SetPosition({ -24, 5, 40 });
    gParallax->SetScale(0.8f);
    gSpecular->SetPosition({-45,5,-5});
    gSpecular->SetRotation({ 0,380,0 });
    gMul->SetPosition({ 25, 5, -25 });
    gMul->SetScale(0.7f);
    gAdd->SetPosition({40, 21, 30});
    gAlphaTest->SetPosition({-15, 5.5f, -25});
    gAlphaTest->SetScale(0.8f);
    gSecret->SetPosition({ 25, 2, -25 });
    gSecret->SetScale(0.1);
    gSecret->SetRotation({300, 0, 400});
    gChangeModel->SetPosition({ -55, 5, 45});

    gCubeMap->SetPosition({ -25, 5, -35 });
    gCubeMap->SetScale(0.4);

    
  
   
  

    // Light set-up - using an array this time
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        gLights[i].model = new Model(gLightMesh);
    }

    gLights[0].colour = { 0.8f, 0.8f, 1.0f };
    gLights[0].strength = 20;
    gLights[0].model->SetPosition({ 30, 20, 0 });
    gLights[0].model->SetScale(pow(gLights[0].strength, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.
	gLights[0].model->FaceTarget(gCharacter->Position());

    gLights[1].colour = { 1.0f, 0.8f, 0.2f };
    gLights[1].strength = 40;
    gLights[1].model->SetPosition({ -30, 30, 30 });
    gLights[1].model->SetScale(pow(15, 0.7f));
	gLights[1].model->FaceTarget({ 0, 0, 0 });

   

    gLights[2].colour = {0, 0, 0};
    gLights[2].strength = 25;
    gLights[2].model->SetPosition({ 40,25, 0 });
    gLights[2].model->SetScale(pow(20, 0.7f));
    gLights[2].model->FaceTarget({ 0, 0, 0 });


    gLights[3].colour = { 0.8f, 0.8f, 1.0f };
    gLights[3].strength = 10;
    gLights[3].model->SetPosition({ -20, 10, 0 });
    gLights[3].model->SetScale(pow(20, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.
    gLights[3].model->FaceTarget(gSpecular->Position());

    gLights[4].colour = { 0.8f, 0.8f, 1.5f };
    gLights[4].strength = 1;
    gLights[4].model->SetPosition({ 25, 5, -25 });
    gLights[4].model->SetScale(pow(10, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.
    gLights[4].model->FaceTarget({ 0, 0, 0 });
  

    //// Set up camera ////

    gCamera = new Camera();
    gCamera->SetPosition({ 20, 26,-80 });
    gCamera->SetRotation({ ToRadians(10), ToRadians(-13), 0 });

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();

    if (gShadowMap1DepthStencil)  gShadowMap1DepthStencil->Release();
    if (gShadowMap1SRV)           gShadowMap1SRV->Release();
    if (gShadowMap1Texture)       gShadowMap1Texture->Release();
											
    if (gLightDiffuseMapSRV)             gLightDiffuseMapSRV->Release();
    if (gLightDiffuseMap)                gLightDiffuseMap->Release();
    if (gGroundDiffuseSpecularMapSRV)    gGroundDiffuseSpecularMapSRV->Release();
    if (gGroundDiffuseSpecularMap)       gGroundDiffuseSpecularMap->Release();
    if (gCrateDiffuseSpecularMapSRV)     gCrateDiffuseSpecularMapSRV->Release();
    if (gCrateDiffuseSpecularMap)        gCrateDiffuseSpecularMap->Release();
    if (gCharacterDiffuseSpecularMapSRV) gCharacterDiffuseSpecularMapSRV->Release();
    if (gCharacterDiffuseSpecularMap)    gCharacterDiffuseSpecularMap->Release();
    if (gTeaPotDiffuseSpecularMapSRV) gTeaPotDiffuseSpecularMapSRV->Release();
    if (gTeaPotDiffuseSpecularMap)    gTeaPotDiffuseSpecularMap->Release();
    if (gSphereDiffuseSpecularMapSRV)     gSphereDiffuseSpecularMapSRV->Release();
    if (gSphereDiffuseSpecularMap)        gSphereDiffuseSpecularMap->Release();
    if (gCube1DiffuseSpecularMapSRV)     gCube1DiffuseSpecularMapSRV->Release();
    if (gCube1DiffuseSpecularMap)        gCube1DiffuseSpecularMap->Release();
    if (gCube2DiffuseSpecularMapSRV)     gCube2DiffuseSpecularMapSRV->Release();
    if (gCube2DiffuseSpecularMap)        gCube2DiffuseSpecularMap->Release();
    if (gCubeNormalDiffuseSpecularMapSRV)   gCubeNormalDiffuseSpecularMapSRV->Release();
    if (gCubeNormalDiffuseSpecularMap)      gCubeNormalDiffuseSpecularMap->Release();
    if (gCubeNormalMapSRV)            gCubeNormalMapSRV->Release();
    if (gCubeNormalMap)               gCubeNormalMap->Release();
    if (gParallaxDiffuseSpecularMapSRV) gParallaxDiffuseSpecularMapSRV->Release();
    if (gParallaxDiffuseSpecularMap)    gParallaxDiffuseSpecularMap->Release();
    if (gParallaxNormalHeightMapSRV)    gParallaxNormalHeightMapSRV->Release();
    if (gParallaxNormalHeightMap)       gParallaxNormalHeightMap->Release();
    if (gSpecularDiffuseSpecularMapSRV)     gSpecularDiffuseSpecularMapSRV->Release();
    if (gSpecularDiffuseSpecularMap)        gSpecularDiffuseSpecularMap->Release();
    if (gMulDiffuseMapSRV)             gMulDiffuseMapSRV->Release();
    if (gMulDiffuseMap)                gMulDiffuseMap->Release();
    if (gAddDiffuseMapSRV)             gAddDiffuseMapSRV->Release();
    if (gAddDiffuseMap)                gAddDiffuseMap->Release();
    if (gSecretDiffuseMapSRV)             gSecretDiffuseMapSRV->Release();
    if (gSecretDiffuseMap)                gSecretDiffuseMap->Release();
    if (gAlphaTestDiffuseMapSRV)             gAlphaTestDiffuseMapSRV->Release();
    if (gAlphaTestDiffuseMap)                gAlphaTestDiffuseMap->Release();
    if (gCubeMapDiffuseSpecularMapSRV)     gCubeMapDiffuseSpecularMapSRV->Release();
    if (gCubeMapDiffuseSpecularMap)        gCubeMapDiffuseSpecularMap->Release();
    if (gChangeNormalMapSRV)            gChangeNormalMapSRV->Release();
    if (gChangeNormalMap)               gChangeNormalMap->Release();
    if (gChangeNormalDiffuseSpecularMapSRV)   gChangeNormalDiffuseSpecularMapSRV->Release();
    if (gChangeNormalDiffuseSpecularMap)      gChangeNormalDiffuseSpecularMap->Release();

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();
    if (gChange1DiffuseSpecularMapSRV)     gChange1DiffuseSpecularMapSRV->Release();
    if (gChange1DiffuseSpecularMap)        gChange1DiffuseSpecularMap->Release();
    if (gChange2DiffuseSpecularMapSRV)     gChange2DiffuseSpecularMapSRV->Release();
    if (gChange2DiffuseSpecularMap)        gChange2DiffuseSpecularMap->Release();

    ReleaseShaders();

    // See note in InitGeometry about why we're not using unique_ptr and having to manually delete
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        delete gLights[i].model;  gLights[i].model = nullptr;
    }
    delete gCamera;    gCamera    = nullptr;
    delete gGround;    gGround    = nullptr;
    delete gCrate;     gCrate     = nullptr;
    delete gCharacter; gCharacter = nullptr;
    delete gTeaPot; gTeaPot = nullptr;
    delete gSphere;     gSphere = nullptr;
    delete gCube;     gCube = nullptr;
    delete gCubeNormal;     gCubeNormal = nullptr;
    delete gParallax;     gParallax = nullptr;
    delete gSpecular;     gSpecular = nullptr;
    delete gMul;     gMul = nullptr;
    delete gAdd;     gAdd = nullptr;
    delete gSecret;     gSecret = nullptr;
    delete gAlphaTest;     gAlphaTest = nullptr;
    delete gChangeModel;     gChangeModel = nullptr;
    delete gCubeMap;     gCubeMap = nullptr;


    delete gLightMesh;     gLightMesh     = nullptr;
    delete gGroundMesh;    gGroundMesh    = nullptr;
    delete gCrateMesh;     gCrateMesh     = nullptr;
    delete gCharacterMesh; gCharacterMesh = nullptr;
    delete gTeaPotMesh; gTeaPotMesh = nullptr;
    delete gCubeMesh;     gCubeMesh = nullptr;
    delete gCubeMeshNormal;     gCubeMeshNormal = nullptr;
    delete gParallaxMesh;     gParallaxMesh = nullptr;

}



//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Render the scene from the given light's point of view. Only renders depth buffer
void RenderDepthBufferFromLight(int lightIndex)
{
    // Get camera-like matrices from the spotlight, seet in the constant buffer and send over to GPU
    gPerFrameConstants.viewMatrix           = CalculateLightViewMatrix(lightIndex);
    gPerFrameConstants.projectionMatrix     = CalculateLightProjectionMatrix(lightIndex);
    gPerFrameConstants.viewProjectionMatrix = gPerFrameConstants.viewMatrix * gPerFrameConstants.projectionMatrix;
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //// Only render models that cast shadows ////

    // Use special depth-only rendering shaders
    gD3DContext->VSSetShader(gBasicTransformVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gDepthOnlyPixelShader,       nullptr, 0);
    
    // States - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullFrontState);

    // Render models - no state changes required between each object in this situation (no textures used in this step)
    gGround->Render();
    gCharacter->Render();
    gCrate->Render();
    gTeaPot->Render();
    gSphere->Render();
    gCube->Render();
    gCubeNormal->Render();
    gParallax->Render();
    gSpecular->Render();
    gChangeModel->Render();
   
}



// Render everything in the scene from the given camera
// This code is common between rendering the main scene and rendering the scene in the portal
// See RenderScene function ow
void RenderSceneFromCamera(Camera* camera)
{
    // Set camera matrices in the constant buffer and send over to GPU
    gPerFrameConstants.viewMatrix           = camera->ViewMatrix();
    gPerFrameConstants.projectionMatrix     = camera->ProjectionMatrix();
    gPerFrameConstants.viewProjectionMatrix = camera->ViewProjectionMatrix();
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //// Render lit models ////

    // Select which shaders to use next
    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gPixelLightingPixelShader,  nullptr, 0);
    
    // States - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullBackState);

    // Select the approriate textures and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gGroundDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shader
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model - it will update the model's world matrix and send it to the GPU in a constant buffer, then it will call
    // the Mesh render function, which will set up vertex & index buffer before finally calling Draw on the GPU
    gGround->Render();

    // Render other lit models, only change textures for each onee
    gD3DContext->PSSetShaderResources(0, 1, &gCharacterDiffuseSpecularMapSRV); 
    gCharacter->Render();

    gD3DContext->PSSetShaderResources(0, 1, &gCrateDiffuseSpecularMapSRV);
    gCrate->Render();

    gD3DContext->PSSetShaderResources(0, 1, &gTeaPotDiffuseSpecularMapSRV);
    gTeaPot->Render();

   

   
    gD3DContext->PSSetShaderResources(0, 1, &gSecretDiffuseMapSRV);
    gSecret->Render();



    gD3DContext->VSSetShader(gSpecularVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSpecularPixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gSpecularDiffuseSpecularMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gSpecular->Render();




    gD3DContext->VSSetShader(gSphereVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSpherePixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gSphereDiffuseSpecularMapSRV);

    gSphere->Render();

    gD3DContext->VSSetShader(gCubeVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCubePixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gCube1DiffuseSpecularMapSRV);
    gD3DContext->PSSetShaderResources(1, 1, &gCube2DiffuseSpecularMapSRV);

    gCube->Render();

    // Select the cube texture and sampler to use in the pixel shader
    gD3DContext->VSSetShader(gNormalMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gNormalMappingPixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gCubeNormalDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shader
    gD3DContext->PSSetShaderResources(1, 1, &gCubeNormalMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    gCubeNormal->Render();


    gD3DContext->VSSetShader(gParallaxVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gParallaxPixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gParallaxDiffuseSpecularMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetShaderResources(1, 1, &gParallaxNormalHeightMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
    gParallax->Render();


    gD3DContext->VSSetShader(gNormalMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gChangePixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gChangeNormalDiffuseSpecularMapSRV);
    gD3DContext->PSSetShaderResources(1, 1, &gChangeNormalMapSRV);
    gD3DContext->PSSetShaderResources(2, 1, &gChange1DiffuseSpecularMapSRV);
    gD3DContext->PSSetShaderResources(3, 1, &gChange2DiffuseSpecularMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    gChangeModel->Render();

    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCubeMapPixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gCubeMapDiffuseSpecularMapSRV);
    gCubeMap->Render();


    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gAlphaTestingPixelShader, nullptr, 0);
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->PSSetShaderResources(0, 1, &gAlphaTestDiffuseMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gTrilinearSampler);

    gD3DContext->RSSetState(gCullNoneState);

    gAlphaTest->Render();

    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gBlendingPixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gMulDiffuseMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    gD3DContext->OMSetBlendState(gMultiplicativeBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    gMul->Render();


    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gBlendingPixelShader, nullptr, 0);
    gD3DContext->PSSetShaderResources(0, 1, &gAddDiffuseMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    gAdd->Render();




  

    //// Render lights ////

    // Select which shaders to use next
    gD3DContext->VSSetShader(gBasicTransformVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gLightModelPixelShader,      nullptr, 0);

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gLightDiffuseMapSRV); // First parameter must match texture slot number in the shaer
   // gD3DContext->PSSetSamplers(0, 1, &gnosampler);

    gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Render all the lights in the array
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        gPerModelConstants.objectColour = gLights[i].colour; // Set any per-model constants apart from the world matrix just before calling render (light colour here)
        gLights[i].model->Render();
    }
}




// Rendering the scene now renders everything twice. First it renders the scene for the portal into a texture.
// Then it renders the main scene using the portal texture on a model.
void RenderScene()
{
    //// Common settings ////

    // Set up the light information in the constant buffer
    // Don't send to the GPU yet, the function RenderSceneFromCamera will do that
    gPerFrameConstants.light1Colour   = gLights[0].colour * gLights[0].strength;
    gPerFrameConstants.light1Position = gLights[0].model->Position();
    gPerFrameConstants.light1Facing   = Normalise(gLights[0].model->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light1CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light1ViewMatrix       = CalculateLightViewMatrix(0);         // Calculate camera-like matrices for...
    gPerFrameConstants.light1ProjectionMatrix = CalculateLightProjectionMatrix(0);   //...lights to support shadow mapping

    gPerFrameConstants.light2Colour = gLights[1].colour * gLights[1].strength;
    gPerFrameConstants.light2Position = gLights[1].model->Position();
    gPerFrameConstants.light2Facing = Normalise(gLights[1].model->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light2CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light2ViewMatrix = CalculateLightViewMatrix(1);         // Calculate camera-like matrices for...
    gPerFrameConstants.light2ProjectionMatrix = CalculateLightProjectionMatrix(1);   //...lights to support shadow mapping

    gPerFrameConstants.light3Colour = gLights[2].colour * gLights[2].strength;
    gPerFrameConstants.light3Position = gLights[2].model->Position();
    gPerFrameConstants.light3Facing = Normalise(gLights[2].model->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light3CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light3ViewMatrix = CalculateLightViewMatrix(2);         // Calculate camera-like matrices for...
    gPerFrameConstants.light3ProjectionMatrix = CalculateLightProjectionMatrix(2);   //...lights to support shadow mapping

    gPerFrameConstants.light4Colour = gLights[3].colour * gLights[3].strength;
    gPerFrameConstants.light4Position = gLights[3].model->Position();
    gPerFrameConstants.light4Facing = Normalise(gLights[3].model->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light4CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light4ViewMatrix = CalculateLightViewMatrix(3);         // Calculate camera-like matrices for...
    gPerFrameConstants.light4ProjectionMatrix = CalculateLightProjectionMatrix(3);   //...lights to support shadow mapping


    gPerFrameConstants.light5Colour = gLights[4].colour * gLights[3].strength;
    gPerFrameConstants.light5Position = gLights[4].model->Position();
    gPerFrameConstants.light5Facing = Normalise(gLights[4].model->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light5CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light5ViewMatrix = CalculateLightViewMatrix(4);         // Calculate camera-like matrices for...
    gPerFrameConstants.light5ProjectionMatrix = CalculateLightProjectionMatrix(4);   //...lights to support shadow mapping

    gPerFrameConstants.ambientColour  = gAmbientColour;
    gPerFrameConstants.specularPower  = gSpecularPower;
    gPerFrameConstants.cameraPosition = gCamera->Position();

    gPerFrameConstants.wiggle = wiggle;
    gPerFrameConstants.change = change;

    //***************************************//
    //// Render from light's point of view ////
    
    // Only rendering from light 1 to begin with

    // Setup the viewport to the size of the shadow map texture
    D3D11_VIEWPORT vp;
    vp.Width  = static_cast<FLOAT>(gShadowMapSize);
    vp.Height = static_cast<FLOAT>(gShadowMapSize);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Select the shadow map texture as the current depth buffer. We will not be rendering any pixel colours
    // Also clear the the shadow map depth buffer to the far distance
    gD3DContext->OMSetRenderTargets(0, nullptr, gShadowMap1DepthStencil);
    gD3DContext->ClearDepthStencilView(gShadowMap1DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	RenderDepthBufferFromLight(0);


    //**************************//


    //// Main scene rendering ////

    // Set the back buffer as the target for rendering and select the main depth buffer.
    // When finished the back buffer is sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);

    // Clear the back buffer to a fixed colour and the depth buffer to the far distance
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, &gBackgroundColor.r);
    gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Setup the viewport to the size of the main window
    vp.Width  = static_cast<FLOAT>(gViewportWidth);
    vp.Height = static_cast<FLOAT>(gViewportHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Set shadow maps in shaders
    // First parameter is the "slot", must match the Texture2D declaration in the HLSL code
    // In this app the diffuse map uses slot 0, the shadow maps use slots 1 onwards. If we were using other maps (e.g. normal map) then
    // we might arrange things differently
    gD3DContext->PSSetShaderResources(1, 1, &gShadowMap1SRV);
    gD3DContext->PSSetSamplers(1, 1, &gPointSampler);



    // Render the scene for the main window
    RenderSceneFromCamera(gCamera);

    // Unbind shadow maps from shaders - prevents warnings from DirectX when we try to render to the shadow maps again next frame
    ID3D11ShaderResourceView* nullView = nullptr;
    gD3DContext->PSSetShaderResources(1, 1, &nullView);





    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


float r = 255;
float g = 0;
float b = 0;

int dr = 0;
int dg = 0;
int db = 0;

bool strengthIsZero = false;

//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// Update models and camera. frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{

    wiggle += frameTime;
    change += frameTime/2 * wiggleDirection;

    if (change > 1.0f)
    {
        change = 1.0f;
        wiggleDirection = -1.0f;
    }
    if (change < 0)
    {
        change = 0;
        wiggleDirection = 1.0f;
    }

	// Control sphere (will update its world matrix)
	//gCharacter->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );
    gAlphaTest->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);

    // Orbit the light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	static float rotate = 0.0f;
    static bool go = true;
	gLights[0].model->SetPosition( gCharacter->Position() + CVector3{ cos(rotate) * gLightOrbit, 10, sin(rotate) * gLightOrbit } );
	gLights[0].model->FaceTarget(gCharacter->Position());
    gLights[3].model->SetPosition(gSpecular->Position() + CVector3{ cos(rotate) * gLightOrbit, 1, sin(rotate) * gLightOrbit });
    gLights[3].model->FaceTarget(gSpecular->Position());
    if (go)  rotate -= gLightOrbitSpeed * frameTime;
    if (KeyHit(Key_1))  go = !go;


       
    r += dr;
    g += dg;
    b += db;

    if (r == 255 && g == 0 && b == 0)
    {
        dr = 0; dg = 1; db = 0;
    }

    if (r == 255 && g == 255 && b == 0)
    {
        dr = -1; dg = 0; db = 0;
    }

    if (r == 0 && g == 255 && b == 0)
    {
        dr = 0; dg = 0; db = 1;
    }

    if (r == 0 && g == 255 && b == 255)
    {
        dr = 0; dg = -1; db = 0;
    }

    if (r == 0 && g == 0 && b == 255)
    {
        dr = 1; dg = 0; db = 0;
    }

    if (r == 255 && g == 0 && b == 255)
    {
        dr = 0; dg = 0; db = -1;
    }

    
    gLights[2].colour = { r/255, g/255, b/255 };

    /*************************************************
     * Title: Cycle RGB values as HUE
     * Author: Lukas
     * Date: 12/07/2012
     * Code version: 1.0
     * Availability: https://stackoverflow.com/questions/11458552/cycle-r-g-b-vales-as-hue
     *************************************************/

    if (strengthIsZero == false)
    {
        gLights[1].strength -= 0.1f;
        gLights[1].model->SetScale(pow(gLights[1].strength, 0.7f));
    }
    else
    {
        gLights[1].strength += 0.1f;
        gLights[1].model->SetScale(pow(gLights[1].strength, 0.7f));
    }

    if (gLights[1].strength < 0)
    {
        strengthIsZero = true;
    }
    if (gLights[1].strength > 40)
    {
        strengthIsZero = false;
    }

	// Control camera (will update its view matrix)
	gCamera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );


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
        std::string windowTitle = "CO2409 Week 20: Shadow Mapping - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
