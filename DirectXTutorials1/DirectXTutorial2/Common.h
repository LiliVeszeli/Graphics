//--------------------------------------------------------------------------------------
// Commonly used definitions across entire project
//--------------------------------------------------------------------------------------
#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_

#include <windows.h>
#include <d3d11.h>
#include <vector>

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Make global Variables from various files available to other files. "extern" means
// this variable is defined in another file somewhere. We should use classes and avoid
// use of globals, but done this way to keep code simpler so the DirectX content is
// clearer. However, try to architect your own code in a better way.

// Windows variables
extern HWND gHWnd;

// Viewport size
extern int gViewportWidth;
extern int gViewportHeight;

// Important DirectX variables
extern ID3D11Device*           gD3DDevice;
extern ID3D11DeviceContext*    gD3DContext;
extern IDXGISwapChain*         gSwapChain;
extern ID3D11RenderTargetView* gBackBufferRenderTarget;

// Shaders - we won't look at shaders until later in the module, but they are needed to
// render anything. Very simple shaders are used in this tutorial
extern ID3D11PixelShader*  gSimplePixelShader;
extern ID3D11VertexShader* gSimpleVertexShader;


#endif //_COMMON_H_INCLUDED_
