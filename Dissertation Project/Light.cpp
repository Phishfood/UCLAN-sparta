#include "Light.h"


CLight::CLight(void)
{
	mBrightness = 10.0f;
	mCumulativeTime = 0.0f;
	mOrbitObject = false;
	mOrbitPoint = false;
	mPulse = false;
	mCycleColour = false;
	mFade = false;
	mDisco = false;
}


CLight::~CLight(void)
{
	delete mModel;
}

void CLight::SetColour(float red, float green, float blue) 
{ 
	mr = fmodf(red, 1.001f); 
	mg = fmodf(green, 1.001f); 
	mb = fmodf(blue, 1.001f); 
}

void CLight::SetColour(int red, int green, int blue)
{
	mr = float((red % 256) / 255);
	mg = float((green % 256) / 255);
	mb = float((blue % 256) / 255);
}


void CLight::SetOrbit(CModel* model, float radius, float speed)
{
	mOrbiting = model;
	mOrbitRadius = radius;
	mOrbitSpeed = speed;
	mOrbitPoint = false;
	mOrbitObject = true;
}

void CLight::SetOrbit(D3DXVECTOR3 point, float radius, float speed)
{
	mOrbitingPoint = point;
	mOrbitRadius = radius;
	mOrbitSpeed = speed;
	mOrbitPoint = true;
	mOrbitObject = false;
}

void CLight::StopOrbit()
{
	mOrbitObject = false;
	mOrbitPoint = false;
}

void CLight::SetColourCycle( float rate )
{
	mColourRate = rate;
	mCycleColour = true;
}

void CLight::StopColourCycle()
{
	mCycleColour = false;
}

void CLight::SetPulse( float rate )
{
	mPulse = true;
	mPulseRate = rate;
	mOldBright = mBrightness;
}

void CLight::StopPulse()
{
	mPulse = false;
}

void CLight::SetFade( float rate )
{
	mPulse = false;
	mFade = true;
	mFadeRate = rate;
	mFadeDir = true;
	mOldBright = mBrightness;
}

void CLight::StopFade()
{
	mFade = false;
}

void CLight::Update(float frameTime)
{
	mCumulativeTime += frameTime;
	if( mOrbitPoint )
	{
		mModel->SetPosition( mOrbitingPoint + D3DXVECTOR3(cos(mCumulativeTime * mOrbitSpeed)* mOrbitRadius, 0, sin(mCumulativeTime * mOrbitSpeed) * mOrbitRadius) );
	}

	if( mOrbitObject )
	{
		mModel->SetPosition( mOrbiting->GetPosition() + D3DXVECTOR3(cos(mCumulativeTime * mOrbitSpeed)* mOrbitRadius, 0, sin(mCumulativeTime * mOrbitSpeed) * mOrbitRadius) );
	}

	if( mCycleColour )
	{
		float hue = fmodf(mCumulativeTime, mColourRate) / mColourRate;
		fHSLToRGB( hue, 0.5f, 0.5f,mr, mg, mb);
	}

	if( mPulse )
	{
		if( fmodf(mCumulativeTime, mPulseRate) < (mPulseRate / 2.0f) )
		{
			mBrightness = 0.0f;
		}
		else
		{
			mBrightness = mOldBright;
		}
	}

	if( mFade )
	{
		if( mFadeDir )
		{
			mBrightness -= mFadeRate * frameTime;
			if(mBrightness < 0)
			{
				mBrightness = 0;
				mFadeDir = false;
			}
		}
		else
		{
			mBrightness += mFadeRate * frameTime;
			if(mBrightness > mOldBright)
			{
				mBrightness = mOldBright;
				mFadeDir = true;
			}
		}
	}

	if( mDisco )
	{
		mr = (rand() % 256) / 255.0f;
		mg = (rand() % 256) / 255.0f;
		mb = (rand() % 256) / 255.0f;
	}

	mModel->UpdateMatrix();
}



