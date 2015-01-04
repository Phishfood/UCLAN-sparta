#include "TextureManager.h"


CTextureManager::CTextureManager(ID3D11Device* device)
{
	mpd3dDevice = device;
}


CTextureManager::~CTextureManager(void)
{
}

ID3D10ShaderResourceView* CTextureManager::LoadTexture ( char filename[50] )
{
	
	for( int i = 0; i < miNumTextures; i++ )
	{
		if( strcmp( filename, mcTexturesFile[i] ) == 0 )
		{
			return mpTextures[i];
		}
	}
	
	ID3D11ShaderResourceView* newTexture;
	D3DX11CreateShaderResourceViewFromFileA( mpd3dDevice, filename,		NULL, NULL, &newTexture, NULL );
	if( newTexture == nullptr )
	{
		return nullptr;
	}

	mpTextures[miNumTextures] = newTexture;
	miNumTextures++;
	
	return nullptr;

}

void CTextureManager::UnloadTexture( char filename[50] )
{

}

void CTextureManager::UnloadTexture( int index )
{

}