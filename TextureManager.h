#pragma once

#include "Defines.h"

class CTextureManager
{
private:
	static const int MAX_TEXTURES = 100;
	static const int MAX_NAME_LENGTH = 50;
	ID3D11Device* mpd3dDevice;
	ID3D10ShaderResourceView* mpTextures[MAX_TEXTURES];
	char mcTexturesFile[MAX_TEXTURES][MAX_NAME_LENGTH];
	int miNumTextures;
public:
	ID3D10ShaderResourceView* LoadTexture ( char filename[50] );
	void UnloadTexture( char filename[50] );
	void UnloadTexture( int index );
	CTextureManager(ID3D11Device* device);
	~CTextureManager(void);
};

