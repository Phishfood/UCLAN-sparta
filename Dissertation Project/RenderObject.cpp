#include "RenderObject.h"


/*CRenderObject::CRenderObject(void)
{
	mModel = new CModel;				
	mr = 1.0f;
	mg = 1.0f;
	mb = 1.0f;							
	mTechnique = nullptr;		
	mTexture = nullptr;		
	mNormMap = nullptr;		
	mIsLit = false;							
}*/

CRenderObject::CRenderObject( 
							 char file[30],  
							 D3DXVECTOR3 position, 
							 D3DXVECTOR3 colour, 
							 ID3D10EffectTechnique* technique, 
							 ID3D10EffectTechnique* mirrorTechnique,
							 ID3D10ShaderResourceView* texture, 
							 ID3D10ShaderResourceView* normmap, 
							 bool lit, 
							 bool transparent,
							 bool tangents
							 )
{
	mModel = new CModel(position, D3DXVECTOR3(0,0,0), 1.0f);
	mModel->Load( file, technique, tangents );
	//mModel->SetPosition( position );

	mr = colour.x;
	mg = colour.y;
	mb = colour.z;

	mTechnique = technique;
	mMirrorTechnique = mirrorTechnique;

	mTexture = texture;

	mNormMap = normmap;

	mIsLit = lit;

	mIsTransparent = transparent;

	mColourCycle = false;

	mCumulativeTime = 0.0f;

	mSpinX = false;
	mSpinY = false;
	mSpinZ = false;

	mWiggle = false;
	mWiggleRate = 0.0f;
	mWiggleValue = 0.0f;

	mOutlineThickness = 0.05f;
}

CRenderObject::~CRenderObject(void)
{
	delete mModel;
}

void CRenderObject::SetColour(float red, float green, float blue) 
{ 
	mr = fmodf(red, 1.001f); 
	mg = fmodf(green, 1.001f); 
	mb = fmodf(blue, 1.001f); 
}

void CRenderObject::SetColour(int red, int green, int blue)
{
	mr = float((red % 256) / 255);
	mg = float((green % 256) / 255);
	mb = float((blue % 256) / 255);
}

void CRenderObject::Update(float frameTime)
{
	mCumulativeTime += frameTime;
	if( mColourCycle )
	{
		float hue = fmodf(mCumulativeTime, 1.0f);
		fHSLToRGB( hue, 0.5f, 0.5f,mr, mg, mb);
	}

	D3DXVECTOR3 currRotation = mModel->GetRotation();

	if( mSpinX )
	{
		currRotation.x += mSpinXRate * frameTime;
	}

	if( mSpinY )
	{
		currRotation.y += mSpinYRate * frameTime;
	}

	if( mSpinZ )
	{
		currRotation.z += mSpinZRate * frameTime;
	}

	mWiggleValue += frameTime * mWiggleRate;

	mModel->SetRotation(currRotation);

	mModel->UpdateMatrix();
}
