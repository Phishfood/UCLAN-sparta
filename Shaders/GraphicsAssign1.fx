//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// All these variables are created & manipulated in the C++ code and passed into the shader here

// The matrices (4x4 matrix of floats) for transforming from 3D model to 2D projection (used in vertex shader)
float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;

// A single colour for an entire model - used for light models and the intial basic shader
float3 ModelColour;

// Diffuse texture map (the main texture colour) - may contain specular map in alpha channel
Texture2D DiffuseMap;

// Normal Map
Texture2D NormalMap;

static const uint MAX_LIGHTS = 12;

float4 lightPos[MAX_LIGHTS] : LIGHTPOS;
float4 lightColour[MAX_LIGHTS] : COLOR0;
float4 lightBright[MAX_LIGHTS];

float3 cameraPos;

float3 ambientColour;

float wiggle;

float outline;

float passCount;

float OutlineThickness = 0.05;

float4 ClipPlane;


//--------------------------------------------------------------------------------------
// Sampler States
//--------------------------------------------------------------------------------------

//Bilinear filtering
SamplerState Bilinear
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

//Trilinear filtering
SamplerState Trilinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Blender States
//--------------------------------------------------------------------------------------

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

BlendState AdditiveBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
};

BlendState MultiplicativeBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = DEST_COLOR;
	BlendOp = ADD;
};

BlendState HaloBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = DEST_COLOR;
	DestBlend = ONE;
	BlendOp = ADD;
};

BlendState NoColourOutput // Use blending to prevent drawing pixel colours, but still allow depth/stencil updates
{
    BlendEnable[0] = TRUE;
    SrcBlend = ZERO;
    DestBlend = ONE;
    BlendOp = ADD;
};

//--------------------------------------------------------------------------------------
// Rasterizer States
//--------------------------------------------------------------------------------------

RasterizerState CullNone  // Cull none of the polygons, i.e. show both sides
{
	CullMode = None;
};
RasterizerState CullBack  // Cull back side of polygon - normal behaviour, only show front of polygons
{
	CullMode = Back;
};

RasterizerState CullFront  // Cull back side of polygon - unsusual behaviour for special techniques
{
	CullMode = Front;
};

//--------------------------------------------------------------------------------------
// Depth States
//--------------------------------------------------------------------------------------

DepthStencilState DepthWritesOff // Don't write to the depth buffer - polygons rendered will not obscure other polygons
{
	DepthWriteMask = ZERO;
	DepthFunc = LESS;
	StencilEnable = FALSE;
};

DepthStencilState DepthWritesOn  // Write to the depth buffer - normal behaviour 
{
	DepthWriteMask = ALL;
	DepthFunc = LESS;
	StencilEnable = FALSE;
};

//--------------------------------------------------------------------------------------
// Stencil Buffer
//--------------------------------------------------------------------------------------

// Set a specific stencil value in the rendered area - used to initialise the stencil area of the mirror
DepthStencilState SetStencilValue
{
	// Use depth buffer normally
	DepthFunc      = Less;
	DepthWriteMask = All;

	// Enable stencil buffer and replace all pixel stencil values with the reference value (value specified in the technique)
	StencilEnable        = True;
	FrontFaceStencilFunc = Always;  // Always...
	FrontFaceStencilPass = Replace; // ...replace the stencil values
	BackFaceStencilFunc  = Always;
	BackFaceStencilPass  = Replace; 
};


// Only render to the area with a given stencil value, used for rendering mirror content (ensures we don't rendered outside area of the mirror)
DepthStencilState AffectStencilArea
{
	// Use depth buffer normally
	DepthFunc      = Less;
	DepthWriteMask = All;

	// Only render those pixels whose stencil value is equal to the reference value (value specified in the technique)
	StencilEnable        = True;
	FrontFaceStencilFunc = Equal; // Only render on matching stencil
	FrontFaceStencilPass = Keep;  // But don't change the stencil values
	BackFaceStencilFunc  = Equal;
	BackFaceStencilPass  = Keep; 
};

// Only render to the area with a given stencil value but don't write to depth-buffer, used for rendering transparent polygons within the mirror (similar to state above)
DepthStencilState AffectStencilAreaDepthWritesOff
{
	// Test the depth buffer, but don't write anything new to it
	DepthFunc      = Less;
	DepthWriteMask = Zero; 

	// Only render those pixels whose stencil value is equal to the reference value (value specified in the technique)
	StencilEnable        = True;
	FrontFaceStencilFunc = Equal; // Only render on matching stencil
	FrontFaceStencilPass = Keep;  // But don't change the stencil values
	BackFaceStencilFunc  = Equal;
	BackFaceStencilPass  = Keep; 
};

// Only render to the area with a given stencil value and completely ignore the depth buffer - used to clear the mirror before rendering the scene inside it
DepthStencilState AffectStencilAreaIgnoreDepth
{
	// Disable depth buffer - we're going to fill the mirror/portal with distant z-values as we will want to render a new scene in there. So we must
	// ignore the z-values of the mirror surface - or the depth buffer would think the mirror polygon was obscuring our distant z-values
	DepthFunc = Always;

	// Only render those pixels whose stencil value is equal to the reference value (value specified in the technique)
	StencilEnable        = True;
	FrontFaceStencilFunc = Equal; // Only render on matching stencil
	FrontFaceStencilPass = Keep;  // But don't change the stencil values
	BackFaceStencilFunc  = Equal;
	BackFaceStencilPass  = Keep; 
};


//--------------------------------------------------------------------------------------
// One Colour
//--------------------------------------------------------------------------------------

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

VS_BASIC_OUTPUT BasicTransform( VS_BASIC_INPUT vIn )
{
	VS_BASIC_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	vOut.ClipDist = dot( worldPos, ClipPlane );
	
	return vOut;
}

float4 OneColour( VS_BASIC_OUTPUT vOut ) : SV_Target
{
	return float4( ModelColour, 1.0 ); // Set alpha channel to 1.0 (opaque)
}

technique10 tPlainColour
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, BasicTransform() ) );
        SetGeometryShader( NULL );                                   
        SetPixelShader( CompileShader( ps_4_0, OneColour() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


//--------------------------------------------------------------------------------------
// Unlit Texture
//--------------------------------------------------------------------------------------

struct VS_TEXTURE_INPUT
{
    float3 Pos    : POSITION;
	float2 UV     : TEXCOORD0;
};

struct VS_TEXTURE_OUTPUT
{
    float4 ProjPos	: SV_POSITION;
	float2 UV		: TEXCOORD0;
	float  ClipDist    : SV_ClipDistance;
};

VS_TEXTURE_OUTPUT TextureTransform( VS_TEXTURE_INPUT vIn )
{
	VS_TEXTURE_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	float4 worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );


	vOut.UV = vIn.UV;
	vOut.ClipDist = dot( worldPos, ClipPlane );

	return vOut;
}

//Textured pixel Shader
float4 BasicTexture( VS_TEXTURE_OUTPUT vOut ) : SV_Target
{
	return DiffuseMap.Sample(Trilinear, vOut.UV);
}


//Changes the colour of the texture over time
float4 ColourTexture( VS_TEXTURE_OUTPUT vOut ) : SV_Target
{
	return ( DiffuseMap.Sample(Trilinear, vOut.UV) * float4( ModelColour, 1.0) ) / 2;
}

technique10 tPlainTexture
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_4_0, TextureTransform() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, BasicTexture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

technique10 tColourChangeTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, TextureTransform() ) );
		SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, ColourTexture() ) );
		SetBlendState( AdditiveBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOff, 0 );
	}
}

//--------------------------------------------------------------------------------------
// Simply Lit Textures
//--------------------------------------------------------------------------------------

struct VS_LIT_TEXTURE_INPUT
{
    float3 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD0;
};

struct VS_LIT_TEXTURE_OUTPUT
{
	float4 ProjPos		: SV_POSITION;
	float4 worldPos		: POSITION;
	float4 worldNormal	: NORMAL;
	float2 UV			: TEXCOORD0;
	float  ClipDist    : SV_ClipDistance;
};

VS_LIT_TEXTURE_OUTPUT BasicTransformLitTex( VS_LIT_TEXTURE_INPUT vIn )
{
	VS_LIT_TEXTURE_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	vOut.worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( vOut.worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	// Now the normals
	float4 modelNormal = float4(vIn.Normal, 0.0f); //promote to 4 so we can multiply
	vOut.worldNormal = mul( modelNormal, WorldMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	vOut.ClipDist = dot( vOut.worldPos, ClipPlane );

	return vOut;
}

//------------------
// 2 Lights
//------------------

//Lit and Textured pixel Shader - 2 lights
float4 LitTexture( VS_LIT_TEXTURE_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 lightVector = normalize(lightPos[0].xyz - vOut.worldPos.xyz);
	float3 lightVector2 = normalize(lightPos[1].xyz - vOut.worldPos.xyz);

	float3 light1Light = lightColour[0].xyz * max( dot( worldNormal.xyz, lightVector), 0 );
	float3 light2Light = lightColour[1].xyz * max( dot( worldNormal.xyz, lightVector2), 0 );
	float3 diffuseLight = ambientColour + light1Light + light2Light;

	float3 halfway1 = normalize( lightVector + cameraVector );
	float3 halfway2 = normalize( lightVector2 + cameraVector );

	light1Light = lightColour[0].xyz * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
	light2Light = lightColour[1].xyz * pow( max( dot( worldNormal.xyz, halfway2), 0 ), 256);
	float3 specularLight = light1Light + light2Light;

	float4 texColour = DiffuseMap.Sample(Trilinear, vOut.UV);
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 tLitTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, LitTexture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

//------------------
// 4 Lights
//------------------

//Lit and Textured pixel Shader - all 4 lights
float4 Lit4Texture( VS_LIT_TEXTURE_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);

	float3 diffuseLight = ambientColour;
	float3 specularLight;

	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float3 lightVector = normalize(lightPos[i].xyz - vOut.worldPos.xyz);
		diffuseLight += lightColour[i].xyz * max( dot( worldNormal.xyz, lightVector), 0 );
		float3 halfway1 = normalize( lightVector + cameraVector );
		specularLight += lightColour[i].xyz * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
	}

	float4 texColour = DiffuseMap.Sample(Trilinear, vOut.UV);
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

//Lit and Textured pixel Shader - all 4 lights with attenuation
float4 Lit4AttenTexture( VS_LIT_TEXTURE_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 diffuseLight = ambientColour;
	float3 specularLight;

	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float3 lightVector = normalize(lightPos[i].xyz - vOut.worldPos.xyz);
		
		float3 lightTemp = lightColour[i].xyz * max( dot( worldNormal.xyz, lightVector.xyz), 0 );
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		diffuseLight += lightTemp;
		
		float3 halfway1 = normalize( lightVector + cameraVector );
		lightTemp = lightColour[i].xyz * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		specularLight += lightTemp;
	}

	float4 texColour = DiffuseMap.Sample(Trilinear, vOut.UV);
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 t4LitTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, Lit4Texture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

technique10 t4LitAttenTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, Lit4AttenTexture() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}

float4 LitGlassPS( VS_LIT_TEXTURE_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 diffuseLight = ambientColour;
	float3 specularLight;

	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float3 lightVector = normalize(lightPos[i].xyz - vOut.worldPos.xyz);
		
		float3 lightTemp = lightColour[i].xyz * max( dot( worldNormal.xyz, lightVector.xyz), 0 );
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		diffuseLight += lightTemp;
		
		float3 halfway1 = normalize( lightVector + cameraVector );
		lightTemp = lightColour[i].xyz * pow( max( dot( worldNormal.xyz, halfway1), 0 ), 256);
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		specularLight += lightTemp;
	}

	float4 texColour = DiffuseMap.Sample(Trilinear, vOut.UV);
	return (4 * texColour) * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 tLitGlass
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, LitGlassPS() ) );
		SetBlendState( MultiplicativeBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOff, 0 );
	}
}

//--------------------------------------------------------------------------------------
// Normal Mapping
//--------------------------------------------------------------------------------------

struct VS_NMAP_INPUT
{
    float3 Pos		: POSITION;
	float2 UV		: TEXCOORD0;
};

struct VS_NMAP_OUTPUT
{
	float4 worldPos		: POSITION;
	float4 ProjPos		: SV_POSITION;
	float2 UV			: TEXCOORD0;
	float  ClipDist    : SV_ClipDistance;
};


VS_NMAP_OUTPUT NormMapTransform( VS_NMAP_INPUT vIn )
{
	VS_NMAP_OUTPUT vOut;
	
	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
	vOut.worldPos = mul( modelPos, WorldMatrix );
	float4 viewPos  = mul( vOut.worldPos, ViewMatrix );
	vOut.ProjPos    = mul( viewPos,  ProjMatrix );
	
	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	vOut.ClipDist = dot( vOut.worldPos, ClipPlane );

	return vOut;
}

float4 NormMap ( VS_NMAP_OUTPUT vOut ) : SV_Target
{	
	float4 texColour = DiffuseMap.Sample( Trilinear, vOut.UV );
	float3 mapNormal = NormalMap.Sample( Bilinear, vOut.UV ).rgb;

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 diffuseLight = ambientColour;
	float3 specularLight;

	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float3 lightVector = normalize(lightPos[i].xyz - vOut.worldPos.xyz);
		
		float3 lightTemp = lightColour[i].xyz * max( dot(mapNormal, lightVector.xyz), 0 );
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		diffuseLight += lightTemp;
		
		float3 halfway1 = normalize( lightVector + cameraVector );
		lightTemp = lightColour[i].xyz * pow( max( dot( mapNormal, halfway1), 0 ), 256);
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		specularLight += lightTemp;
	}

	
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 tNormalMapping
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, NormMapTransform() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, NormMap() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone );
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


//--------------------------------------------------------------------------------------
// Specular Mapping
//--------------------------------------------------------------------------------------

float4 Lit4SpecMap( VS_LIT_TEXTURE_OUTPUT vOut ) : SV_Target
{
	float4 worldNormal = normalize( vOut.worldNormal );

	float4 texColour = DiffuseMap.Sample(Trilinear, vOut.UV);

	float specularPower = texColour.a;

	float3 cameraVector = normalize(cameraPos - vOut.worldPos.xyz);
	float3 diffuseLight = ambientColour;
	float3 specularLight;

	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float3 lightVector = normalize(lightPos[i].xyz - vOut.worldPos.xyz);
		
		float3 lightTemp = lightColour[i].xyz * max( dot( worldNormal.xyz, lightVector.xyz), 0 );
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		diffuseLight += lightTemp;
		
		float3 halfway1 = normalize( lightVector + cameraVector );
		lightTemp = lightColour[i].xyz * pow( max( dot( worldNormal.xyz, halfway1), 0 ), specularPower);
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		specularLight += lightTemp;
	}

	
	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 t4LitSpecularMap
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, BasicTransformLitTex() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, Lit4SpecMap() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullNone ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


//--------------------------------------------------------------------------------------
// Paralax Mapping
//--------------------------------------------------------------------------------------

struct VS_PMAP_INPUT
{
    float3 Pos     : POSITION;
    float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
	float2 UV      : TEXCOORD0;
};

struct VS_PMAP_OUTPUT
{
	float4 ProjPos      : SV_POSITION;
	float3 worldPos     : POSITION;
	float3 ModelNormal  : NORMAL;
	float3 ModelTangent : TANGENT;
	float2 UV           : TEXCOORD0;
	float  ClipDist    : SV_ClipDistance;
};


VS_PMAP_OUTPUT PMapTransform( VS_PMAP_INPUT vIn )
{
	VS_PMAP_OUTPUT vOut;

	float4 modelPos = float4(vIn.Pos, 1.0f); 
	float4 worldPos = mul( modelPos, WorldMatrix );
	vOut.worldPos = worldPos.xyz;

	float4 viewPos = mul( worldPos, ViewMatrix );
	vOut.ProjPos   = mul( viewPos,  ProjMatrix );

	vOut.ModelNormal  = vIn.Normal;
	vOut.ModelTangent = vIn.Tangent;

	vOut.UV = vIn.UV;

	vOut.ClipDist = dot( vOut.worldPos, ClipPlane );

	return vOut;
}

float4 ParallaxMap( VS_PMAP_OUTPUT vOut ) : SV_Target
{

	// Renormalise pixel normal/tangent that were *interpolated* from the vertex normals/tangents (and may have been scaled too)
	float3 modelNormal = normalize( vOut.ModelNormal );
	float3 modelTangent = normalize( vOut.ModelTangent );

	// Calculate bi-tangent and return to model space.
	float3 modelBiTangent = cross( modelNormal, modelTangent );
	float3x3 invTangentMatrix = float3x3(modelTangent, modelBiTangent, modelNormal);

	float3 cameraVector = normalize( cameraPos.xyz - vOut.worldPos.xyz);
	//reverse world matrix transform to go back to model space
	float3x3 invWorldMatrix = transpose( WorldMatrix );
	float3 cameraModelDir = normalize( mul( cameraVector, invWorldMatrix ) ); 

	float3x3 tangentMatrix = transpose( invTangentMatrix ); 
	float2 textureOffsetDir = mul( cameraModelDir, tangentMatrix );
	
	//retrieve the depth from the normal map - multiply by constant for parallax depth
	float texDepth = 0.05f * (NormalMap.Sample( Trilinear, vOut.UV ).a - 0.5f);
	
	// Use the depth of the texture to offset the given texture coordinate - this corrected texture coordinate will be used from here on
	float2 offsetTexCoord = vOut.UV + ( texDepth * textureOffsetDir );
	float3 mapNormal = 2.0f * NormalMap.Sample( Trilinear, offsetTexCoord ).rgb - 1.0f; 
	float4 texColour = DiffuseMap.Sample( Trilinear, offsetTexCoord );

	float specularPower = texColour.a;

	float3 worldNormal = normalize( mul( float4 (mul( mapNormal, invTangentMatrix ), 0.0f), WorldMatrix ).xyz );


	float3 diffuseLight = ambientColour;
	float3 specularLight;
	
	for(uint i = 0; i < MAX_LIGHTS; i++)
	{
		float3 lightVector = normalize(lightPos[i].xyz - vOut.worldPos.xyz);
		
		float3 lightTemp = lightColour[i].xyz * max( dot( worldNormal, lightVector.xyz), 0 );
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		diffuseLight += lightTemp;
		
		float3 halfway1 = normalize( lightVector + cameraVector.xyz );
		lightTemp = lightColour[i].xyz * pow( max( dot( worldNormal, halfway1), 0 ), specularPower);
		lightTemp = lightTemp * ( lightBright[i].x / length( lightPos[i].xyz - vOut.worldPos.xyz ) );
		specularLight += lightTemp;
	}

	return texColour * ( float4(diffuseLight, 1.0) + float4(specularLight, 1.0) );
}

technique10 tParallaxMap
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, PMapTransform() ) );
        SetGeometryShader( NULL );  
		SetPixelShader( CompileShader( ps_4_0, ParallaxMap() ) );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( CullBack ); 
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}