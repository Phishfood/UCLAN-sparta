
//--------------------------------------------------------------------------------------
//	Defines.h
//
//	General definitions shared across entire project
//--------------------------------------------------------------------------------------

#ifndef DEFINES_H_INCLUDED // Header guard - prevents file being included more than once (would cause errors)
#define DEFINES_H_INCLUDED

#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <stdint.h>

const int WALL_INFLUENCE_RANGE = 5;

//-----------------------------------------------------------------------------
// Helper functions and macros
//-----------------------------------------------------------------------------

// Helper macro to release DirectX pointers only if they are not NULL
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }

// Angular helper functions to convert from degrees to radians and back (D3DX_PI is a double)
inline float ToRadians( float deg ) { return deg * (float)D3DX_PI / 180.0f; }
inline float ToDegrees( float rad ) { return rad * 180.0f / (float)D3DX_PI; }
inline float square (float x) { return x * x; }


//helps keep track of all the techniques - rather than just using the index alone.

const UINT32 MAX_MAP_SIZE = 256;

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
