#pragma once

//DirectX Tool Kit - Must go first!
//#include "DirectXTK/CommonStates.h"
//#include "DirectXTK/DirectXHelpers.h"
#include "DirectXTK/SpriteFont.h"
#pragma comment(lib, ".\\lib\\DirectXTK.lib")

//External includes
#include <atlbase.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "resource.h"
#include "D3DCompiler.h"
#include "D3DX11async.h"

//Local includes
#include "Defines.h"    // General definitions shared by all source files
#include "Model.h"      // Model class - encapsulates working with vertex/index data and world matrix
#include "Camera.h"     // Camera class - encapsulates the camera's view and projection matrix
#include "CTimer.h"     // Timer class - not DirectX
#include "Input.h"      // Input functions - not DirectX
#include "MapSquare.h"  // Storage for base map data and influence maps
#include "SettingsManager.h"
#include "Light.h"			// Light class.
#include "RenderObject.h"	// Render class.
#include "ModelBatch.h"

#include "TweakFunctions.h"

#include "Colours.h"		// Functions to adjust colours

#define TweakError(x) if(x == 0) {TwGetLastError(); return false;}

class CScene
{
public:
	SettingsManager* mySettings;
private:
	//Need this to pass self to tweak bar. Really.
	CScene* mp_self;
	
	TwBar* mpSettingsBar;
	TwBar* mpCalculationsBar;
	TwBar* mpFileBar;

	// Fonts
	DirectX::SpriteBatch* mpSpriteBatch;
	DirectX::SpriteFont* mpSpriteFont;

	// Map data
	MapSquare* mc_map;
	uint32_t mi_numSquares;
	DirectX::XMMATRIX md_p1StartMatrix, md_p2StartMatrix;
	DirectX::XMMATRIX* mp_matrixArray[1000];
	
	ModelBatch mp_openSquares;
	ModelBatch mp_walls;
	ModelBatch mp_pathModel;
	ModelBatch mp_splineModel;

	CRenderObject* mp_p1StartModel;
	CRenderObject* mp_p2StartModel;

	ModelBatch* mp_path;

	// Text rendering
	RECT FontRect;

	enum textDisplay
	{
		showNone,
		showCost,
		showWallIM,
		showPathIM,
		showChokeIM,
		showBaseIM,
		showBase2IM,
		showHTIM,
		showMTIM,
		showLTIM,
	} meTextState;

	// Display options
	bool mb_showCost;
	bool mb_showWallIM;    // Toggleable to show the influence of the walls for each square.
	bool mb_showPathIM;    // Path
	bool mb_showBaseIM;
	bool mb_showBase2IM;
	bool mb_showHTIM;      // Heavy turrets
	bool mb_showMTIM;      // Medium turrets
	bool mb_showLTIM;      // Light turrets
	

	// DX Device pointer
	ID3D11Device* mpd3dDevice;
	ID3D11DeviceContext* mpd3dDeviceContext;

	//Viewport details - static because the message handler is static. 
	static int mViewportWidth, mViewportHeight;	

	static const int MAX_LIGHTS = 30;
	static const int MAX_SHADER_LIGHTS = 12;
	static const int MAX_OBJECTS = 2000;

	//MapSquare* mapValues[MAX_MAP_SIZE][MAX_MAP_SIZE];
	UINT32 mi_mapWidth, mi_mapHeight;
	
	// Models and cameras encapsulated in classes for flexibity and convenience
	// The CModel class collects together geometry and world matrix, and provides functions to control the model and render it
	// The CCamera class handles the view and projections matrice, and provides functions to control the camera
	CCamera Camera;

	CRenderObject* mpObjects[MAX_OBJECTS];
	int miNumObjects;

	CLight* mpLights[MAX_LIGHTS];
	int miNumLights;

	// Textures - no texture class yet so using DirectX variables
	// * 2 since each object can potentially have 2 textures, although this is likely overkill.
	ID3D10ShaderResourceView* mpMaps[MAX_OBJECTS*2];
	int miNumMaps;

	// Ambient Light
	DirectX::XMFLOAT3 AmbientColour;

	//defines the object to be controlled via keyboard
	int mControlObject;

	// Note: There are move & rotation speed constants in Defines.h


	//--------------------------------------------------------------------------------------
	// Shader Variables
	//--------------------------------------------------------------------------------------
	// Variables to connect C++ code to HLSL shaders

	// Effects / techniques
	ID3DX11Effect*          Effect;
	ID3DX11EffectTechnique* mTechniques[30];
	ID3DX11EffectTechnique* mTechniquesMirror[30];
	int miNumTechniques;

	// Shaders
	ID3D11PixelShader*  mp_PixelShader[30];
	ID3D11VertexShader* mp_VertexShader[30];

	// Matrices
	ID3DX11EffectMatrixVariable* WorldMatrixVar;
	ID3DX11EffectMatrixVariable* ViewMatrixVar;
	ID3DX11EffectMatrixVariable* ProjMatrixVar;
	ID3DX11EffectMatrixVariable* ViewProjMatrixVar;

	// Textures
	ID3DX11EffectShaderResourceVariable* DiffuseMapVar;
	ID3DX11EffectShaderResourceVariable* NormalMapVar;

	// Miscellaneous
	ID3DX11EffectVectorVariable* ModelColourVar;

	//Positions
	ID3DX11EffectVectorVariable* dxLightPosA;
	ID3DX11EffectVectorVariable* dxCameraPos;

	//Lights
	ID3DX11EffectVectorVariable* dxLightColourA;
	ID3DX11EffectVectorVariable* dxAmbientColour;
	ID3DX11EffectVariable*		dxLightBrightnessA;

	ID3DX11EffectVariable*		dxWiggle;
	ID3DX11EffectVariable*		dxOutlineThickness;

	ID3DX11EffectVectorVariable* ClipPlaneVar;

	//--------------------------------------------------------------------------------------
	// DirectX Variables
	//--------------------------------------------------------------------------------------

	// Variables used to setup D3D
	IDXGISwapChain*         SwapChain;
	ID3D11Texture2D*        DepthStencil;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11RenderTargetView* RenderTargetView;
	
	ID3D11ShaderResourceView* DepthShaderView;

	// Variables used to setup the Window
	HINSTANCE HInst;
	HWND      HWnd;

	// Choose the lights to send to the shader
	void SetLights( DirectX::XMFLOAT3 source, CLight* lightsSource[MAX_LIGHTS], int lightCount);
	// Sort function for the lights - static for function pointer.
	static inline int compareLights( const void* a, const void* b);

	// Message Handler - static for function pointer.
	static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	inline void DrawObject(int i, bool mirror = false);
	inline void DrawAllObjects(bool mirror);

	void DisplayMapText(INT32 offset);

	void DisplayText( char text[], UINT32 line );

	bool LoadShaders();

	bool LoadTechnique();

	//float* MatrixFix(DirectX::XMMATRIX input);

public:
	CScene(void);
	~CScene(void);


	bool InitDevice();
	void ReleaseResources();
	bool LoadEffectFile();
	bool InitScene();
	void UpdateScene( float frameTime );
	void VisualisePath();
	void RenderScene();
	void SetSelf(CScene* pointer){ mp_self = pointer; }
	//void RenderMirrors();
	bool InitWindow( HINSTANCE hInstance, int nCmdShow );

};

