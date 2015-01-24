#pragma once

float Min( float f1, float f2, float f3 );
float Max( float f1, float f2, float f3 );
void RGBToHSL( int R, int G, int B, int& H, int& S, int& L );
void HSLToRGB( int H, int S, int L, int& R, int& G, int& B );
void fHSLToRGB(float h, float sl, float l, float& rout, float& gout, float& bout);