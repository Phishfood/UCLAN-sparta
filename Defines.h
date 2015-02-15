
//--------------------------------------------------------------------------------------
//	Defines.h
//
//	General definitions shared across entire project
//--------------------------------------------------------------------------------------
#pragma once;
#ifndef DEFINES_H_INCLUDED // Header guard - prevents file being included more than once (would cause errors)
#define DEFINES_H_INCLUDED


#include <Windows.h>

//DX 10. Should be able to remove this.
//#include <d3d10.h>
//#include <d3dx10.h>

#include <fstream>

//DX 11
#include <D3D11.h>
#include <dxgi.h>
#include <d3dx11.h>
//#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <D3DX11async.h>
#include "d3dx11effect.h"
#include <DirectXPackedVector.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "D3DX11.lib")

//#pragma comment(lib, "Effects11D.lib")

#pragma comment(lib, "Effects11.lib")

#pragma comment(lib, ".\\lib\\AntTweakBar.lib")
#include "AntTweakBar\include\AntTweakBar.h"


//FMOD

#include "Import\fmod\fmod.hpp"
#pragma comment(lib, ".\\Import\\fmod\\fmodexL_vc.lib")

//guaranteed variable sizes. 
#include <stdint.h>

//Break people's brains.
//#define int auto

//consts for array sizes

const int32_t MAX_MAP_SIZE = 256;
const int32_t MAX_MODEL_INSTANCES = 1000;

/******************************************************************
*  CONVERT DX10 TO DX11 
******************************************************************/

//#define DirectX::XMFLOAT4 DirectX::XMFLOAT4
//#define DirectX::XMFLOAT3 DirectX::XMFLOAT3
//#define DirectX::XMFLOAT2 DirectX::XMFLOAT2
//#define DirectX::XMMATRIX DirectX::XMMATRIX 
//#define D3DXMATRIX DirectX::XMATRIX
//#define D3DXCOLOR DirectX::XMFLOAT4

#define ID3D10EffectTechnique ID3DX11EffectTechnique
#define ID3D10ShaderResourceView ID3D11ShaderResourceView

const DirectX::XMVECTOR XAXIS = DirectX::XMVectorSet(1.0f,0.0f,0.0f,1.0f);
const DirectX::XMVECTOR YAXIS = DirectX::XMVectorSet(0.0f,1.0f,0.0f,1.0f);
const DirectX::XMVECTOR ZAXIS = DirectX::XMVectorSet(0.0f,0.0f,1.0f,1.0f);

const DirectX::XMFLOAT3 ORIGIN = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
const DirectX::XMFLOAT3 NO_ROTATION = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );

const DirectX::XMFLOAT3 _BLACK = DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f );
const DirectX::XMFLOAT3 _WHITE = DirectX::XMFLOAT3( 1.0f, 1.0f, 1.0f );
const DirectX::XMFLOAT3 _RED = DirectX::XMFLOAT3( 0.8f, 0.0f, 0.0f );
const DirectX::XMFLOAT3 _MRED = DirectX::XMFLOAT3( 0.8f, 0.3f, 0.3f);
const DirectX::XMFLOAT3 _LRED = DirectX::XMFLOAT3( 0.8f, 0.6f, 0.6f);
const DirectX::XMFLOAT3 _GREEN = DirectX::XMFLOAT3( 0.0f, 0.8f, 0.0f );
const DirectX::XMFLOAT3 _ORANGE = DirectX::XMFLOAT3(1.0f, 0.5f, 0.1f);


struct V3 
{
	float x;
	float y;
	float z;
};

V3 XMF3ToFloat3( DirectX::XMFLOAT3 in);

//-----------------------------------------------------------------------------
// Helper functions and macros
//-----------------------------------------------------------------------------

// Helper macro to release DirectX pointers only if they are not NULL
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }

// Angular helper functions to convert from degrees to radians and back (D3DX_PI is a double)
#define D3DX_PI 3.14159265359
inline float ToRadians( float deg ) { return deg * (float)D3DX_PI / 180.0f ; }
inline float ToDegrees( float rad ) { return rad * 180.0f / (float)D3DX_PI ; }
inline float square (float x) { return x * x; }

//#define MAX(a,b) ((a) > (b) ? (a) : (b))
//#define MIN(a,b) ((a) < (b) ? (a) : (b))

//#define SQUARE(x) (x*x)
//#define CUBE(x) (x*x*x)

enum MDOffset
{
	COST_OFFSET,
	WALL_OFFSET,
	PATH_OFFSET,
	BASE_OFFSET,
	BASE2_OFFSET,
	LIGHT_OFFSET,
	MEDIUM_OFFSET,
	HEAVY_OFFSET
};

//helps keep track of all the techniques - rather than just using the index alone.

enum TechniqueNames {
	PlainColour,
	PlainTexture,
	ColourChangeTexture,
	LitTexture,
	MultiLitTexture,
	MultiLitAttenuatedTexture,
	SpecularMapped,
	ParallaxMapped,
	Jelly,
	CellShaded,
	BadCellShaded,
	Halo,
	TripleHaloPara,
	TripleOutlinePara,
	LitGlass,
	SpaceHalo,
	TripleOutline,
	TextureMorph,
	MirrorClear,
	MirrorSurface
};

#endif // End of header guard - see top of file
