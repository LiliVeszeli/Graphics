#include "Scene.h"
#include "Shader.h"
#include "Common.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way

ID3D11InputLayout* gVertexLayout = nullptr;
ID3D11Buffer*      gVertexBuffer = nullptr;



//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

//****
// An x, y, z could be a point or a vector. It is usual to use a structures called "Vector"
// to hold either kind of data and rely on variable names or context to make it clear what
// kind of data we have. Later we will see how to distinguish them properly.
struct Vector3
{
    float x, y, z;
};

// The content of a single vertex in the geometry that we will render. Currently it just stores
// the position of the vertex (x,y,z). However, we can also store other information for each
// vertex (for example, a colour for the vertex) so we use a structure.
struct SimpleVertex
{
    Vector3 Position; 
};

//****


//--------------------------------------------------------------------------------------
// Scene Geometry
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitScene()
{
    //****
    // Define the input layout. Go to the top of the file and look at the SimpleVertex structure, then return here. Each vertex in 
    // our geometry will contain the data in the SimpleVertex structure. In this example just the (x,y,z) position for each vertex
    //
    // We need to tell D3D about the contents of that structure (it is not going to read our C++ code). The input layout is a special
    // structure to describe the vertex data. There is one row in the layout array below for each data element in our structure.
    // So the row below describes the fact that each vertex in our geometry has a position which is made of three 32-bit floats.
    //
    // D3D refers to most data as though it was colour data, which is why the constant below is ...R32G32B32... rather than ...X32Y32Z32...
    // We will come across vertices with more complex data soon, and the need for this structure will become clearer.
    //
    D3D11_INPUT_ELEMENT_DESC vertexLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = sizeof(vertexLayout) / sizeof(vertexLayout[0]); // This just gives a count of rows in the array above

    // These lines convert the array above into an object (gVertexLayout) we use when rendering
    auto shaderSignature = CreateSignatureForVertexLayout(vertexLayout, numElements);
    gD3DDevice->CreateInputLayout(vertexLayout, numElements, shaderSignature->GetBufferPointer(), shaderSignature->GetBufferSize(), &gVertexLayout);
    shaderSignature->Release();


    // Next create an array of vertices for our geometry. We use the structure we saw above. These 3 vertices define a single triangle.
    // Note that this array is created in "CPU memory", it is not available to Direct3D or the GPU in this C++ array form. This is just
    // a convenient way to prepare the data for now.
    //
    SimpleVertex vertices[] =
    {
        Vector3{ 0.0f,  0.5f, 0.5f},
        Vector3{ 0.0f, -0.5f, 0.5f},
        Vector3{-0.5f, -0.5f, 0.5f},

	
		Vector3{ 0.9f, 0.5f, 0.5f },
		Vector3{ 0.9f, -0.5f, 0.5f },
		Vector3{ 0.4f, -0.5f, 0.5f },
    };

	
	
	
    int numVertices = sizeof(vertices) / sizeof(SimpleVertex); // A count of the number of vertices in the array above
	//int numVertices2 = sizeof(vertices2) / sizeof(SimpleVertex);

    // Finally, create a "vertex buffer", an area of Direct3D controlled memory that will really hold the vertex data above. This buffer
    // is likely to be created in "GPU memory" for performance reasons. The vertex buffer will be initialised with the above array, but
    // once created we cannot change it contents directly - it is under DirectX control. 
    // A key point to understand is the separation between C++ data like the array above and DirectX data like this vertex buffer
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage     = D3D11_USAGE_DEFAULT;      // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = numVertices * sizeof(SimpleVertex); // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA initData; // Fill the new vertex buffer with the array above as initial data
    initData.pSysMem = vertices;
	//initData.pSysMem = vertices2;
    if (FAILED(gD3DDevice->CreateBuffer(&bufferDesc, &initData, &gVertexBuffer)))
    {
        return false;
    }

    //****

    return true;
}


// Release the geometry resources created above
void ReleaseScene()
{
    if (gVertexBuffer)  gVertexBuffer->Release();
    if (gVertexLayout)  gVertexLayout->Release();
}


//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Called once a frame, from the loop in Main.cpp
void RenderScene()
{
    //****
    // Once per frame tasks

    // Set the "back buffer" as the target for rendering. The "back buffer" is an off-screen viewport. When
    // we have finished drawing to the back buffer it sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, nullptr);

    // Clear the back buffer to a fixed colour
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red, green, blue, alpha
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, ClearColor);

    // Setup the viewport - defines which part of the window we will render to, almost always the whole window
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(gViewportWidth);
    vp.Height = static_cast<FLOAT>(gViewportHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    //****


    //****
    // Scene rendering tasks
    //
    // After the preperation initialising the geometry in the function above, these are
    // the basic steps to actually draw something in Direct3D:
    //    1) select a vertex buffer,  2) indicate layout and topology of vertex buffer,  3) select shaders,  4) draw

    // 1) Select a vertex buffer - tell D3D that the next geometry data it will be drawing is in the vertex buffer created above
    UINT stride = sizeof(SimpleVertex); // Size of a single vertex in the buffer
    UINT offset = 0;
    gD3DContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &stride, &offset);


    // 2a) Indicate the layout of the vertex buffer
    gD3DContext->IASetInputLayout(gVertexLayout);
    
    // 2b) Also indicate the primitive topology of the buffer. Our buffer holds a triangle list, and each triplet of vertices
    // should be connected into a triangle. There are other topologies and we will see them shortly.
    gD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // 3) Select which shaders to use when rendering
    gD3DContext->VSSetShader(gSimpleVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSimplePixelShader,  nullptr, 0);


    // 4) Draw 3 vertices, starting at vertex 0. This will draw a triangle using the vertex data and shaders selected
    gD3DContext->Draw(6, 0);

    //****


    // When drawing to the off-screen back buffer is complete, "present" the image to the front buffer (the screen)
    gSwapChain->Present(0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
    // No scene update in this tutorial
}

