//--------------------------------------------------------------------------------------
// Initialisation of Direct3D and main resources (textures, shaders etc.)
//--------------------------------------------------------------------------------------

#include "Direct3DSetup.h"
#include "Shader.h"
#include "Common.h"
#include <d3d11.h>
#include <vector>


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

// The main Direct3D (D3D) variables
ID3D11Device*        gD3DDevice  = nullptr; // D3D device for overall features
ID3D11DeviceContext* gD3DContext = nullptr; // D3D context for specific rendering tasks

// Swap chain and back buffer
IDXGISwapChain*         gSwapChain = nullptr;
ID3D11RenderTargetView* gBackBufferRenderTarget = nullptr;

// Shaders - we won't look at shaders until later in the module, but they are needed to
// render anything. Very simple shaders are used in this tutorial
ID3D11PixelShader*  gSimplePixelShader = nullptr;
ID3D11VertexShader* gSimpleVertexShader = nullptr;


//--------------------------------------------------------------------------------------
// Load shaders
//--------------------------------------------------------------------------------------

bool LoadShaders()
{
    // Shaders - we won't look at shaders until later in the module, but they are needed to render anything.
    //
    // Shaders must be added to the Visual Studio project to be compiled, they use the extension ".hlsl".
    // To load them for use, include them here without the extension. Use the correct function for each.
    // Ensure you release the shaders in the ShutdownDirect3D function below
    gSimpleVertexShader = LoadVertexShader("transform3Dto2D_vs");
    gSimplePixelShader  = LoadPixelShader ("simple_ps"); // Note how the shaders are named to show what type they are

    if (gSimpleVertexShader == nullptr ||
        gSimplePixelShader  == nullptr)
    {
        gLastError = "Error loading shaders";
        return false;
    }

    return true;
}



//--------------------------------------------------------------------------------------
// Initialise / uninitialise Direct3D
//--------------------------------------------------------------------------------------
// Returns false on failure
bool InitDirect3D()
{
    // Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
    // the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
    HRESULT hr = S_OK;


    // Create a Direct3D device (i.e. initialise D3D) and create a swap-chain (create a back buffer to render to)
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.OutputWindow = gHWnd;                           // Target window
    sd.Windowed = TRUE;
    sd.BufferCount = 1;
    sd.BufferDesc.Width  = gViewportWidth;             // Target window size
    sd.BufferDesc.Height = gViewportHeight;            // --"--
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
    sd.BufferDesc.RefreshRate.Numerator   = 60;        // Refresh rate of monitor (provided as fraction = 60/1 here)
    sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;
    UINT debug = 0; // Set this to D3D11_CREATE_DEVICE_DEBUG to get more debugging information
    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, debug, 0, 0, D3D11_SDK_VERSION, &sd,
                                       &gSwapChain, &gD3DDevice, nullptr, &gD3DContext);
    if (FAILED(hr))
    {
        gLastError = "Error creating Direct3D device";
        return false;
    }


    // Get a "render target view" of back-buffer - standard behaviour
    ID3D11Texture2D* backBuffer;
    hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        gLastError = "Error creating swap chain";
        return false;
    }
    hr = gD3DDevice->CreateRenderTargetView(backBuffer, NULL, &gBackBufferRenderTarget);
    backBuffer->Release();
    if (FAILED(hr))
    {
        gLastError = "Error creating render target view";
        return false;
    }


    // Load the shaders required for the app
    if (!LoadShaders())
    {
        return false;
    }


    return true;
}


// Release the memory held by all objects created
void ShutdownDirect3D()
{
    // Release each Direct3D object to return resources to the system. Missing these out will cause memory
    // leaks. Check documentation to see which objects need to be released when adding new features in your
    // own projects.
    if (gSimplePixelShader)  gSimplePixelShader->Release();
    if (gSimpleVertexShader) gSimpleVertexShader->Release();
    if (gD3DContext)
    {
        gD3DContext->ClearState(); // This line is also needed to reset the GPU before shutting down DirectX
        gD3DContext->Release();
    }
    if (gBackBufferRenderTarget) gBackBufferRenderTarget->Release();
    if (gSwapChain)              gSwapChain->Release();
    if (gD3DDevice)              gD3DDevice->Release();
}


