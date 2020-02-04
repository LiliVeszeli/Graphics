//--------------------------------------------------------------------------------------
// State creation
// - Sampler state (Bilinear, trilinear etc.)
// - Blender state (Additive blending, alpha blending etc.)
// - Rasterizer state (Wireframe mode, don't cull back faces etc.)
// - Depth stencil state (How to use the depth and stencil buffer)
//--------------------------------------------------------------------------------------

#include "State.h"
#include "Common.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

// GPU "States" //

// A sampler state object represents a way to filter textures, such as bilinear or trilinear. We have one object for each method we want to use
ID3D11SamplerState* gTrilinearSampler = nullptr;

// Blend states allow us to switch between blending modes (none, additive, multiplicative etc.)
ID3D11BlendState* gNoBlendState = nullptr;
ID3D11BlendState* gAdditiveBlending = nullptr;
ID3D11BlendState* gMultiplicativeBlending = nullptr;
ID3D11BlendState* gAlphaBlending = nullptr;
//**** Add each new blend state variable here and add externs in the Common.h file ****//

// Rasterizer states affect how triangles are drawn
ID3D11RasterizerState* gCullBackState = nullptr;
ID3D11RasterizerState* gCullFrontState = nullptr;
ID3D11RasterizerState* gCullNoneState = nullptr;

// Depth-stencil states allow us change how the depth buffer is used
ID3D11DepthStencilState* gUseDepthBufferState = nullptr;
ID3D11DepthStencilState* gDepthReadOnlyState = nullptr;



// Create all the states used in this app, returns true on success
bool CreateStates()
{
	//--------------------------------------------------------------------------------------
	// Texture Samplers
	//--------------------------------------------------------------------------------------
	// Each block of code creates a filtering mode. Copy a block and adjust values to add another mode. See texturing lab for details
	D3D11_SAMPLER_DESC samplerDesc = {};

	////-------- Trilinear filtering --------////

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Trilinear filtering
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap addressing mode for texture coordinates outside 0->1
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
	samplerDesc.MaxAnisotropy = 1;                        // Number of samples used if using anisotropic filtering, more is better but max value depends on GPU

	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Controls how much mip-mapping can be used. These settings are full mip-mapping, the usual values
	samplerDesc.MinLOD = 0;                 // --"--

	// Then create a DirectX object for your description that can be used by a shader
	if (FAILED(gD3DDevice->CreateSamplerState(&samplerDesc, &gTrilinearSampler)))
	{
		gLastError = "Error creating trilinear sampler";
		return false;
	}



  //--------------------------------------------------------------------------------------
	// Rasterizer States
	//--------------------------------------------------------------------------------------
	// Rasterizer states adjust how triangles are filled in and when they are shown
	// Each block of code creates a rasterizer state. Copy a block and adjust values to add another mode
	D3D11_RASTERIZER_DESC rasterizerDesc = {};

	////-------- Back face culling --------////
	// This is the usual mode - don't show inside faces of objects

  rasterizerDesc.FillMode              = D3D11_FILL_SOLID; // Can also set this to wireframe - experiment if you wish
  rasterizerDesc.CullMode              = D3D11_CULL_BACK;  // Setting that decides whether the "front" and "back" side of each
                                                            // triangle is drawn or not. Culling back is normal, but transparent objects
                                                            // usually make more sense if you show both sides (cull none)
  rasterizerDesc.DepthClipEnable       = TRUE; // Advanced setting - only used in rare cases

  // Create a DirectX object for your description above that can be used by a shader
  if (FAILED(gD3DDevice->CreateRasterizerState(&rasterizerDesc, &gCullBackState)))
  {
    gLastError = "Error creating cull-back state";
    return false;
  }


	
	////-------- Front face culling --------////
	// This is an unusual mode - it shows inside faces only so the model looks inside-out

  rasterizerDesc.FillMode              = D3D11_FILL_SOLID; // Can also set this to wireframe - experiment if you wish
  rasterizerDesc.CullMode              = D3D11_CULL_FRONT;  // Setting that decides whether the "front" and "back" side of each
                                                            // triangle is drawn or not. Culling back is normal, but transparent objects
                                                            // usually make more sense if you show both sides (cull none)
  rasterizerDesc.DepthClipEnable       = TRUE; // Advanced setting - only used in rare cases

  // Create a DirectX object for your description above that can be used by a shader
  if (FAILED(gD3DDevice->CreateRasterizerState(&rasterizerDesc, &gCullFrontState)))
  {
    gLastError = "Error creating cull-back state";
    return false;
  }

	
	
  ////-------- No culling --------////
  // Used for transparent or flat objects - show both sides of faces

  rasterizerDesc.FillMode              = D3D11_FILL_SOLID;
  rasterizerDesc.CullMode              = D3D11_CULL_NONE;
  rasterizerDesc.DepthClipEnable       = TRUE; // Advanced setting - only used in rare cases

  // Create a DirectX object for your description above that can be used by a shader
  if (FAILED(gD3DDevice->CreateRasterizerState(&rasterizerDesc, &gCullNoneState)))
  {
    gLastError = "Error creating cull-none state";
    return false;
  }
	
	
	
//--------------------------------------------------------------------------------------
	// Blending States
	//--------------------------------------------------------------------------------------
	// Each block of code creates a filtering mode. Copy a block and adjust values to add another mode. See blending lab for details
	D3D11_BLEND_DESC blendDesc = {};

	////-------- Blending Off State --------////

  blendDesc.RenderTarget[0].BlendEnable = FALSE;              // Disable blending
  blendDesc.RenderTarget[0].SrcBlend  = D3D11_BLEND_ONE;      // How to blend the source (texture colour) - See lab notes
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;     // How to blend the destination (colour already on screen) - See lab notes
  blendDesc.RenderTarget[0].BlendOp   = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD, leave this alone.

  //** Leave the following settings alone, they are used only in highly unusual cases
  //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
  blendDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  // Then create a DirectX object for your description that can be used by a shader
  if (FAILED(gD3DDevice->CreateBlendState(&blendDesc, &gNoBlendState)))
  {
    gLastError = "Error creating no-blend state";
    return false;
  }



  ////**** Create more blend states here, just copy and adjust the code above ****////

  blendDesc.RenderTarget[0].BlendEnable = TRUE;              // additive blending
  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;      // How to blend the source (texture colour) - See lab notes
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;     // How to blend the destination (colour already on screen) - See lab notes
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD, leave this alone.

  //** Leave the following settings alone, they are used only in highly unusual cases
  //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  // Then create a DirectX object for your description that can be used by a shader
  if (FAILED(gD3DDevice->CreateBlendState(&blendDesc, &gAdditiveBlending)))
  {
	  gLastError = "Error creating no-blend state";
	  return false;
  }

  //multiplicative blending -----------------------------------------------------------------------------------------------------------
  
  
  blendDesc.RenderTarget[0].BlendEnable = TRUE;              // additive blending
  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;      // How to blend the source (texture colour) - See lab notes
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;     // How to blend the destination (colour already on screen) - See lab notes
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD, leave this alone.

  //** Leave the following settings alone, they are used only in highly unusual cases
  //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  // Then create a DirectX object for your description that can be used by a shader
  if (FAILED(gD3DDevice->CreateBlendState(&blendDesc, &gMultiplicativeBlending)))
  {
	  gLastError = "Error creating no-blend state";
	  return false;
  }



  //alpha blending -----------------------------------------------------------------------------------------------------------------------------

  blendDesc.RenderTarget[0].BlendEnable = TRUE;              // alpha blending
  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;      // How to blend the source (texture colour) - See lab notes
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;     // How to blend the destination (colour already on screen) - See lab notes
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;   // How to combine the above two, almost always ADD, leave this alone.

  //** Leave the following settings alone, they are used only in highly unusual cases
  //** Despite the word "Alpha" in the variable names, these are not the settings used for alpha blending
  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  // Then create a DirectX object for your description that can be used by a shader
  if (FAILED(gD3DDevice->CreateBlendState(&blendDesc, &gAlphaBlending)))
  {
	  gLastError = "Error creating no-blend state";
	  return false;
  }

  
  //--------------------------------------------------------------------------------------
	// Depth-Stencil States
	//--------------------------------------------------------------------------------------
	// Depth-stencil states adjust how the depth and stencil buffers are used. The stencil buffer is rarely used so 
	// these states are most often used to switch the depth buffer on and off. See depth buffers lab for details
	// Each block of code creates a rasterizer state. Copy a block and adjust values to add another mode
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	////-------- Enable depth buffer --------////

  depthStencilDesc.DepthEnable      = TRUE;
  depthStencilDesc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc        = D3D11_COMPARISON_LESS;
  depthStencilDesc.StencilEnable    = FALSE;

  // Create a DirectX object for the description above that can be used by a shader
  if (FAILED(gD3DDevice->CreateDepthStencilState(&depthStencilDesc, &gUseDepthBufferState)))
  {
    gLastError = "Error creating use-depth-buffer state";
    return false;
  }

	
  ////-------- Enable depth buffer reads only --------////
  // Disables writing to depth buffer - used for transparent objects because they should not be entered in the buffer but do need to check if they are behind something

  depthStencilDesc.DepthEnable      = TRUE;
  depthStencilDesc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable writing to depth buffer
  depthStencilDesc.DepthFunc        = D3D11_COMPARISON_LESS;
  depthStencilDesc.StencilEnable    = FALSE;

  // Create a DirectX object for the description above that can be used by a shader
  if (FAILED(gD3DDevice->CreateDepthStencilState(&depthStencilDesc, &gDepthReadOnlyState)))
  {
    gLastError = "Error creating depth-read-only state";
    return false;
  }

  return true;
}


// Release DirectX state objects
void ReleaseStates()
{
  if (gUseDepthBufferState) gUseDepthBufferState->Release();
  if (gDepthReadOnlyState)  gDepthReadOnlyState->Release();
  if (gCullBackState)       gCullBackState->Release();
  if (gCullFrontState)      gCullFrontState->Release();
  if (gCullNoneState)       gCullNoneState->Release();
  if (gNoBlendState)        gNoBlendState->Release();
  if (gTrilinearSampler)    gTrilinearSampler->Release();
}
