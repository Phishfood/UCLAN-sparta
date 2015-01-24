#include "Scene.h"

int CScene::mViewportWidth, CScene::mViewportHeight;

CScene::CScene(void)
{
	//set counters to 0
	miNumObjects = 0;
	miNumLights = 0;
	miNumMaps = 0;

	//set the ambient colour for the scene
	AmbientColour = DirectX::XMFLOAT3( 0.2f, 0.2f, 0.8f );

	//set the controlled object to the mirror
	mControlObject = 0;
	
	mi_numSquares = 0;

	mb_showWallIM = false;
	mb_showWallIM = false;
	mb_showPathIM = false;
	mb_showBaseIM = false;
	mb_showHTIM = false;
	mb_showLTIM = false;
	mb_showMTIM = false;
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

	//Camera = new CCamera();
	Camera.SetPosition( DirectX::XMFLOAT3(-15, 20,-40) );
	Camera.SetRotation( DirectX::XMFLOAT3(ToRadians(13.0f), ToRadians(18.0f), 0.0f) ); // ToRadians is a new helper function to convert degrees to radians
	Camera.SetNearClip( 1.0f );

	//load all textures and maps
	miNumMaps = 0;

	mc_map = new MapSquare("bob", mySettings);

	//caching these locally - does mean more coupling, but seems better than fetching an unchanging variable repeatedly.
	mi_mapHeight = mc_map->GetHeight();
	mi_mapWidth = mc_map->GetWidth();
	
	//mp_openSquares = new ModelBatch();
	mp_openSquares.Load( "Cube.x", mTechniques[0], false );
	mp_openSquares.SetColour(_WHITE);
	//mp_openSquares.mp_technique = mTechniques[0];

	//mp_walls = new ModelBatch();
	mp_walls.Load( "Cube.x", mTechniques[0], false );
	mp_walls.SetColour(_BLACK);
	//mp_walls.mp_technique = mTechniques[0];

	mp_pathModel.Load("TrafficCone.x", mTechniques[0], false);
	mp_pathModel.SetColour(_ORANGE);
	//mp_pathModel.mp_technique = mTechniques[0];

	mp_splineModel.Load("TrafficCone.x", mTechniques[0], false);
	mp_splineModel.SetColour(_ORANGE);
	//mp_splineModel.mp_technique = mTechniques[0];



	for( uint32_t y = 0; y < mi_mapHeight; y++ )
	{
		for( uint32_t x = 0; x < mi_mapWidth; x++ )
		{
			switch( mc_map->GetCost( x, y ) )
			{
				case 0: //walls
					mp_matrixArray[mi_numSquares] = mp_walls.AddInstance( DirectX::XMFLOAT3( static_cast<float>(x), 0.0f, static_cast<float>(y) ), 0.1f );
					break;
				case 1: //open
					mp_matrixArray[mi_numSquares] = mp_openSquares.AddInstance( DirectX::XMFLOAT3( static_cast<float>(x), 0.0f, static_cast<float>(y) ), 0.1f );
					break;
				case 2:
					mp_p1StartModel = new CRenderObject( "Cube.x", DirectX::XMFLOAT3( static_cast<float>(x), 0.0f, static_cast<float>(y) ), DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), mTechniques[0], NULL );
					mp_p1StartModel->SetColour( _RED );
					mp_p1StartModel->GetModel()->SetScale(0.1f);
					mp_p1StartModel->GetModel()->UpdateMatrix();
					md_p1StartMatrix = mp_p1StartModel->GetModel()->GetWorldMatrix();
					mp_matrixArray[mi_numSquares] = &md_p1StartMatrix;
					break;
				case 3:
					mp_p2StartModel = new CRenderObject( "Cube.x", DirectX::XMFLOAT3( static_cast<float>(x), 0.0f, static_cast<float>(y) ), DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), mTechniques[0], NULL );
					mp_p2StartModel->SetColour( _GREEN );
					mp_p2StartModel->GetModel()->SetScale( 0.1f );
					mp_p2StartModel->GetModel()->UpdateMatrix();
					md_p2StartMatrix = mp_p2StartModel->GetModel()->GetWorldMatrix();
					mp_matrixArray[mi_numSquares] = &md_p2StartMatrix;
					break;
			}

			mi_numSquares++;
		}
	}

	mpObjects[0] = mp_p1StartModel;
	mpObjects[1] = mp_p2StartModel;
	miNumObjects = 2;

	  ///////////////////////////////////////
	 // Text                              //
	///////////////////////////////////////

	mpSpriteBatch = new DirectX::SpriteBatch( mpd3dDeviceContext );
	mpSpriteFont = new DirectX::SpriteFont(mpd3dDevice, L"Arial.spritefont");

	mb_showBase2IM = false;
	mb_showBaseIM = false;
	mb_showCost = false;
	mb_showHTIM = false;
	mb_showLTIM = false;
	mb_showMTIM = false;
	mb_showPathIM = false;
	mb_showWallIM = false;

	  ///////////////////////////////////////
	 // Tweak bars                        //
	///////////////////////////////////////

	int32_t result;
	result = TwInit(TW_DIRECT3D11, mpd3dDevice);
	TweakError(result)
	result = TwWindowSize(mySettings->resolutionX, mySettings->resolutionY);
	TweakError( result )

		
	mpSettingsBar = TwNewBar( "Settings" );
	TwDefine(" Settings position='5 5' ");
	result = TwAddSeparator( mpSettingsBar, "Graphics - requires restart", "" );
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "ResolutionX", TW_TYPE_UINT32, &mySettings->resolutionX, "" );
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "ResolutionY", TW_TYPE_UINT32, &mySettings->resolutionY, "" );
	TweakError(result)
	result = TwAddVarRW(mpSettingsBar, "Node Model", TW_TYPE_CHAR, &mySettings->nodeModel, "");
	TweakError(result)
	result = TwAddVarRW(mpSettingsBar, "Node Scale", TW_TYPE_FLOAT, &mySettings->nodeScale, "");
	TweakError(result)
	result = TwAddVarRW(mpSettingsBar, "Spline Model", TW_TYPE_CHAR, &mySettings->splineModel, "");
	TweakError(result)
	result = TwAddVarRW(mpSettingsBar, "Spline Scale", TW_TYPE_FLOAT, &mySettings->splineScale, "");
	TweakError(result)
	result = TwAddSeparator(mpSettingsBar, "AI - requires recalcualte", "");
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "Wall Range", TW_TYPE_UINT32, &mySettings->wallRange, "" );
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "Base Range", TW_TYPE_UINT32, &mySettings->baseRange, "" );
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "Heavy Turret Range", TW_TYPE_UINT32, &mySettings->HTRange, "" );
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "Medium Turret Range", TW_TYPE_UINT32, &mySettings->MTRange, "" );
	TweakError( result )
	result = TwAddVarRW( mpSettingsBar, "Light Turret Range", TW_TYPE_UINT32, &mySettings->LTRange, "" );
	TweakError( result )

	mpCalculationsBar = TwNewBar("Calculations");
	TwDefine( " Calculations position='205 5' " );
	result = TwAddButton(mpCalculationsBar, "Recalculate Walls", CalculateWallIM, mc_map, "");
	TweakError( result )
	result = TwAddButton( mpCalculationsBar, "Recalculate Path", CalculatePath, mc_map, "" );
	TweakError(result)
	result = TwAddButton(mpCalculationsBar, "Visualise Path", TWVisualisePath, mp_self, "");
	TweakError(result)
	result = TwAddVarRO(mpCalculationsBar, "Last Calculation Time", TW_TYPE_FLOAT, &mc_map->mf_calcTime, "");
	TweakError( result )

	mpFileBar = TwNewBar("Files");
	TwDefine( " Files position='410 5' " );
	result = TwAddButton(mpFileBar, "Reload Settings", ReloadSettings, mySettings, "");
	TweakError( result )
	result = TwAddButton( mpFileBar, "Write Settings", WriteSettings, mySettings, "" );
	TweakError( result )
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
	Camera.Control( frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );
	Camera.UpdateMatrices();
	

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

	if (KeyHit( Key_F1 ) )
	{
		mb_showCost = !mb_showCost;
		mb_showWallIM = false;
		mb_showPathIM = false;
		mb_showBaseIM = false;
		mb_showBase2IM = false;
		mb_showHTIM = false;
		mb_showLTIM = false;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F2 ) )
	{
		mb_showCost = false;
		mb_showWallIM = !mb_showWallIM;
		mb_showPathIM = false;
		mb_showBaseIM = false;
		mb_showBase2IM = false;
		mb_showHTIM = false;
		mb_showLTIM = false;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F3 ) )
	{
		mb_showCost = false;
		mb_showWallIM = false;
		mb_showPathIM = !mb_showPathIM;
		mb_showBaseIM = false;
		mb_showBase2IM = false;
		mb_showHTIM = false;
		mb_showLTIM = false;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F4 ) )
	{
		mb_showCost = false;
		mb_showWallIM = false;
		mb_showPathIM = false;
		mb_showBaseIM = !mb_showBaseIM;
		mb_showBase2IM = false;
		mb_showHTIM = false;
		mb_showLTIM = false;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F5 ) )
	{
		mb_showCost = false;
		mb_showWallIM = false;
		mb_showPathIM = false;
		mb_showBaseIM = false;
		mb_showBase2IM = !mb_showBase2IM;
		mb_showHTIM = false;
		mb_showLTIM = false;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F6 ) )
	{
		mb_showCost = false;
		mb_showWallIM = false;
		mb_showPathIM = false;
		mb_showBaseIM = false;
		mb_showBase2IM = false;
		mb_showHTIM = !mb_showHTIM;
		mb_showLTIM = false;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F7 ) )
	{
		mb_showCost = false;
		mb_showWallIM = false;
		mb_showPathIM = false;
		mb_showBaseIM = false;
		mb_showBase2IM = false;
		mb_showHTIM = false;
		mb_showLTIM = !mb_showLTIM;
		mb_showMTIM = false;
	}

	if (KeyHit( Key_F8 ) )
	{
		mb_showCost = false;
		mb_showWallIM = false;
		mb_showPathIM = false;
		mb_showBaseIM = false;
		mb_showBase2IM = false;
		mb_showHTIM = false;
		mb_showLTIM = false;
		mb_showMTIM = !mb_showMTIM;
	}
}

//function to pass data to the shaders and render the object.
void CScene::DrawObject(int i, bool mirror)
{

	//outline thickness
	//float thickness = mpObjects[i]->GetOutlineThickness();
	//dxOutlineThickness->SetRawValue( &thickness, 0, 4);

	//pass the matrix
	WorldMatrixVar->SetMatrix( (float*)mpObjects[i]->GetModel()->GetWorldMatrix().r );
	
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
	DirectX::XMFLOAT3 f3 = mpObjects[i]->GetColourV();
	//V3 temp = XMF3ToFloat3( f3 );
	ModelColourVar->SetRawValue( (float*)&f3, 0, 12 );
	
	//if the object is lit, pass over the 12 closest lights. 
	if( mpObjects[i]->IsLit() )
	{
		SetLights( mpObjects[i]->GetModel()->GetPosition(), mpLights, miNumLights);
	}

	// choose how to render the model based on where we are rendering it. 
	mpObjects[i]->Render();
}

// function to render everything in the scene
void CScene::DrawAllObjects(bool mirror)
{
	//
	// Solid objects
	//
	for(int i = 0; i < miNumObjects; i++)
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
		WorldMatrixVar->SetMatrix( (float*)mpLights[i]->GetModel()->GetWorldMatrix().r );
		V3 temp = XMF3ToFloat3(mpLights[i]->GetColourV() );
		ModelColourVar->SetRawValue(&temp, 0, 12 );
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
	mpd3dDeviceContext->ClearRenderTargetView( RenderTargetView, ClearColor );
	mpd3dDeviceContext->ClearDepthStencilView( DepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL , 1.0f, 0 ); // Clear the depth buffer too


	// Pass the camera's matrices to the vertex shader
	ViewMatrixVar->SetMatrix( (float*)&Camera.GetViewMatrix() );
	ProjMatrixVar->SetMatrix( (float*)&Camera.GetProjectionMatrix() );

	//pass the camera position
	//V3 temp = XMF3ToFloat3( Camera->GetPosition() );
	//dxCameraPos->SetRawValue( &temp, 0, 12);

	//pass the lighting colours
	//temp = XMF3ToFloat3( AmbientColour );
	//dxAmbientColour->SetRawValue( &temp, 0, 12  );

	//---------------------------
	// Render each model
	
	DrawAllObjects(false);

	mp_openSquares.RenderBatch(WorldMatrixVar, ModelColourVar);
	mp_walls.RenderBatch( WorldMatrixVar, ModelColourVar );
	mp_pathModel.RenderBatch(WorldMatrixVar, ModelColourVar);
	mp_splineModel.RenderBatch(WorldMatrixVar, ModelColourVar);

	mpSpriteBatch->Begin();
	//mpSpriteFont->DrawString( mpSpriteBatch, L"Hello, world!", DirectX::XMFLOAT2( 500.0f, 500.0f ) );

	FontRect.bottom = 0;
	FontRect.right = 0;

	if( mb_showBaseIM )
	{
		DisplayText( "Base Influence Map", 0);
		DisplayMapText(BASE_OFFSET);
	}

	if( mb_showBase2IM )
	{
		DisplayText( "Base 2 Influence Map", 0);
		DisplayMapText(BASE2_OFFSET);
	}

	if( mb_showCost )
	{
		DisplayText( "Square Cost", 0);
		DisplayMapText(COST_OFFSET);
	}

	if( mb_showHTIM )
	{
		DisplayText( "Heavy Turret Influence Map", 0 );
		DisplayMapText(HEAVY_OFFSET);
	}

	if( mb_showLTIM )
	{
		DisplayText( "Light Turret Influence Map", 0);
		DisplayMapText(LIGHT_OFFSET);
	}

	if( mb_showMTIM )
	{
		DisplayText( "Medium Turret Influence Map", 0);
		DisplayMapText(MEDIUM_OFFSET);
	}

	if( mb_showPathIM )
	{
		DisplayText( "Path Influence Map", 0 );
		DisplayMapText( PATH_OFFSET);
	}

	if( mb_showWallIM )
	{
		DisplayText( "Wall Influence Map", 0);
		DisplayMapText(WALL_OFFSET);
	}
	mpSpriteBatch->End();
	TwDraw();

	//---------------------------
	// Display the Scene

	// After we've finished drawing to the off-screen back buffer, we "present" it to the front buffer (the screen)
	SwapChain->Present( 0, 0 );
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
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
	if( !RegisterClassEx( &wcex ) )	return false;

	// Create window
	HInst = hInstance;
	
	RECT rc = { 0, 0, mySettings->resolutionX, mySettings->resolutionY };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	HWnd = CreateWindow( L"TutorialWindowClass", L"Dissertation Project", WS_OVERLAPPEDWINDOW,
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
	if( TwEventWin( hWnd, message, wParam, lParam ) ) // send event message to AntTweakBar
		return 0; // event has been handled by AntTweakBar
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
	sd.BufferDesc.RefreshRate.Numerator = 60;         // Refresh rate of monitor
	sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.OutputWindow = HWnd;                          // Target window
	sd.Windowed = TRUE; // Whether to render in a window (TRUE) or go fullscreen (FALSE)

	D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
	//D3D_FEATURE_LEVEL fLevel;

	/*hr = D3D11CreateDeviceAndSwapChain( 
		NULL,						//Adaptor - Use default
		D3D_DRIVER_TYPE_HARDWARE,	//Driver Type - Hardware, always hardware. 
		0,							//Software rasteriser. Yeah right. Null because we are using hardware.
		2,							//Build flags (2 = debug)
		featureLevels,				//feature levels (see above)
		7,							//number of feature levels
		D3D11_SDK_VERSION,			//SDK version
		&sd,						//SwapChainDesc
		&SwapChain,					//SwapChain
		&mpd3dDevice,				//Device
		&fLevel,					//chosen feature level
		&mpd3dDeviceContext			//Device context
		);		*/	

	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, 0, 2, 0, 0, D3D11_SDK_VERSION, &sd, &SwapChain, &mpd3dDevice, NULL, &mpd3dDeviceContext ); //D3D11_CREATE_DEVICE_DEBUG
	if( FAILED( hr ) ) return false;


	// Specify the render target as the back-buffer - this is an advanced topic. This code almost always occurs in the standard D3D setup
	ID3D11Texture2D* pBackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	if( FAILED( hr ) ) return false;
	hr = mpd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &RenderTargetView );
	pBackBuffer->Release();
	if( FAILED( hr ) ) return false;


	// Create a texture (bitmap) to use for a depth buffer
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = mViewportWidth;
	descDepth.Height = mViewportHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// 24 bits for depth, 8 for stencil - more memory efficient
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	//descDepth.Format = DXGI_FORMAT_D32_FLOAT;				// 32 bit float depth only 
	//descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// 32 bit depth + 8 for stencil, 24 unused bytes - more precise depth buffer
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mpd3dDevice->CreateTexture2D( &descDepth, NULL, &DepthStencil );
	if( FAILED( hr ) ) return false;

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	//descDSV.Format = descDepth.Format;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;
	hr = mpd3dDevice->CreateDepthStencilView( DepthStencil, &descDSV, &DepthStencilView );
	if( FAILED( hr ) ) return false;

	// Select the back buffer and depth buffer to use for rendering now
	mpd3dDeviceContext->OMSetRenderTargets( 1, &RenderTargetView, DepthStencilView );


	// Setup the viewport - defines which part of the window we will render to, almost always the whole window
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(mViewportWidth);
	vp.Height = static_cast<float>(mViewportHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mpd3dDeviceContext->RSSetViewports( 1, &vp );

	CCamera::SetViewport( mViewportWidth, mViewportHeight );
	
	CModel::SetDevice( mpd3dDevice );
	CModel::SetDeviceContext( mpd3dDeviceContext );

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
	if( mpd3dDeviceContext )     mpd3dDeviceContext->ClearState();

	//delete Camera;

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
	TwTerminate();
}



//--------------------------------------------------------------------------------------
// Load and compile Effect file (.fx file containing shaders)
//--------------------------------------------------------------------------------------
// An effect file contains a set of "Techniques". A technique is a combination of vertex, geometry and pixel shaders (and some states) used for
// rendering in a particular way. We load the effect file at runtime (it's written in HLSL and has the extension ".fx"). The effect code is compiled
// *at runtime* into low-level GPU language. When rendering a particular model we specify which technique from the effect file that it will use
bool CScene::LoadShaders()
{
	ID3DBlob* pEffectBlob;
	ID3DBlob* pErrors; // This strangely typed variable collects any errors when compiling the effect file
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; // These "flags" are used to set the compiler options

	//compile shaders individually
	HRESULT hr; 
	hr = D3DX11CompileFromFile(
		L"VertexShader.hlsl",
		NULL,
		NULL,
		NULL,
		"fx_5_0",
		dwShaderFlags,
		0,
		NULL,
		&pEffectBlob,
		&pErrors,
		&hr
		);

	if( FAILED( hr ) )
	{
		if( pErrors != 0 )  MessageBox( NULL, CA2CT( reinterpret_cast<char*>(pErrors->GetBufferPointer()) ), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error loading Vertex Shader FX file. Ensure your FX file is in the same folder as this executable.", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	hr = mpd3dDevice->CreateVertexShader( pEffectBlob->GetBufferPointer(), pEffectBlob->GetBufferSize(), NULL, &mp_VertexShader[0] );

	if( FAILED( hr ) )
	{
		if( pErrors != 0 )  MessageBox( NULL, CA2CT( reinterpret_cast<char*>(pErrors->GetBufferPointer()) ), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error creating Vertex Shader.", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	hr = D3DX11CompileFromFile(
		L"PixelShader.hlsl",
		NULL,
		NULL,
		"main",
		"fx_5_0",
		dwShaderFlags,
		0,
		NULL,
		&pEffectBlob,
		&pErrors,
		&hr
		);

	if( FAILED( hr ) )
	{
		if( pErrors != 0 )  MessageBox( NULL, CA2CT( reinterpret_cast<char*>(pErrors->GetBufferPointer()) ), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error loading Pixel Shader FX file. Ensure your FX file is in the same folder as this executable.", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	hr = mpd3dDevice->CreatePixelShader( pEffectBlob->GetBufferPointer(), pEffectBlob->GetBufferSize(), NULL, &mp_PixelShader[0] );


	if( FAILED( hr ) )
	{
		if( pErrors != 0 )  MessageBox( NULL, CA2CT( reinterpret_cast<char*>(pErrors->GetBufferPointer()) ), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error Creating Pixel Shader", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}
	return true;
}

bool CScene::LoadTechnique()
{
	HRESULT hr;
	ID3DBlob* pEffectBlob;
	ID3DBlob* pErrors; // This strangely typed variable collects any errors when compiling the effect file
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS; // These "flags" are used to set the compiler options
	DWORD flags2 = D3DCOMPILE_DEBUG | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

	//Compile Technique file
	hr = D3DX11CompileFromFile(
		L"./Shaders/Technique.fx",
		NULL,
		NULL,
		NULL,
		"fx_5_0",
		dwShaderFlags,
		0,
		NULL,
		&pEffectBlob,
		&pErrors,
		&hr
		);

	if( FAILED( hr ) )
	{
		if( pErrors != 0 )  MessageBox( NULL, CA2CT( reinterpret_cast<char*>(pErrors->GetBufferPointer()) ), L"Error", MB_OK ); // Compiler error: display error message
		else               MessageBox( NULL, L"Error loading Vertex Shader FX file. Ensure your FX file is in the same folder as this executable.", L"Error", MB_OK );  // No error message - probably file not found
		return false;
	}

	uint32_t SHADER_DEBUG = 1;
	uint32_t SHADER_RELEASE = 0;
	LPVOID bufferPointer = pEffectBlob->GetBufferPointer();
	SIZE_T bufferSize = pEffectBlob->GetBufferSize();
	hr = D3DX11CreateEffectFromMemory( bufferPointer, bufferSize, 0, mpd3dDevice, &Effect );

	if( FAILED( hr ) )
	{
		MessageBox( NULL, L"Error creating effects", L"Error", MB_OK );
		return false;
	}

	return true;
}

bool CScene::LoadEffectFile()
{	
	if( !LoadTechnique() )
	{
		return false; 
	}
	
	// Now we can select techniques from the compiled effect file
	// Regular techniques for the main scene
	//mTechniques[0] = NULL;
	mTechniques[ 0]	= Effect->GetTechniqueByName( "SingleColour" );
	miNumTechniques = 5;
	
	// Create special variables to allow us to access global variables in the shaders from C++
	WorldMatrixVar		= Effect->GetVariableByName( "WorldMatrix" )->AsMatrix();
	ViewMatrixVar		= Effect->GetVariableByName( "ViewMatrix"  )->AsMatrix();
	ProjMatrixVar		= Effect->GetVariableByName( "ProjMatrix"  )->AsMatrix();

	// We access the texture variable in the shader in the same way as we have before for matrices, light data etc.
	// Only difference is that this variable is a "Shader Resource"
	//DiffuseMapVar = Effect->GetVariableByName( "DiffuseMap" )->AsShaderResource();
	//NormalMapVar = Effect->GetVariableByName( "NormalMap" )->AsShaderResource();

	//Camera Position
	//dxCameraPos = Effect->GetVariableByName( "cameraPos" )->AsVector();

	// Other shader variables
	ModelColourVar = Effect->GetVariableByName( "ModelColour"  )->AsVector();

	//Lighting Values
	//dxLightPosA = Effect->GetVariableByName( "lightPos" )->AsVector();
	//dxLightColourA = Effect->GetVariableByName( "lightColour" )->AsVector();
	//dxLightBrightnessA = Effect->GetVariableByName( "lightBright" )->AsVector();
	
	//dxAmbientColour = Effect->GetVariableByName( "ambientColour" )->AsVector();

	//dxWiggle = Effect->GetVariableByName( "wiggle" )->AsScalar();
	//dxOutlineThickness = Effect->GetVariableByName( "OutlineThickness" )->AsScalar();

	//ClipPlaneVar      = Effect->GetVariableByName( "ClipPlane"      )->AsVector();

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
void CScene::SetLights( DirectX::XMFLOAT3 source, CLight* lightsSource[MAX_LIGHTS], int lightCount)
{
	LightIndex indexes[MAX_LIGHTS];

	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		indexes[i].distance = 777777777.0f;
	}

	DirectX::XMFLOAT3 vLight;
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

	DirectX::XMFLOAT4 positions[MAX_SHADER_LIGHTS];
	DirectX::XMFLOAT4 colours[MAX_SHADER_LIGHTS];
	DirectX::XMFLOAT4 bright[MAX_SHADER_LIGHTS];
	
	for(int i = 0; i < count; i++)
	{
		int c = indexes[i].index;
		V3 temp = XMF3ToFloat3( mpLights[c]->GetModel()->GetPosition() );
		positions[i] = DirectX::XMFLOAT4 (temp.x, temp.y, temp.z, 0 );

		temp = XMF3ToFloat3( mpLights[c]->GetColourV() );
		colours[i] = DirectX::XMFLOAT4 (temp.x, temp.y, temp.z, 0 );
		bright[i] = DirectX::XMFLOAT4( mpLights[c]->GetBrightness(), 0, 0, 0 );   // vector 4 for packing, remaining variables would be useful for light type and angle. 
	}

	dxLightPosA->SetRawValue( positions, 0, 16 * count );
	dxLightColourA->SetRawValue( colours, 0, 16 * count );
	dxLightBrightnessA->SetRawValue( bright, 0, 16 * count );
}

void CScene::DisplayText( char text[], UINT32 line )
{

}

void CScene::DisplayMapText( INT32 offset )
{

	DirectX::XMFLOAT4 matrixTemp;
	DirectX::XMFLOAT3 world;
	DirectX::XMFLOAT2 pixel;
	for(int i = 0; i < mi_numSquares; i++)
	{	
		//DirectX::XMFLOAT3 world = DirectX::XMFLOAT3(md_mapMatrix[i]._41, md_mapMatrix[i]._42, md_mapMatrix[i]._43);
		
		
		//DirectX::XMStoreFloat4x4( &matrixTemp, md_mapMatrix[i] );
		DirectX::XMStoreFloat4(&matrixTemp, mp_matrixArray[i]->r[3] );
		world.x = matrixTemp.x;
		world.y = matrixTemp.y;
		world.z = matrixTemp.z;

		Camera.PixelFromWorldPt(&pixel, world);
		wchar_t buffer[6];
		//_itoa_s( mc_map->GetValueByOffset(i%mi_mapWidth,i/mi_mapWidth, offset), temp, 10 );
		swprintf( buffer, 6, L"%d", mc_map->GetValueByOffset( i%mi_mapWidth, i / mi_mapWidth, offset ) );
		mpSpriteFont->DrawString(
			mpSpriteBatch,						//Font
			buffer,								//Text
			pixel,								//Position
			DirectX::Colors::Red,				//Colour
			0.0f,								//Rotation
			DirectX::XMFLOAT2( 0.0f, 0.0f ),	//origin
			DirectX::XMFLOAT2( 0.5f, 0.5f )		//scale
			);
	}
	
}

void CScene::VisualisePath()
{
	Coords* nodes;
	sCoords** splines;
	uint32_t numNodes, numSplines;

	mc_map->GetPathNodes(numNodes, nodes);
	mc_map->GetPathSplines(numSplines, splines);

	for (uint32_t i = 0; i < numNodes; i++)
	{
		mp_pathModel.AddInstance(
			DirectX::XMFLOAT3(static_cast<float>(nodes[i].x), 1.0f, static_cast<float>(nodes[i].y)),
			mySettings->nodeScale
		);
	}

	for (uint32_t i = 0; i < numSplines; i++)
	{
		mp_splineModel.AddInstance(
			DirectX::XMFLOAT3(splines[i]->x, 1.0f, splines[i]->y),
			mySettings->splineScale
		);
	}
	
}