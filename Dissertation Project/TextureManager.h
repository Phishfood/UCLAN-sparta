#pragma once

#include <d3d10.h>
#include <d3dx10.h>

class CTextureManager
{
private:
	static const int MAX_TEXTURES = 100;
	static const int MAX_NAME_LENGTH = 50;
	ID3D10Device* mpd3dDevice;
	ID3D10ShaderResourceView* mpTextures[MAX_TEXTURES];
	char mcTexturesFile[MAX_TEXTURES][MAX_NAME_LENGTH];
	int miNumTextures;
public:
	ID3D10ShaderResourceView* LoadTexture ( char filename[50] );
	void UnloadTexture( char filename[50] );
	void UnloadTexture( int index );
	CTextureManager(ID3D10Device* device);
	~CTextureManager(void);
};

