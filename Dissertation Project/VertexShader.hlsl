float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;



struct VS_BASIC_INPUT
{
    float3 Pos    : POSITION;
	float2 UV     : TEXCOORD0;
};

struct VS_BASIC_OUTPUT
{
    float4 ProjPos : SV_POSITION;
	float  ClipDist    : SV_ClipDistance;
};

VS_BASIC_OUTPUT main ( VS_BASIC_INPUT vIn )
{
	VS_BASIC_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	//vOut.ClipDist = dot( worldPos, ClipPlane );
	vOut.ClipDist = 0;
	return vOut;
}