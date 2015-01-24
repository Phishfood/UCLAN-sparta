float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;

float3 ModelColour;

//-------------------------------------------------------------------------------------
// STRUCTS
//-------------------------------------------------------------------------------------

struct VS_BASIC_INPUT
{
	float3 Pos    : POSITION;
	float2 UV     : TEXCOORD0;
};

struct VS_BASIC_OUTPUT
{
	float4 ProjPos : SV_POSITION;
	float  ClipDist : SV_ClipDistance;
};


//--------------------------------------------------------------------------------------
// States
//--------------------------------------------------------------------------------------

// States are needed to switch between additive blending for the lights and no blending for other models

RasterizerState CullNone  // Cull none of the polygons, i.e. show both sides
{
	CullMode = None;
	FillMode = SOLID;
};
RasterizerState CullBack  // Cull back side of polygon - normal behaviour, only show front of polygons
{
	CullMode = Back;
	FillMode = SOLID;
};
RasterizerState Wireframe
{
	CullMode = None;
	FillMode = WIREFRAME;
};

DepthStencilState DepthWritesOff // Don't write to the depth buffer - polygons rendered will not obscure other polygons
{
	DepthFunc = LESS;
	DepthWriteMask = ZERO;
};
DepthStencilState DepthWritesOn  // Write to the depth buffer - normal behaviour 
{
	DepthFunc = LESS;
	DepthWriteMask = ALL;
};
DepthStencilState DisableDepth   // Disable depth buffer entirely
{
	DepthFunc = ALWAYS;
	DepthWriteMask = ZERO;
};

BlendState NoBlending // Switch off blending - pixels will be opaque
{
	BlendEnable[0] = FALSE;
};
BlendState AdditiveBlending // Additive blending is used for lighting effects
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};
BlendState AlphaBlending // Alpha blending is used for the particles
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
};


//--------------------------------------------------------------------------------------
// SHADERS
//--------------------------------------------------------------------------------------

VS_BASIC_OUTPUT VS_Basic( VS_BASIC_INPUT vIn )
{
	VS_BASIC_OUTPUT vOut;

	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
		float4 worldPos = mul( modelPos, WorldMatrix );
		float4 viewPos = mul( worldPos, ViewMatrix );
		vOut.ProjPos = mul( viewPos, ProjMatrix );

	//vOut.ClipDist = dot( worldPos, ClipPlane );
	vOut.ClipDist = 0;
	return vOut;
}

float4 PS_Basic( VS_BASIC_OUTPUT vOut ) : SV_TARGET
{
	return float4(ModelColour, 1.0);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

// Techniques are used to render models in our scene. They select a combination of vertex, geometry and pixel shader from those provided above. Can also set states.

// Displacement mapping using tessellation
technique11 SingleColour
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS_Basic()) );
		SetHullShader( NULL );
		SetDomainShader( NULL );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS_Basic()) );

		// Switch off blending states
		SetBlendState( NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetRasterizerState( CullNone );
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}