//Matricies
float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;

//Camera
float3 cameraPos;
float4 ClipPlane;

//Model Data
float3 ModelColour;

//Light Data
static const uint MAX_LIGHTS = 12;
float3 ambientColour;
float4 lightPos[MAX_LIGHTS] : LIGHTPOS;
float4 lightColour[MAX_LIGHTS] : COLOR0;

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
// Techniques
//--------------------------------------------------------------------------------------

//----------------
// Single Colour
//----------------

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
	return float4(ModelColour,1.0);
}

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

//----------------
// Lit
//----------------

struct VS_LIT_INPUT
{
	float3 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD0;
};

struct VS_LIT_OUTPUT
{
	float4 ProjPos		: SV_POSITION;
	float4 worldPos		: POSITION;
	float4 worldNormal	: NORMAL;
	float2 UV			: TEXCOORD0;
	float  ClipDist : SV_ClipDistance;
};

VS_LIT_OUTPUT VSLit( VS_LIT_INPUT vIn )
{
	VS_LIT_OUTPUT vOut;

	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
		vOut.worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos = mul( vOut.worldPos, ViewMatrix );
		vOut.ProjPos = mul( viewPos, ProjMatrix );

	// Now the normals
	float4 modelNormal = float4(vIn.Normal, 0.0f); //promote to 4 so we can multiply
		vOut.worldNormal = mul( modelNormal, WorldMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	vOut.ClipDist = dot( vOut.worldPos, ClipPlane );

	return vOut;
}

float4 PSLit( VS_LIT_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize( cameraPos - vOut.worldPos.xyz );

	float3 diffuseLight = ambientColour;
	float3 specularLight;

	for( uint i = 0; i < MAX_LIGHTS; i++ )
	{
		float3 lightVector = normalize(vOut.worldPos.xyz - lightPos[i].xyz);
		diffuseLight += lightColour[i].xyz * max( dot( worldNormal.xyz, lightVector ), 0 );
		float3 halfway1 = normalize( lightVector + cameraVector );
		specularLight += lightColour[i].xyz * pow( max( dot( worldNormal.xyz, halfway1 ), 0 ), 256 );
	}

	//float4 texColour = DiffuseMap.Sample( Trilinear, vOut.UV );
	return (float4(ModelColour, 1.0) * float4(diffuseLight, 1.0)) + float4(specularLight, 1.0);
}

technique10 SingleColourLit
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VSLit()) );
		SetHullShader( NULL );
		SetDomainShader( NULL );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PSLit()) );

		SetBlendState( NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetRasterizerState( CullNone );
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

