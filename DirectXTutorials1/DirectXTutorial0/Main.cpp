//--------------------------------------------------------------------------------------
// Entry point for the application
// Window creation code
//--------------------------------------------------------------------------------------

#include <windows.h>
#include "input.h"


// Forward declarations of functions included in this code module
// Lets us use functions above their code. Without this we end up having to put less
// interesting functions first in the file.
BOOL             InitWindow(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Using classes would be better, but this module keeps code simple to better highlight
// the more important DirectX features. However, architect your own code in a better way.
HINSTANCE gHInst;
HWND      gHWnd;

// Viewport size (which determines the size of the window used for our app)
int gViewportWidth = 960;
int gViewportHeight = 800;



//--------------------------------------------------------------------------------------
// The entry function for a Windows application is called wWinMain
//--------------------------------------------------------------------------------------
// This is the equivalent of "main" in a console application

int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance); // This convenient macro stops Visual Studio issuing warnings...
    UNREFERENCED_PARAMETER(lpCmdLine);     // .. when we we don't use some parameters (which is not uncommon)
    

    //****
    // Initialise everything
    //****
    if (!InitWindow(hInstance, nCmdShow)) // Create a window to display the scene
    {
        return 0;
    }
    InitInput(); // Prepare TL-Engine style input functions


    //****
    // Main message loop - this is a Windows equivalent of the loop in a TL-Engine application
    //****
    MSG msg = {};
    while (msg.message != WM_QUIT) // Loop until a quit message arrives (when the window closes)
    {
        //****
        // Check for and deal with any window messages (input, window resizing, minimizing, etc.).
        // When there are no messages pending the window is idle and the DirectX rendering/update will occur
        // The actual message processing happens in the function WndProc below
        //****
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Deal with messages
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        else // No windows messages left so render & update our scene
        {
            //****
            // This tutorial doesn't draw anything, but normally we would render / update the scene here
            //****

            if (KeyHit(Key_Q))
            {
                DestroyWindow(gHWnd); // This will close the window and ultimately trigger a quit message that will exit this loop
            }
        }
    }

    return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Create a window to display our scene, returns false on failure.
//--------------------------------------------------------------------------------------
BOOL InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Get a stock icon to show on the taskbar for this program. We can draw/use our own icons of course.
    SHSTOCKICONINFO stockIcon;
    stockIcon.cbSize = sizeof(stockIcon);
    if (SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON, &stockIcon) != S_OK) // Returns false on failure
    {
        return false;
    }

    // Register window class. Defines various UI features of the window for our application.
    // We don't usually change much here, a couple of interesting parameters have comments
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;    // Which function deals with windows messages
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0; SIID_APPLICATION;
    wcex.hInstance = hInstance;
    wcex.hIcon = stockIcon.hIcon; // Which icon to use for the window
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // What cursor appears over the window
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"CO2409WindowClass";
    wcex.hIconSm = stockIcon.hIcon;
    if (!RegisterClassEx(&wcex)) // Returns false on failure
    {
        return false;
    }

    //****
    // Dimensions for the window, by using AdjustWindowRect, we ensure the *inside* of the window is the
    // size given (i.e. not including the borders, title bar etc.)
    //****
    RECT rc = { 0, 0, gViewportWidth, gViewportHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    //****
    // Create window, the second parameter is the text that appears in the title bar
    //****
    gHInst = hInstance;
    gHWnd = CreateWindow(L"CO2409WindowClass", L"Miau", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
    if (!gHWnd)
    {
        return false;
    }

    ShowWindow(gHWnd, nCmdShow);
    UpdateWindow(gHWnd);

    return TRUE;
}


// Deal with a message from Windows. There are very many possible messages, such as keyboard/mouse input, resizing
// or minimizing windows, the system shutting down etc. We only deal with messages that we are interested in
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT: // A necessary message to ensure the window content is displayed
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY: // Another necessary message to deal with the window being closed
        PostQuitMessage(0);
        break;


    //****
    // The WM_KEYXXXX messages report keyboard input to our window.
    // This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
    // so you don't need to change this code. Instead simply use KeyHit, KeyHeld etc.
    //****
    case WM_KEYDOWN:
        KeyDownEvent(static_cast<KeyCode>(wParam));
        break;

    case WM_KEYUP:
        KeyUpEvent(static_cast<KeyCode>(wParam));
        break;


    //****
    // The following WM_XXXX messages report mouse movement and button presses
    // Use KeyHit to get mouse buttons, GetMouseX, GetMouseY for its position
    //****
    case WM_MOUSEMOVE:
    {
        MouseMoveEvent(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_LBUTTONDOWN:
    {
        KeyDownEvent(Mouse_LButton);
        break;
    }
    case WM_LBUTTONUP:
    {
        KeyUpEvent(Mouse_LButton);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        KeyDownEvent(Mouse_RButton);
        break;
    }
    case WM_RBUTTONUP:
    {
        KeyUpEvent(Mouse_RButton);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        KeyDownEvent(Mouse_MButton);
        break;
    }
    case WM_MBUTTONUP:
    {
        KeyUpEvent(Mouse_MButton);
        break;
    }


    // Any messages we don't handle are passed back to Windows default handling
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
