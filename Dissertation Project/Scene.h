#pragma once

//External includes
#include <atlbase.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "resource.h"
#include "D3DCompiler.h"
#include "D3DX11async.h"

#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "D3DX11.lib")
//#pragma comment(lib, "D3DX11EffectsD.lib")
#pragma comment(lib, "Effects11D.lib")
//Local includes
#include "Defines.h"    // General definitions shared by all source files
#include "Model.h"      // Model class - encapsulates working with vertex/index data and world matrix
#include "Camera.h"     // Camera class - encapsulates the camera's view and projection matrix
#include "CTimer.h"     // Timer class - not DirectX
#include "Input.h"      // Input functions - not DirectX
#include "MapSquare.h"  // Storage for base map data and influence maps

#include "Light.h"			// Light class.
#include "RenderObject.h"	// Render class.

#include "Colours.h"		// Functions to adjust colours



class CScene
{
private:
	// Map data
	MapSquare* mc_map;
	D3DXMATRIXA16 md_mapMatrix[1000];
	D3DXVECTOR3 md_mapColours[1000];
	INT32 mi_numSquares;
	CModel* mp_mapModel;
	
	// Text rendering
	RECT FontRect;

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
	CCamera* Camera;

	CRenderObject* mpObjects[MAX_OBJECTS];
	int miNumObjects;

	CLight* mpLights[MAX_LIGHTS];
	int miNumLights;

	// Textures - no texture class yet so using DirectX variables
	// * 2 since each object can potentially have 2 textures, although this is likely overkill.
	ID3D10ShaderResourceView* mpMaps[MAX_OBJECTS*2];
	int miNumMaps;

	// Ambient Light
	D3DXVECTOR3 AmbientColour;

	//defines the object to be controlled via keyboard
	int mControlObject;

	// Note: There are move & rotation speed constants in Defines.h


	//--------------------------------------------------------------------------------------
	// Shader Variables
	//--------------------------------------------------------------------------------------
	// Variables to connect C++ code to HLSL shaders

	// Effects / techniques - No longer used, delete?
	ID3D10Effect*          Effect;
	ID3D10EffectTechnique* mTechniques[30];
	ID3D10EffectTechnique* mTechniquesMirror[30];
	int miNumTechniques;

	// Shaders
	ID3D11PixelShader*  mp_PixelShader[30];
	ID3D11VertexShader* mp_VertexShader[30];

	// Matrices
	ID3D10EffectMatrixVariable* WorldMatrixVar;
	ID3D10EffectMatrixVariable* ViewMatrixVar;
	ID3D10EffectMatrixVariable* ProjMatrixVar;
	ID3D10EffectMatrixVariable* ViewProjMatrixVar;

	// Textures
	ID3D10EffectShaderResourceVariable* DiffuseMapVar;
	ID3D10EffectShaderResourceVariable* NormalMapVar;

	// Miscellaneous
	ID3D10EffectVectorVariable* ModelColourVar;

	//Positions
	ID3D10EffectVectorVariable* dxLightPosA;
	ID3D10EffectVectorVariable* dxCameraPos;

	//Lights
	ID3D10EffectVectorVariable* dxLightColourA;
	ID3D10EffectVectorVariable* dxAmbientColour;
	ID3D10EffectVariable*		dxLightBrightnessA;

	ID3D10EffectVariable*		dxWiggle;
	ID3D10EffectVariable*		dxOutlineThickness;

	ID3D10EffectVectorVariable* ClipPlaneVar;

	//--------------------------------------------------------------------------------------
	// DirectX Variables
	//--------------------------------------------------------------------------------------

	// Variables used to setup D3D
	IDXGISwapChain*         SwapChain;
	ID3D11Texture2D*        DepthStencil;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11RenderTargetView* RenderTargetView;
	

	// Variables used to setup the Window
	HINSTANCE HInst;
	HWND      HWnd;

	// Choose the lights to send to the shader
	void SetLights( D3DXVECTOR3 source, CLight* lightsSource[MAX_LIGHTS], int lightCount);
	// Sort function for the lights - static for function pointer.
	static inline int compareLights( const void* a, const void* b);

	// Message Handler - static for function pointer.
	static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	inline void DrawObject(int i, bool mirror = false);
	inline void DrawAllObjects(bool mirror);

	void DisplayMapText(INT32 offset);

	void DisplayText( char text[], UINT32 line );

public:
	CScene(void);
	~CScene(void);


	bool InitDevice();
	void ReleaseResources();
	bool LoadEffectFile();
	bool InitScene();
	void UpdateScene( float frameTime );
	void RenderScene();
	//void RenderMirrors();
	bool InitWindow( HINSTANCE hInstance, int nCmdShow );

};

