#pragma once

#include "Model.h"
#include <d3d10.h>
#include <d3dx10.h>

#include "Colours.h"

class CRenderObject
{
private:
	CModel* mModel;							//the model
	float mr,mg,mb;							//base colour - will be overwritten with texture
	ID3D10EffectTechnique* mTechnique;	//pointer to the shaders to be used.
	ID3D10EffectTechnique* mMirrorTechnique;	//pointer to the shaders to be used.
	ID3D10ShaderResourceView* mTexture;		//pointer to the base texture (diffuse map)
	ID3D10ShaderResourceView* mNormMap;		//pointer to the normal map
	bool mIsLit;							//indicates if the model accounts for lighting - if not, skip the lighting related tasks.
	bool mIsTransparent;

	float mCumulativeTime;

	bool mColourCycle;

	bool mSpinX;
	bool mSpinY;
	bool mSpinZ;
	float mSpinXRate;
	float mSpinYRate;
	float mSpinZRate;

	bool mWiggle;
	float mWiggleRate;
	float mWiggleValue;

	float mOutlineThickness;

public:
	//CRenderObject(void);
	CRenderObject( 
					char file[30], 
					D3DXVECTOR3 position, 
					D3DXVECTOR3 colour, 
					ID3D10EffectTechnique* technique, 
					ID3D10EffectTechnique* mirrorTechnique,
					ID3D10ShaderResourceView* texture = nullptr, 
					ID3D10ShaderResourceView* normmap = nullptr, 
					bool lit = false, 
					bool transparent = false,
					bool tangents = false
					);
	~CRenderObject(void);

	CModel* GetModel() { return mModel; }
	void SetModel( CModel* model ) { mModel = model; }

	void GetColour(float& red, float& green, float& blue) { red = mr; green = mg; blue = mb; }
	void GetColour(int& red, int& green, int& blue){ red = int(mr * 255); green = int(mg * 255); blue = int(mb * 255); } 
	D3DXVECTOR3 GetColourV(){ return D3DXVECTOR3( mr, mg, mb ); }

	void SetColour(float red, float green, float blue);
	void SetColour(int red, int green, int blue);

	ID3D10EffectTechnique* GetTechnique() { return mTechnique; }
	ID3D10EffectTechnique* GetMirrorTechnique() { return mMirrorTechnique; }
	void SetTechnique(ID3D10EffectTechnique* technique) { mTechnique = technique; }
	void SetMirrorTechnique(ID3D10EffectTechnique* technique) { mMirrorTechnique = technique; }

	ID3D10ShaderResourceView* GetTexture() { return mTexture; }
	void SetTexture(ID3D10ShaderResourceView* texture) { mTexture = texture; }

	ID3D10ShaderResourceView* GetNormalMap() { return mNormMap; }
	void SetNormalMap(ID3D10ShaderResourceView* normMap) { mNormMap = normMap; }

	bool IsLit() { return mIsLit; }

	void SetLit() { mIsLit = true; }
	void SetNotLit() { mIsLit = false; }

	bool IsTransparent() { return mIsTransparent; }

	void SetTransparent() { mIsTransparent = true; }
	void SetNotTransparent() { mIsTransparent = false; }

	void Render() { mModel->Render( mTechnique ); }
	void RenderMirror() { mModel->Render( mMirrorTechnique ); }
	
	void SetColourCycle() { mColourCycle = true; }
	void StopColourCycle() { mColourCycle = false; }
	
	void SetSpinX( float rate ) { mSpinX = true; mSpinXRate = rate; }
	void SetSpinY( float rate ) { mSpinY = true; mSpinYRate = rate; }
	void SetSpinZ( float rate ) { mSpinZ = true; mSpinZRate = rate; }

	void StopSpinX() { mSpinX = false; }
	void StopSpinY() { mSpinY = false; }
	void StopSpinZ() { mSpinZ = false; }

	void SetWiggle( float rate ) { mWiggle = true; mWiggleRate = rate; }
	void StopWiggle() { mWiggle = false; }
	float GetWiggle() { return mWiggleValue; }

	float GetOutlineThickness() { return mOutlineThickness; }

	void Update(float frameTime);

};

