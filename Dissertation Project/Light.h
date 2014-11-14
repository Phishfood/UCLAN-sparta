#pragma once

#include "Model.h"
#include "Colours.h"
#include <d3d10.h>
#include <d3dx10.h>

enum lightType
{
	POINTLIGHT,
	DIRECTIONALLIGHT,
	SPOTLIGHT
};

class CLight
{
private:
	CModel* mModel;			//model for light source
	
	//Light attributes
	float mr,mg,mb;			//light colour
	lightType mType;		//light type
	float mSize;			//angle for spot, diameter for directional
	float mBrightness;		//intensity of the light - ie, how far it will spread

	//Light Behavior
	float mCumulativeTime;

	bool mOrbitPoint;
	bool mOrbitObject;
	CModel* mOrbiting;
	D3DXVECTOR3 mOrbitingPoint;
	float mOrbitRadius;
	float mOrbitSpeed;

	bool mCycleColour;
	float mColourRate;

	bool mPulse;
	float mPulseRate;
	float mOldBright;

	bool mFade;
	bool mFadeDir;
	float mFadeRate;

	bool mDisco;
	

public:
	CLight(void);
	~CLight(void);

	void GetColour(float& red, float& green, float& blue) { red = mr; green = mg; blue = mb; }
	void GetColour(int& red, int& green, int& blue){ red = int(mr * 255); green = int(mg * 255); blue = int(mb * 255); } 
	D3DXVECTOR3 GetColourV(){ return D3DXVECTOR3( mr, mg, mb ); }
	
	void SetColour(float red, float green, float blue);
	void SetColour(int red, int green, int blue);

	lightType GetType(){ return mType; }
	void SetType(lightType type){ mType = type; }

	float GetSize() { return mSize; }
	void SetSize(float size) { mSize = size; }

	CModel* GetModel() { return mModel; }
	void SetModel( CModel* model ) { mModel = model; }

	float GetBrightness() { return mBrightness; }
	void SetBrightness(float brightness) { mBrightness = brightness; }

	void SetOrbit(CModel* model, float radius, float speed);
	void SetOrbit(D3DXVECTOR3 point, float radius, float speed);
	void StopOrbit();

	void SetColourCycle( float rate );
	void StopColourCycle();

	void SetPulse( float rate );
	void StopPulse();

	void SetFade( float rate );
	void StopFade();

	void SetDisco() { mDisco = true; mPulse = false; mCycleColour = false;}
	void StopDisco() { mDisco = false; }

	void Update(float frameTime);
};

