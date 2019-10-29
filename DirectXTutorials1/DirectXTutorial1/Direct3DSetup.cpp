//--------------------------------------------------------------------------------------
// Initialisation of Direct3D and main resources
//--------------------------------------------------------------------------------------

#include "Direct3DSetup.h"
#include "Common.h"
#include <d3d11.h>


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

// The main Direct3D (D3D) variables
ID3D11Device*        gD3DDevice  = nullptr; // The D3D "device" manages overall features, like window access, setup / shutdown etc.
ID3D11DeviceContext* gD3DContext = nullptr; // The D3D "context" manages most rendering tasks, this is used a lot.

// Swap chain and back buffer - see lab worksheet notes.
IDXGISwapChain*         gSwapChain = nullptr;
ID3D11RenderTargetView* gBackBufferRenderTarget = nullptr;



//--------------------------------------------------------------------------------------
// Initialise / uninitialise Direct3D
//--------------------------------------------------------------------------------------
// Returns false on failure
bool InitDirect3D()
{
    // Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
    // the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
    HRESULT hr = S_OK;


    //****
    // Create a Direct3D device (i.e. initialise D3D), and create a swap-chain (create a back buffer to render to)
    // Key information is the size of the window, the pixel format (R8G8B8A8 - recall the lectures about colour formats),
    // the refresh rate of the monitor, the window we are targeting and whether we want to go full-screen.
    //
    // Other details are more rare to change (such as whether we want anti-aliasing)
    //
    //****
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.OutputWindow = gHWnd; // Target window
    sd.Windowed = TRUE;      // Whether to render in a window (TRUE) or go fullscreen (FALSE)
    sd.BufferCount = 1;
    sd.BufferDesc.Width  = gViewportWidth;             // Target window size
    sd.BufferDesc.Height = gViewportHeight;            // --"--
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
    sd.BufferDesc.RefreshRate.Numerator   = 60;        // Refresh rate of monitor (provided as fraction = 60/1 here)
    sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;
    UINT debug = 0;          // Set this to D3D11_CREATE_DEVICE_DEBUG to get more debugging information
    hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, debug, 0, 0, D3D11_SDK_VERSION, &sd,
                                       &gSwapChain, &gD3DDevice, nullptr, &gD3DContext);
    if (FAILED(hr))
    {
        return false;
    }


    // Get a "render target view" of back-buffer - this is an advanced topic. DirectX 11 can interpret memory in different
    // ways. Just as we can cast an "int" to a "char" in C++, DX11 can change the interepretation of a block of memory. For
    // example, it can consider a bitmap (texture) as a viewport if required.
    //
    // This code is getting the back buffer and interpreting it as a "Render Target" - something we can render to. This is
    // the normal interpretation for the back buffer, so this code almost always occurs in the standard D3D setup
    //
    ID3D11Texture2D* backBuffer;
    hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        return false;
    }
    hr = gD3DDevice->CreateRenderTargetView(backBuffer, NULL, &gBackBufferRenderTarget);
    backBuffer->Release();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}


// Release the memory held by all objects created
void ShutdownDirect3D()
{
    //****
    // Each Direct3D resource that has been created needs to be released to return resources to the system.
    // Missing these out will cause memory leaks. Check documentation to see which objects need to be released
    // when adding new features in your own projects.
    //****
    if (gD3DContext)
    {
        gD3DContext->ClearState(); // This line is also needed to reset the GPU before shutting down DirectX
        gD3DContext->Release();
    }
    if (gBackBufferRenderTarget) gBackBufferRenderTarget->Release();
    if (gSwapChain)              gSwapChain->Release();
    if (gD3DDevice)              gD3DDevice->Release();
}



