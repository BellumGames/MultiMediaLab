
#include <d3d9.h>
#include <d3dx9.h>

//Make sure the libraries d3d9.lib and d3dx9.lib are linked


//Add this source to your project

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
D3DXIMAGE_INFO Info;
IDirect3DSurface9 *Surface=NULL;
IDirect3DSurface9 *Backbuffer=NULL;
RECT source;
RECT dest;
POINT updatedest;
int depx=0;
int depy=0;
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And 
    // we request a back buffer format that matches the current desktop display 
    // format.
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the Direct3D device. Here we are using the default adapter (most
    // systems only have one, unless they have multiple graphics hardware cards
    // installed) and requesting the HAL (which is saying we want the hardware
    // device rather than a software one). Software vertex processing is 
    // specified since we know it will work on all cards. On cards that support 
    // hardware vertex processing, though, we would see a big performance gain 
    // by specifying hardware vertex processing.
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
			return E_FAIL;

    }
	D3DXGetImageInfoFromFile("D:\\234-2\\Laborator2\\img3.jpg",&Info);
	
	g_pd3dDevice->CreateOffscreenPlainSurface(Info.Width,Info.Height,Info.Format,D3DPOOL_SYSTEMMEM,&Surface,NULL);

	
	
     return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pd3dDevice != NULL) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL)
        g_pD3D->Release();
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    if( NULL == g_pd3dDevice )
        return;
	
    // Clear the backbuffer to a blue color
	
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );
	
    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Rendering of scene objects can happen here
		
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Tutorial", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Tutorial", "D3D Tutorial 01: CreateDevice", 
		WS_OVERLAPPEDWINDOW, 0, 0, 1450, 1450,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
        // Show the window
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );

		MSG mssg;
		PeekMessage( &mssg, NULL, 0, 0, PM_NOREMOVE);
		// run till completed
		while (mssg.message!=WM_QUIT) 
		{
			// is there a message to process?
			if (PeekMessage( &mssg, NULL, 0, 0, PM_REMOVE))
			{
				
				TranslateMessage(&mssg);
				DispatchMessage(&mssg);
			} 
			else 
			{
	           	//No message to process?
				// Then do your game stuff here
    			 Render();
     		}
		}
    }

    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}