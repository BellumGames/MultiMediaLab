#include <Windows.h>
#include <d3dx9.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             directD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       direct3Device9 = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 vertexBuffer = NULL;
IDirect3DIndexBuffer9* IB;

//-----------------------------------------------------------------------------
// Custom vertex
//-----------------------------------------------------------------------------

struct CUSTOMVERTEX
{
    FLOAT x, y, z; //Position
    DWORD color; //Colour
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
    // Create the D3D object.
    if (NULL == (directD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if (FAILED(directD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &direct3Device9)))
    {
        if (FAILED(directD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &d3dpp, &direct3Device9)))
            return E_FAIL;
    }

    // Turn on the zbuffer
    direct3Device9->SetRenderState(D3DRS_ZENABLE, TRUE);

    // Turn on ambient lighting 
    direct3Device9->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

    // Turn off culling, so we see the front and back of the triangle
    direct3Device9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Turn off D3D lighting, since we are providing our own vertex colors
    direct3Device9->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the vertex buffer
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Initialize three vertices for rendering a triangle
    CUSTOMVERTEX g_Vertices[] =
    {
        { -1.0f,-1.0f, 0.0f, 0xffffff00, },
        {  1.0f,-1.0f, 0.0f, 0xffffff00, },
        {  1.0f, 1.0f, 0.0f, 0xffffff00, },
        { -1.0f,1.0f, 0.0f, 0xffffff00, },
    };

    // Create the vertex buffer.
    if (FAILED(direct3Device9->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &vertexBuffer, NULL)))
    {
        return E_FAIL;
    }

    // Create the index buffer
    if (FAILED(direct3Device9->CreateIndexBuffer(4 * sizeof(CUSTOMVERTEX),
        0, D3DFMT_INDEX16,
        D3DPOOL_DEFAULT, &IB, NULL)))
    {
        return E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if (FAILED(vertexBuffer->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0)))
        return E_FAIL;
    memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
    vertexBuffer->Unlock();

    WORD indices[] = { 0,1,2,3 };

    VOID* pIndices;
    if (FAILED(IB->Lock(0, sizeof(g_Vertices), (void**)&pIndices, 0)))
    {
        return E_FAIL;
    }
    memcpy(pIndices, indices, sizeof(indices));
    IB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if (vertexBuffer)
        vertexBuffer->Release();

    if (direct3Device9 != NULL)
        direct3Device9->Release();

    if (directD3D != NULL)
        directD3D->Release();
}

void SetupWorldMatrix()
{
    // For our world matrix, we will just leave it as the identity
    D3DXMATRIX g_Transform;
    D3DXMatrixIdentity(&g_Transform);
    direct3Device9->SetTransform(D3DTS_WORLD, &g_Transform);
}

void SetupViewMatrix()
{
    D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    direct3Device9->SetTransform(D3DTS_VIEW, &matView);
}

void SetupProjectionMatrix()
{
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
    direct3Device9->SetTransform(D3DTS_PROJECTION, &matProj);
}

//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    SetupWorldMatrix();

    SetupViewMatrix();

    SetupProjectionMatrix();
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer and the zbuffer
    direct3Device9->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

    // Begin the scene
    if (SUCCEEDED(direct3Device9->BeginScene()))
    {
        // Setup the world, view, and projection matrices
        SetupMatrices();

        direct3Device9->SetStreamSource(0, vertexBuffer, 0, sizeof(CUSTOMVERTEX));
        direct3Device9->SetFVF(D3DFVF_CUSTOMVERTEX);

        direct3Device9->SetIndices(IB);
        direct3Device9->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, 4, 0, 2);


        //DeviceSetIndex
        //DrawIndexPrimitive
        //direct3Device9->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
        //deseneaza puncte
        //direct3Device9->DrawPrimitive( D3DPT_POINTLIST, 0, 3 );
        //deseneaza linii
        //direct3Device9->DrawPrimitive( D3DPT_LINESTRIP, 0, 3 );
        // End the scene
        direct3Device9->EndScene();
    }

    // Present the backbuffer contents to the display
    direct3Device9->Present(NULL, NULL, NULL, NULL);
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Tutorial", NULL };
    RegisterClassEx(&wc);

    // Create the application's window
    HWND hWnd = CreateWindow("D3D Tutorial", "Vertex Buffer",
        WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
        GetDesktopWindow(), NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (SUCCEEDED(InitD3D(hWnd)))
    {
        // Create the scene geometry
        if (SUCCEEDED(InitGeometry()))
        {
            // Show the window
            ShowWindow(hWnd, SW_SHOWDEFAULT);
            UpdateWindow(hWnd);

            // Enter the message loop
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            while (msg.message != WM_QUIT)
            {
                if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass("D3D Tutorial", wc.hInstance);
    return 0;
}

//TODO: IndexBuffer in loc de VertexBuffer