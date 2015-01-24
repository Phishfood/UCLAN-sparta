cbuffer attributes
{
	float3 ModelColour;
}

struct VS_BASIC_OUTPUT
{
    float4 ProjPos : SV_POSITION;
	float  ClipDist    : SV_ClipDistance;
};

float4 main(VS_BASIC_OUTPUT vOut) : SV_TARGET
{
	return float4( ModelColour, 1.0 ); 
}