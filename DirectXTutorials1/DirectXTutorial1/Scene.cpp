#include "Scene.h"
#include "Common.h"

//--------------------------------------------------------------------------------------
// Render the scene
//--------------------------------------------------------------------------------------
void RenderScene()
{
    //****
    // Called once a frame, from the loop in Main.cpp
    // All this program does is clear the screen to blue

    // Set the "back buffer" as the target for rendering. The "back buffer" is an off-screen viewport. When
    // we have finished drawing to the back buffer it is sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, nullptr);

    // Clear the render target (which is currently the back buffer) to a fixed colour
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, ClearColor);

    // After we've finished drawing to the off-screen back buffer, we "present" it to the front buffer (the monitor)
    gSwapChain->Present(0, 0);

    //****
}


//--------------------------------------------------------------------------------------
// Update the scene
//--------------------------------------------------------------------------------------

// frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
    // No scene update in this tutorial
}

