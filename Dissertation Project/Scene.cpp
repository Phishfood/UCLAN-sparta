#include "Scene.h"

int CScene::mViewportWidth, CScene::mViewportHeight;

CScene::CScene(void)
{
	//set counters to 0
	miNumObjects = 0;
	miNumLights = 0;
	miNumMaps = 0;

	//set the ambient colour for the scene
	AmbientColour = D3DXVECTOR3( 0.2f, 0.2f, 0.2f );

	//set the controlled object to the mirror
	mControlObject = 0;

	FontColour = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );

	mb_showWallIM = false;
}

CScene::~CScene(void)
{
}

//--------------------------------------------------------------------------------------
// Scene Setup / Update / Rendering
//--------------------------------------------------------------------------------------

// Create / load the camera, models and textures for the scene
bool CScene::InitScene()
{
	//////////////////
	// Create camera

	Camera = new CCamera();
	Camera->SetPosition( D3DXVECTOR3(-15, 20,-40) );
	Camera->SetRotation( D3DXVECTOR3(ToRadians(13.0f), ToRadians(18.0f), 0.0f) ); // ToRadians is a new helper function to convert degrees to radians
	Camera->SetNearClip( 1.0f );

	//load all textures and maps
	miNumMaps = 0;

	mc_map = new MapSquare("bob");

	FontDesc.Height	= 20;
	FontDesc.Width	= 0;
	FontDesc.Weight	= 0;
	FontDesc.MipLevels	= 1;
	FontDesc.Italic	= false;
	FontDesc.CharSet = OUT_DEFAULT_PRECIS;
	FontDesc.Quality = DEFAULT_QUALITY;
	FontDesc.PitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;

	strcpy_s( FontDesc.FaceName, "Arial" );

	D3DX10CreateFontIndirect( mpd3dDevice, &FontDesc, &md_Font );

	//caching these locally - does mean more coupling, but seems better than fetching an unchanging variable repeatedly.
	mi_mapHeight = mc_map->GetHeight();
	mi_mapWidth = mc_map->GetWidth();

	mi_numSquares = 0;

	mp_mapModel = new CModel( D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), 0.1f );
	mp_mapModel->Load( "Cube.x", mTechniques[0], false );

	for (int y = 0; y < mi_mapWidth; y++)
	{
		for (int x = 0; x < mi_mapHeight; x++)
		{

			mp_mapModel->SetPosition( D3DXVECTOR3(x,0.0f,y) );
			mp_mapModel->UpdateMatrix();
			md_mapMatrix[mi_numSquares] = mp_mapModel->GetWorldMatrix();

			//colour the squares based on their type
			switch( mc_map->GetCost(x,y) )
			{
				case 0: //wall 
					md_mapColours[mi_numSquares] =  D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); //black
					break;
				case 1: //open
					md_mapColours[mi_numSquares] =  D3DXVECTOR3( 1.0f, 1.0f, 1.0f ); //white
					break;
				case 2: //player 1
					md_mapColours[mi_numSquares] =  D3DXVECTOR3( 1.0f, 0.0f, 0.0f ); //red 
					break;
				case 3://player 2
					md_mapColours[mi_numSquares] =  D3DXVECTOR3( 0.0f, 1.0f, 0.0f );//green
			}

			mi_numSquares++;
			
		}
	}
	return true;
}


// Update the scene - move/rotate each model and the camera, then update their matrices
void CScene::UpdateScene( float frameTime )
{
	//Show Framerate
	char caption[100];
	char buffer[6];
	
	//copy in base text
	strcpy_s( caption,  "Final Project - Project SPARTA - FPS:");
	
	//calculate FPS
	int FPS = int ( 1.0f / frameTime );
	
	//convert to string in base 10 and add to caption
	_itoa_s( FPS,buffer,10 );
	strcat_s( caption, buffer );
	
	//add in object and light counts as above
	strcat_s( caption, " O: " );
	_itoa_s( miNumObjects, buffer, 10 );
	strcat_s( caption, buffer );

	strcat_s( caption, " L: " );
	_itoa_s( miNumLights, buffer, 10 );
	strcat_s( caption, buffer );

	//pass the new caption to the window
	SetWindowTextA( HWnd, caption );

	// Control camera position and update its matrices (view matrix, projection matrix) each frame
	// Don't be deceived into thinking that this is a new method to control models - the same code we used previously is in the camera class
	Camera->Control( frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );
	Camera->UpdateMatrices();
	

	//update all the objects, including calculating the matrix
	for(int i = 0; i < miNumObjects; i++)
	{
		mpObjects[i]->Update(frameTime);
	}

	//same for the lights
	for(int i = 0; i < miNumLights; i++)
	{
		mpLights[i]->Update(frameTime);
	}


	// Allow user to quit with escape key
	if (KeyHit( Key_Escape )) 
	{
		DestroyWindow( HWnd );
	}

	if (KeyHit( Key_L ) )
	{
		mb_showWallIM = !mb_showWallIM;
	}
}

//function to pass data to the shaders and render the object.
void CScene::DrawObject(int i, bool mirror)
{
	//first the wiggle value
	float wiggle =  mpObjects[i]->GetWiggle();
	dxWiggle->SetRawValue( &wiggle, 0, 4);

	//outline thickness
	float thickness = mpObjects[i]->GetOutlineThickness();
	dxOutlineThickness->SetRawValue( &thickness, 0, 4);

	//pass the matrix
	WorldMatrixVar->SetMatrix( (float*)mpObjects[i]->GetModel()->GetWorldMatrix() );
	
	//pass the textures and maps, if there are any. 
	if( mpObjects[i]->GetTexture() != nullptr )
	{
		DiffuseMapVar->SetResource( mpObjects[i]->GetTexture() );
	}
	if( mpObjects[i]->GetNormalMap() != nullptr )
	{
		NormalMapVar->SetResource( mpObjects[i]->GetNormalMap() );
	}

	//pass the model colour - used for untextured models, texture colour change and outline colour in cell shading
	ModelColourVar->SetRawValue( mpObjects[i]->GetColourV(), 0, 12 );
	
	//if the object is lit, pass over the 12 closest lights. 
	if( mpObjects[i]->IsLit() )
	{
		SetLights( mpObjects[i]->GetModel()->GetPosition(), mpLights, miNumLights);
	}

	// choose how to render the model based on where we are rendering it. 
	if( mirror )
	{
		mpObjects[i]->RenderMirror();
	}
	else
	{
		mpObjects[i]->Render();
		
	}
}

// function to render everything in the scene
void CScene::DrawAllObjects(bool mirror)
{
	//
	// Solid objects
	//
	//start rendering AFTER the mirror.
	for(int i = 1; i < miNumObjects; i++)
	{
		if( !mpObjects[i]->IsTransparent() )
		{
			DrawObject(i, mirror);
		}
	}

	//
	// Light objects
	//


	for(int i = 0; i < miNumLights; i++)
	{
		WorldMatrixVar->SetMatrix( (float*)mpLights[i]->GetModel()->GetWorldMatrix() );
		D3DXVECTOR3 mColour =  mpLights[i]->GetColourV();
		ModelColourVar->SetRawValue(mColour, 0, 12 );
		if( mirror )
		{
			mpLights[i]->GetModel()->Render( mTechniquesMirror[0] );
		}
		else
		{
			mpLights[i]->GetModel()->Render( mTechniques[0] );
		}
	}

	//
	// Transparent objects
	//

	for(int i = 1; i < miNumObjects; i++)
	{
		if( mpObjects[i]->IsTransparent() )
		{
			DrawObject(i, mirror);
		}
	}
}

// Render everything in the scene
void CScene::RenderScene()
{
	// Clear the back buffer - before drawing the geometry clear the entire window to a fixed colour
	float ClearColor[4] = { 0.2f, 0.2f, 0.3f, 1.0f }; // Good idea to match background to ambient colour
	mpd3dDevice->ClearRenderTargetView( RenderTargetView, ClearColor );
	mpd3dDevice->ClearDepthStencilView( DepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL , 1.0f, 0 ); // Clear the depth buffer too


	// Pass the camera's matrices to the vertex shader
	ViewMatrixVar->SetMatrix( (float*)&Camera->GetViewMatrix() );
	ProjMatrixVar->SetMatrix( (float*)&Camera->GetProjectionMatrix() );

	//pass the camera position
	dxCameraPos->SetRawValue( D3DXVECTOR3( Camera->GetPosition()), 0, 12);

	//pass the lighting colours
	dxAmbientColour->SetRawValue( AmbientColour, 0, 12  );

	//---------------------------
	// Render each model
	
	//DrawAllObjects(false);

	mp_mapModel->SendGeometry();
	mp_mapModel->SetTechnique(mTechniques[0]);

	//85 fps this way - 20% improvement over the old way. 
	for(int i = 0; i < mi_numSquares; i++)
	{
		WorldMatrixVar->SetMatrix( (float*) md_mapMatrix[i] );
		ModelColourVar->SetRawValue( md_mapColours[i], 0, 12 );
		mp_mapModel->QuickRender();
	}

	//this won't work yet. 
	/*
	D3DXVECTOR3 WallColour = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 FloorColour = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	D3DXVECTOR3 P1Colour = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	D3DXVECTOR3 P2Colour = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	ModelColourVar->SetRawValue(  WallColour, 0, 12 );
	for(int i = 0; i < mi_numSquares; i++)
	{
		WorldMatrixVar->SetMatrix( (float*) md_mapMatrix[i] );
		mp_mapModel->QuickRender();
	}

	ModelColourVar->SetRawValue(  FloorColour, 0, 12 );
	for(int i = 0; i < mi_numSquares; i++)
	{
		WorldMatrixVar->SetMatrix( (float*) md_mapMatrix[i] );
		ModelColourVar->SetRawValue( md_mapColours[i], 0, 12 );
	}

	ModelColourVar->SetRawValue(  P1Colour, 0, 12 );
	for(int i = 0; i < mi_numSquares; i++)
	{
		WorldMatrixVar->SetMatrix( (float*) md_mapMatrix[i] );
		ModelColourVar->SetRawValue( md_mapColours[i], 0, 12 );
	}

	ModelColourVar->SetRawValue(  P2Colour, 0, 12 );
	for(int i = 0; i < mi_numSquares; i++)
	{
		WorldMatrixVar->SetMatrix( (float*) md_mapMatrix[i] );
		ModelColourVar->SetRawValue( md_mapColours[i], 0, 12 );
	}
	*/

	FontRect.bottom = 0;
	FontRect.right = 0;

	if( mb_showWallIM )
	{
		if( md_Font )
		{
			D3DXVECTOR2* pixel = new D3DXVECTOR2;
			for(int i = 0; i < mi_numSquares; i++)
			{
				
				D3DXVECTOR3 world = D3DXVECTOR3(md_mapMatrix[i]._41, md_mapMatrix[i]._42, md_mapMatrix[i]._43);
				Camera->PixelFromWorldPt(pixel, world);
				FontRect.top = pixel->y;
				FontRect.left = pixel->x;
				char temp[5];
				_itoa_s( mc_map->GetWallI(i/mi_mapWidth,i%mi_mapWidth), temp, 10 );
				md_Font->DrawTextA( 0, temp , -1, &FontRect, DT_NOCLIP, FontColour );
			}
		}

	}

	//---------------------------
	// Display the Scene

	// After we've finished drawing to the off-screen back buffer, we "present" it to the front buffer (the screen)
	SwapChain->Present( 0, 0 );
}

// No longer needed - delete?
void CScene::RenderMirrors()
{
	
	//*******************************************************************************************************
	// First set the stencil value for all mirror pixels to 1 and clear the mirror to a fixed colour and 
	// set its depth-buffer values to maximum (so we can render "inside" the mirror)

	D3DXMATRIXA16 mirrorMatrix = mpObjects[0]->GetModel()->GetWorldMatrix();
	WorldMatrixVar->SetMatrix( (float*)mirrorMatrix );
	ModelColourVar->SetRawValue( mpObjects[0]->GetColourV(), 0, 12 );
	mpObjects[0]->Render();

	//*******************************************************************************************************
	// Next reflect the camera in the mirror

	// Some mathematics to get as reflected version of the camera - using DirectX helper functions mostly

	// Create a plane for the mirror
	D3DXPLANE mirrorPlane;
	D3DXVECTOR3 mirrorPoint  = D3DXVECTOR3( mirrorMatrix(3,0), mirrorMatrix(3,1), mirrorMatrix(3,2) );
	D3DXVECTOR3 mirrorNormal = D3DXVECTOR3( mirrorMatrix(2,0), mirrorMatrix(2,1), mirrorMatrix(2,2) );
	D3DXPlaneFromPointNormal( &mirrorPlane, &mirrorPoint, &mirrorNormal );
	
	// Reflect the camera's view matrix in the mirror plane
	D3DXMATRIXA16 reflectMatrix;
	D3DXMatrixReflect( &reflectMatrix, &mirrorPlane );
	D3DXMATRIXA16 reflectViewMatrix = reflectMatrix * Camera->GetViewMatrix();

	// Reflect the camera's position in the mirror plane
	D3DXVECTOR3 cameraPos = Camera->GetPosition();
	D3DXVECTOR4 reflectCameraPos4; // Initially generate a 4 element vector
	D3DXVec3Transform( &reflectCameraPos4, &cameraPos, &reflectMatrix );
	D3DXVECTOR3 reflectCameraPos = D3DXVECTOR3( (float*)reflectCameraPos4 ); // Drop 4th element


	//*******************************************************************************************************
	// Render all the models "inside" the mirror

	// Set the reflected camera data in the shaders
	ViewMatrixVar->SetMatrix( (float*)&reflectViewMatrix );
	dxCameraPos->SetRawValue( reflectCameraPos, 0, 12 );
	ClipPlaneVar->SetRawValue( mirrorPlane, 0, 16 );

	// Need to use slightly different techniques to avoid mirror rendering being "inside out"
	
	DrawAllObjects(true);

	// Restore main camera data in the shaders
	ViewMatrixVar->SetMatrix( (float*)&Camera->GetViewMatrix() );
	dxCameraPos->SetRawValue( Camera->GetPosition(), 0, 12 );
	ClipPlaneVar->SetRawValue( D3DXVECTOR4(0,0,0,0), 0, 16 );


	//*******************************************************************************************************
	// Finally draw a "surface" for the mirror - a transparent layer over the mirror contents. This sets the correct depth-buffer values 
	// for the mirror surface, so further rendering won't go "inside" the mirrored scene

	WorldMatrixVar->SetMatrix( (float*)mpObjects[0]->GetModel()->GetWorldMatrix() );
	mpObjects[0]->RenderMirror();
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
bool CScene::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CScene::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "TutorialWindowClass";
	wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if( !RegisterClassEx( &wcex ) )	return false;

	// Create window
	HInst = hInstance;
	RECT rc = { 0, 0, 1920, 960 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	HWnd = CreateWindow( "TutorialWindowClass", "CO2409 - Graphics Assignment - Shaders", WS_OVERLAPPEDWINDOW,
	                     CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL );
	if( !HWnd )	return false;

	ShowWindow( HWnd, nCmdShow );

	return true;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK CScene::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;

		// These windows messages (WM_KEYXXXX) can be used to get keyboard input to the window
		// This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
		case WM_KEYDOWN:
			KeyDownEvent( static_cast<EKeyState>(wParam) );
			break;

		case WM_KEYUP:
			KeyUpEvent( static_cast<EKeyState>(wParam) );
			break;
		// catch window resize
		case WM_SIZE:
			RECT rc;
			GetClientRect( hWnd, &rc );
			mViewportWidth = rc.right - rc.left;
			mViewportHeight = rc.bottom - rc.top;
			// pass to camera
			CCamera::SetViewport( mViewportWidth, mViewportHeight );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
bool CScene::InitDevice()
{
	// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
	// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
	HRESULT hr = S_OK;


	////////////////////////////////
	// Initialise Direct3D

	// Calculate the visible area the window we are using - the "client rectangle" refered to in the first function is the 
	// size of the interior of the window, i.e. excluding the frame and title
	RECT rc;
	GetClientRect( HWnd, &rc );
	mViewportWidth = rc.right - rc.left;
	mViewportHeight = rc.bottom - rc.top;


	// Create a Direct3D device (i.e. initialise D3D), and create a swap-chain (create a back buffer to render to)
	DXGI_SWAP_CHAIN_DESC sd;         // Structure to contain all the information needed
	ZeroMemory( &sd, sizeof( sd ) ); // Clear the structure to 0 - common Microsoft practice, not really good style
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mViewportWidth;             // Target window size
	sd.BufferDesc.Height = mViewportHeight;           // --"--
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
	sd.BufferDesc.RefreshRate.Numerator = 60;          // Refresh rate of monitor
	sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.OutputWindow = HWnd;                          // Target window
	sd.Windowed = TRUE;                                // Whether to render in a window (TRUE) or go fullscreen (FALSE)
	hr = D3D10CreateDeviceAndSwapChain( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG,
										D3D10_SDK_VERSION, &sd, &SwapChain, &mpd3dDevice );
	if( FAILED( hr ) ) return false;


	// Specify the render target as the back-buffer - this is an advanced topic. This code almost always occurs in the standard D3D setup
	ID3D10Texture2D* pBackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) ) return false;
	hr = mpd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &RenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) ) return false;


	// Create a texture (bitmap) to use for a depth buffer
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = mViewportWidth;
	descDepth.Height = mViewportHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// 24 bits for depth, 8 for stencil - more memory efficient
	//descDepth.Format = DXGI_FORMAT_D32_FLOAT;				// 32 bit float depth only 
	//descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// 32 bit depth + 8 for stencil, 24 unused bytes - more precise depth buffer
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mpd3dDevice->CreateTexture2D( &descDepth, NULL, &DepthStencil );
	if( FAILED( hr ) ) return false;

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mpd3dDevice->CreateDepthStencilView( DepthStencil, &descDSV, &DepthStencilView );
	if( FAILED( hr ) ) return false;

	// Select the back buffer and depth buffer to use for rendering now
	mpd3dDevice->OMSetRenderTargets( 1, &RenderTargetView, DepthStencilView );


	// Setup the viewport - defines which part of the window we will render to, almost always the whole window
	D3D10_VIEWPORT vp;
	vp.Width  = mViewportWidth;
	vp.Height = mViewportHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mpd3dDevice->RSSetViewports( 1, &vp );

	CCamera::SetViewport( mViewportWidth, mViewportHeight );
	
	CModel::SetDevice( mpd3dDevice );

	//test->SetDevice( mpd3dDevice );

	return true;
}


// Release the memory held by all objects created
void CScene::ReleaseResources()
{
	// The D3D setup and preparation of the geometry created several objects that use up memory (e.g. textures, vertex/index buffers etc.)
	// Each object that allocates memory (or hardware resources) needs to be "released" when we exit the program
	// There is similar code in every D3D program, but the list of objects that need to be released depends on what was created
	// Test each variable to see if it exists before deletion
	if( mpd3dDevice )     mpd3dDevice->ClearState();

	delete Camera;

	for(int i = 0; i < miNumObjects; i++)
	{
		delete mpObjects[i];
	}

	for(int i = 0; i < miNumLights; i++)
	{
		delete mpLights[i];
	}
	
	for(int i = 0; i < miNumMaps; i++)
	{
		if ( mpMaps[i] )
			mpMaps[i]->Release();
	}

	if( Effect )           Effect->Release();
	if( DepthStencilView ) DepthStencilView->Release();
	if( RenderTargetView ) RenderTargetView->Release();
	if( DepthStencil )     DepthStencil->Release();
	if( SwapChain )        SwapChain->Release();
	if( mpd3dDevice )     mpd3dDevice->Release();

}



//--------------------------------------------------------------------------------------
// Load and compile Effect file (.fx file containing shaders)
//--------------------------------------------------------------------------------------
// An effect file contains a set of "Techniques". A technique is a combination of vertex, geometry and pixel shaders (and some states) used for
// rendering in a particular way. We load the effect file at runtime (it's written in HLSL and has the extension ".fx"). The effect code is compiled
// *at runtime* into low-level GPU language. When rendering a particular model we specify which technique from the effect file that it will use

bool CScene::LoadEffectFile()
{
	ID3D10Blob* pErrors; // This strangely typed variable collects any errors when compiling the effect file
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; // These "flags" are used to set the compiler options

	// Load and compile the effect file
	HRESULT hr = D3DX10CreateEffectFromFile( "GraphicsAssign1.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, mpd3dDevice, NULL, NULL, &Effect, &pErrors, NULL );
	if( FAILED( hr ) )
	{
		if (pErrors != 0)  MessageBox( NULL, CA2CT(reinterpret_cast<char*>(pErrors->GetBufferPointer())), "Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, "Error loading FX file. Ensure your FX file is in the same folder as this executable.", "Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	// Now we can select techniques from the compiled effect file
	// Regular techniques for the main scene
	mTechniques[ 0]	= Effect->GetTechniqueByName( "tPlainColour" );
	mTechniques[ 1]	= Effect->GetTechniqueByName( "tPlainTexture" );
	mTechniques[ 2]	= Effect->GetTechniqueByName( "tColourChangeTexture" );
	mTechniques[ 3]	= Effect->GetTechniqueByName( "tLitTexture" );
	mTechniques[ 4]	= Effect->GetTechniqueByName( "t4LitTexture" );
	miNumTechniques = 5;

	// Create special variables to allow us to access global variables in the shaders from C++
	WorldMatrixVar		= Effect->GetVariableByName( "WorldMatrix" )->AsMatrix();
	ViewMatrixVar		= Effect->GetVariableByName( "ViewMatrix"  )->AsMatrix();
	ProjMatrixVar		= Effect->GetVariableByName( "ProjMatrix"  )->AsMatrix();

	// We access the texture variable in the shader in the same way as we have before for matrices, light data etc.
	// Only difference is that this variable is a "Shader Resource"
	DiffuseMapVar = Effect->GetVariableByName( "DiffuseMap" )->AsShaderResource();
	NormalMapVar = Effect->GetVariableByName( "NormalMap" )->AsShaderResource();

	//Camera Position
	dxCameraPos = Effect->GetVariableByName( "cameraPos" )->AsVector();

	// Other shader variables
	ModelColourVar = Effect->GetVariableByName( "ModelColour"  )->AsVector();

	//Lighting Values
	dxLightPosA = Effect->GetVariableByName( "lightPos" )->AsVector();
	dxLightColourA = Effect->GetVariableByName( "lightColour" )->AsVector();
	dxLightBrightnessA = Effect->GetVariableByName( "lightBright" )->AsVector();
	
	dxAmbientColour = Effect->GetVariableByName( "ambientColour" )->AsVector();

	dxWiggle = Effect->GetVariableByName( "wiggle" )->AsScalar();
	dxOutlineThickness = Effect->GetVariableByName( "OutlineThickness" )->AsScalar();

	ClipPlaneVar      = Effect->GetVariableByName( "ClipPlane"      )->AsVector();

	return true;
}

//--------------------------------------------------------------------------------------
// Light helper functions
//--------------------------------------------------------------------------------------


struct LightIndex
{
	int index;
	float distance;
};

int CScene::compareLights( const void* a, const void* b)
{
	LightIndex* x = (LightIndex*)a;
	LightIndex* y = (LightIndex*)b;
	return int(x->distance - y->distance);
}

// calculates the closest lights to the object to pass to the shader. 
void CScene::SetLights( D3DXVECTOR3 source, CLight* lightsSource[MAX_LIGHTS], int lightCount)
{
	LightIndex indexes[MAX_LIGHTS];

	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		indexes[i].distance = 777777777.0f;
	}

	D3DXVECTOR3 vLight;
	for(int i = 0; i < lightCount; i++)
	{
		indexes[i].index = i;
		vLight = lightsSource[i]->GetModel()->GetPosition();
		indexes[i].distance = square(vLight.x - source.x) + square (vLight.y - source.y) + square(vLight.z - source.z);
	}

	qsort( (void*) indexes, MAX_LIGHTS, sizeof(LightIndex), &CScene::compareLights);

	int count;
	
	if( lightCount < MAX_SHADER_LIGHTS )
	{
		count = lightCount;
	}
	else
	{
		count = MAX_SHADER_LIGHTS;
	}

	D3DXVECTOR4 positions[MAX_SHADER_LIGHTS];
	D3DXVECTOR4 colours[MAX_SHADER_LIGHTS];
	D3DXVECTOR4 bright[MAX_SHADER_LIGHTS];

	for(int i = 0; i < count; i++)
	{
		int c = indexes[i].index;
		positions[i] = D3DXVECTOR4 (mpLights[c]->GetModel()->GetPosition(), 0 );
		colours[i] = D3DXVECTOR4 ( mpLights[c]->GetColourV(), 0 );
		bright[i] = D3DXVECTOR4( mpLights[c]->GetBrightness(), 0, 0, 0 );   // vector 4 for packing, remaining variables would be useful for light type and angle. 
	}

	dxLightPosA->SetRawValue( positions, 0, 16 * count );
	dxLightColourA->SetRawValue( colours, 0, 16 * count );
	dxLightBrightnessA->SetRawValue( bright, 0, 16 * count );
}