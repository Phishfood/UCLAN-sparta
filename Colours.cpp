#include "Colours.h"

//--------------------------------------------------------------------------------------
// Colour helper functions
//--------------------------------------------------------------------------------------

void AdvanceHue( int amount, float& R, float& G, float& B )
{
	int H, S, L;
	RGBToHSL( int(R*255.0f), int(G*255.0f), int(B*255.0f), H, S, L);
	H += amount;
	H %= 360;
	int newR, newG, newB;
	HSLToRGB( H, S, L, newR, newG, newB);
	R = float( newR/255.0f );
	G = float( newG/255.0f );
	B = float( newB/255.0f );
}

// Find the minimum of three numbers
float Min( float f1, float f2, float f3 )
{
	float fMin = f1;
	if (f2 < fMin)
	{
		fMin = f2;
	}
	if (f3 < fMin)
	{
		fMin = f3;
	}
	return fMin;
}

// Find the maximum of three numbers
float Max( float f1, float f2, float f3 )
{
	float fMax = f1;
	if (f2 > fMax)
	{
		fMax = f2;
	}
	if (f3 > fMax)
	{
		fMax = f3;
	}
	return fMax;
}

// Convert an RGB colour to a HSL colour
void RGBToHSL( int R, int G, int B, int& H, int& S, int& L )
{
	// Fill in the correct code here for question 4, the functions Min and Max above will help
	float fR, fG, fB, fMax, fMin;
	
	//convert int to float 0-1
	fR = (float) R / 255.0f;
	fG = (float) G / 255.0f;
	fB = (float) B / 255.0f;

	//find max and min values
	fMin = Min(fR, fG, fB);
	fMax = Max(fR, fG, fB);


	L = int( 50 * (fMax +fMin) );
	
	//greys - catch div/0 error
	if(fMax == fMin){
		H=0;
		S=0;
		 return;
	}

	if(L < 50){
		S = int ( 100 * (fMax - fMin) / (fMax + fMin) );
	}else{
		S = int (100 * (fMax - fMin) / (2.0 - (fMax + fMin)) );
	}


	if(fMax == fR){
		H = int ( 60 * ( (fG - fB) / (fMax - fMin) ) );
	}else if(fMax == fG){
		H = int ( 60 * ( (fB - fR) / (fMax - fMin) ) + 120 );
	}else{
		H = int ( 60 * ( (fR - fG) / (fMax - fMin) ) + 240 );
	}

	if(H < 0){
		H += 360;
	}
	return;

}


// Convert a HSL colour to an RGB colour
void HSLToRGB(int hin, int slin, int lin, int& rout, int& gout, int& bout)
{
	float h = hin / 360.0f;
	float sl = slin / 100.0f;
	float l = lin / 100.0f;
    float v;
    float r,g,b;
 
    r = l;   // default to gray
    g = l;
    b = l;
    v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);
    if (v > 0)
    {
            float m;
            float sv;
            int sextant;
            float fract, vsf, mid1, mid2;
 
            m = l + l - v;
            sv = (v - m ) / v;
            h *= 6.0;
            sextant = (int)h;
            fract = h - sextant;
            vsf = v * sv * fract;
            mid1 = m + vsf;
            mid2 = v - vsf;
            switch (sextant)
            {
                case 0:
                        r = v;
                        g = mid1;
                        b = m;
                        break;
                case 1:
                        r = mid2;
                        g = v;
                        b = m;
                        break;
                case 2:
                        r = m;
                        g = v;
                        b = mid1;
                        break;
                case 3:
                        r = m;
                        g = mid2;
                        b = v;
                        break;
                case 4:
                        r = mid1;
                        g = m;
                        b = v;
                        break;
                case 5:
                        r = v;
                        g = m;
                        b = mid2;
                        break;
            }
    }
	rout = int( r * 255.0f );
	bout = int( b * 255.0f );
	gout = int( g * 255.0f );
    return;
}

void fHSLToRGB(float h, float sl, float l, float& r, float& g, float& b)
{
    float v;

    r = l;   // default to gray
    g = l;
    b = l;
    v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);
    if (v > 0)
    {
            float m;
            float sv;
            int sextant;
            float fract, vsf, mid1, mid2;
 
            m = l + l - v;
            sv = (v - m ) / v;
            h *= 6.0;
            sextant = (int)h;
            fract = h - sextant;
            vsf = v * sv * fract;
            mid1 = m + vsf;
            mid2 = v - vsf;
            switch (sextant)
            {
                case 0:
                        r = v;
                        g = mid1;
                        b = m;
                        break;
                case 1:
                        r = mid2;
                        g = v;
                        b = m;
                        break;
                case 2:
                        r = m;
                        g = v;
                        b = mid1;
                        break;
                case 3:
                        r = m;
                        g = mid2;
                        b = v;
                        break;
                case 4:
                        r = mid1;
                        g = m;
                        b = v;
                        break;
                case 5:
                        r = v;
                        g = m;
                        b = mid2;
                        break;
            }
    }
    return;
}