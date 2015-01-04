
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
//s#include <d3dx10.h>

//DX 11
#include <D3D11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <D3DX11async.h>
#include <DirectXPackedVector.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

//guaranteed variable sizes. 
#include <stdint.h>

//Break people's brains.
//#define int auto

//consts - later, extract these to an ini file or something.

const INT32 MAX_MAP_SIZE = 256;

const INT32 WALL_INFLUENCE_RANGE = 5;
const INT32 BASE_INFLUENCE_RANGE = 15;

/******************************************************************
*  CONVERT DX10 TO DX11 
******************************************************************/

#define D3DXVECTOR4 DirectX::XMFLOAT4
#define D3DXVECTOR3 DirectX::XMFLOAT3
#define D3DXVECTOR2 DirectX::XMFLOAT2
#define D3DXMATRIXA16 DirectX::XMMATRIX 
#define D3DXMATRIX DirectX::XMATRIX
#define D3DXCOLOR DirectX::XMFLOAT4

const DirectX::XMVECTOR XAXIS = DirectX::XMVectorSet(1.0f,0.0f,0.0f,1.0f);
const DirectX::XMVECTOR YAXIS = DirectX::XMVectorSet(0.0f,1.0f,0.0f,1.0f);
const DirectX::XMVECTOR ZAXIS = DirectX::XMVectorSet(0.0f,0.0f,1.0f,1.0f);

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

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define SQUARE(x) (x*x)
#define CUBE(x) (x*x*x)

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
