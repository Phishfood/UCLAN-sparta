//--------------------------------------------------------------------------------------
// File: Tessellation.fx
//
// DirectX 11 Tessellation
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

// The matrices (4x4 matrix of floats) for transforming from 3D model to 2D projection (used in vertex shader)
float4x4 WorldMatrix;
float4x4 ViewMatrix;
float4x4 ProjMatrix;
float4x4 ViewProjMatrix;
float4x4 InvViewMatrix;

// Viewport Dimensions
float ViewportWidth;
float ViewportHeight;

// Information used for lighting (in the vertex or pixel shader)
float3 Light1Pos;
float3 Light2Pos;
float3 Light1Colour;
float3 Light2Colour;
float3 AmbientColour;
float  SpecularPower;
float3 CameraPos;

// Variable used to tint each light model to show the colour that it emits
float3 TintColour;

//**** Displacement mapping settings
float DisplacementScale; // Depth of displacement mapping - displacement in world units for the maximum height in the height map
float MinTessDistance;   // Distance at which maximum tessellation will be used (to be used in hull shader patch constant function HS_FixedConstants)
float MaxTessDistance;   // --"-- minimum
float MinTessellation;   // Minimum tessellation factor, i.e. maximum detail level (to be used in hull shader patch constant function HS_FixedConstants)
float MaxTessellation;   // --"-- maximum


// Textures
Texture2D DiffuseMap;       // Diffuse texture map (with optional specular map in alpha)
Texture2D NormalHeightMap;  // Normal & height map used for displacement mapping - exactly the same map as used for parallax mapping

// Samplers to use with the above textures
SamplerState TrilinearWrap
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
SamplerState PointClamp
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};


//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

// This structure describes generic vertex data to be sent into the vertex shader
struct VS_INPUT
{
	float3 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 UV     : TEXCOORD0;
};

// The input required for the per-pixel lighting pixel shader, containing a 2D position, lighting colours and texture coordinates
struct PS_LIGHTING_INPUT
{
	float4 ProjPos     : SV_Position;
	float3 WorldPos    : POSITION;
	float3 WorldNormal : NORMAL;
	float2 UV          : TEXCOORD0;
};

// More basic techniques don't deal with lighting
struct PS_BASIC_INPUT
{
	float4 ProjPos : SV_Position;
	float2 UV      : TEXCOORD0;
};


//**************************************************************************************
// Tessellation - Inputs and Outputs
//**************************************************************************************

// Vertex input with tangents
struct VS_TANGENT_INPUT
{
	float3 Pos     : POSITION;
	float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
	float2 UV      : TEXCOORD0;
};

struct HS_INPUT
{
	float3 WorldPos     : POSITION;
	float3 WorldNormal  : NORMAL;
	float3 WorldTangent : TANGENT;
	float2 UV           : TEXCOORD0;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float3 WorldPos     : POSITION;
	float3 WorldNormal  : NORMAL;
	float3 WorldTangent : TANGENT;
	float2 UV           : TEXCOORD0;
	float  OppEdgeTess : TEXCOORD1;
};

struct HS_PATCH_CONSTANT_OUTPUT
{
	float    EdgeTess[3] : SV_TessFactor;
	float    InsideTess : SV_InsideTessFactor;
};

struct DS_OUTPUT
{
	float4 ProjPos      : SV_Position;
	float3 WorldPos     : POSITION;
	float3 WorldNormal  : NORMAL;
	float3 WorldTangent : TANGENT;
	float2 UV           : TEXCOORD0;
};


//**************************************************************************************
// Tessellation - Vertex Shader
//**************************************************************************************

// Vertex shader for tessellation is just the same as the usual vertex shader except it only transforms the model geometry into world space,
// hull shader works in world space, and the domain shader handles the transformation into screen coordinates (with the view and projection matrix)
HS_INPUT VS_TriTessellation( VS_TANGENT_INPUT vIn )
{
	HS_INPUT vOut;

	// Transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f);
		vOut.WorldPos = mul( modelPos, WorldMatrix ).xyz;

	// Transform the vertex normal and tangent from model space into world space
	float4 modelNormal = float4(vIn.Normal, 0.0f);
		float4 modelTangent = float4(vIn.Tangent, 0.0f);
		vOut.WorldNormal = mul( modelNormal, WorldMatrix ).xyz;
	vOut.WorldTangent = mul( modelTangent, WorldMatrix ).xyz;

	// Pass texture coordinates (UVs) on unchanged
	vOut.UV = vIn.UV;

	return vOut;
}


//**************************************************************************************
// Tessellation - Hull Shader (and patch constant function)
//**************************************************************************************

// The Hull Shader reads in a set of "patches", surfaces defined by "control points". The hull shader is called once for each control point, and its job is to
// do any kind of manipulation or calculation of the control points required *prior* to the tessellation
// A patch is some kind of surface, which starts as a line, triangle or quad, but which will be tessellated and curved/bent into any shape we wish based on the
// control points. Exactly how the control points define the shape of the patch is up to the programmer. For example the control points might be used to take a
// quad and manipulate it as a Bezier spline. However for the case of displacement mapping we won't use splines, the control points are just the vertices of each
// triangle, and we're not going to manipulate them prior to tessellation, so there is little for this hull shader to do yet. However, we need to define a hull
// shader in any case - the most important part being the sequence of attributes that are written before the shader itself:
//
[domain( "tri" )]                        // The input before the tessellation is triangles
[partitioning( "fractional_odd" )]       // Tessellation style to use - can use "fractional_even", "fractional_odd", "integer" or "pow2"
[outputtopology( "triangle_cw" )]        // We want the tessellated output to be triangles too
[outputcontrolpoints( 3 )]               // Not using splines or similar (see lecture) in this case the input and output "control points" are just the vertices of each triangle
[patchconstantfunc( "HS_DistanceTess" )] // Name of the "patch constant" function used to select the tessellation factors etc. (see the function after this one)
[maxtessfactor( 50.0 )]                  // Maximum tessellation factor that the constant function (named on the line above) will select
HS_CONTROL_POINT_OUTPUT HS_TriTessellation
(
InputPatch<HS_INPUT, 3> inputPatch,    // The input patch is an array of control points (up to 32 per patch), here simply the 3 vertices of each triangle
uint pointID : SV_OutputControlPointID // This function is called once for each control point, this integer counts from 0 to indicate which point we are on (0, 1, 2 for a triangle)
)
{
	HS_CONTROL_POINT_OUTPUT hOut;

	// Just copy current control point to the output. The Domain Shader will do the displacement mapping after receiving the output of the tessellation stage
	hOut.WorldPos = inputPatch[pointID].WorldPos;
	hOut.WorldNormal = inputPatch[pointID].WorldNormal;
	hOut.WorldTangent = inputPatch[pointID].WorldTangent;
	hOut.UV = inputPatch[pointID].UV;

	// Calculate the tessellation factor for the opposite edge from this point, result will be used in the patch constant function below
	// This is the solution to watertight seams when varying tessellation by distance: for each edge, we find the furthest of the two points to
	// the camera and set the tessellation factor based on that distance. Of course, the patch on either side of the edge will agree on where the
	// two points in the edge are and which is furthest from the camera, so both patches will choose the same edge tessellation ==> watertight seams
	// Technical point: this code would seem better in the constant function below where the tessellation factors are actually output. However, it
	// doesn't seem to work correctly there - maybe there's some vertex reordering (or I just got it wrong!). In any case, this is the location 
	// suggested by NVidia and Epic, although there is no documentation to back it up... This kind of guesswork is common with newer features...
	const float distanceRange = MaxTessDistance - MinTessDistance;
	const uint oppID1 = pointID < 2 ? pointID + 1 : 0; // Find the two points making up the edge opposite to this point in the triangle
	const uint oppID2 = oppID1  < 2 ? oppID1 + 1 : 0; // --"--
	float maxDist = max( distance( CameraPos, inputPatch[oppID1].WorldPos ), distance( CameraPos, inputPatch[oppID2].WorldPos ) ); // Maximum distance of the two points
	hOut.OppEdgeTess = lerp( MaxTessellation, MinTessellation, sqrt( saturate( (maxDist - MinTessDistance) / distanceRange ) ) );      // Tessellation factor in range required

	return hOut;
}

// The "patcjconstant function" used for the hull shader above. All hull shaders require such a function. It is called once for each patch (rather than once
// for each control point) and sets up any values that are constant for the entire patch. In this example it is called once for each triangle in our mesh.
// At a minimum the function must specify the "Tessellation factors": these determine how finely the patches (triangles) will be tessellated
// For a triangle patch, we specify one tessellation factor for each edge (how many times the edges will be split), and one factor for the inside of the
// triangle (how many extra triangles will be generated in the interior of the triangle)
// Using the same tessellation factor for everything is simplest and will create a nice regular tessellation. However, if you are using any kind of varying
// tessellation (e.g. tessellation that decreases with distance), then the edges between triangles of different tessellation will need to have their factors
// matched or you will see cracks in the geometry
HS_PATCH_CONSTANT_OUTPUT HS_DistanceTess
(
// This function can read the input patches and/or the updated patches that wer output from the hull shader above. Here we will read the output of the
// hull shader as it will help with calculating distance-based tessellation factors in one of the later lab exercises
OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> outputPatch
)
{
	HS_PATCH_CONSTANT_OUTPUT hOut;

	// Starting with fixed tessellation factors for simplicity - all triangles will be tessellated by the same amount
	// Tech detail: The edge referred to is the one opposite to the array index, so EdgeTess[0] refers to the edge from point 1 to point 2
	hOut.EdgeTess[0] = outputPatch[0].OppEdgeTess; // Tessellation for triangle edge
	hOut.EdgeTess[1] = outputPatch[1].OppEdgeTess;
	hOut.EdgeTess[2] = outputPatch[2].OppEdgeTess;
	hOut.InsideTess = min( min( hOut.EdgeTess[0], hOut.EdgeTess[1] ), hOut.EdgeTess[2] ); // Tesselation for triangle interior

	return hOut;
}



//**************************************************************************************
// Tessellation - Domain Shader
//**************************************************************************************

// The domain shader gets the output points from the hull shader, which in our case is just the original geometry triangles in world space, and it
// also gets a tessellated patch in a generic space with 0->1 coordinates from the tessellator stage. It combines these together to create a
// tessellated patch in world space, which it then finally transforms to 2D projection space for rendering by the pixel shader.
//
// When working with quad patches the generic tessellation arrives in a 0->1 unit square, a bit like texture coordinates. However, when working with
// triangle patches the generic tessellation arrives as three Barycentric coordinates. Barycentric coordinates are a good way to specify a position
// within an arbitrary triangle. If the triangle points are P0,P1 and P2, then the Barycentric coordinates a0,a1,a2 define the point:
//  P = a0 * P0 + a1 * P1 + a2 * P2
// In other words add together the points (or normals or whatever) with the Barycentric coordinates as weightings. The one rule for Barycentric
// coordinates is that a0 + a1 + a2 will always be 1. With this rule you can guarantee the point represented is within the plane of the triangle.
// Effectively Barycentric coordinates define a coordinate system with one triangle point as the origin and the two radiating edges as axes.
// Barycentric coordinates occasionally crop up in graphics (or physics) when defining points in triangles, although new, they are not at all complex
// to use. See the Van Verth maths text for further information
[domain( "tri" )]
DS_OUTPUT DS_DisplacementMapping
(
HS_PATCH_CONSTANT_OUTPUT input,
float3 BarycentricCoordinates : SV_DomainLocation,
const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> TrianglePatch
)
{
	DS_OUTPUT dOut;

	// Interpolate world space position with barycentric coordinates
	dOut.WorldPos = BarycentricCoordinates.x * TrianglePatch[0].WorldPos +
		BarycentricCoordinates.y * TrianglePatch[1].WorldPos +
		BarycentricCoordinates.z * TrianglePatch[2].WorldPos;

	// Interpolate world space normal & tangent with barycentric coordinates and renormalize them
	dOut.WorldNormal = BarycentricCoordinates.x * TrianglePatch[0].WorldNormal +
		BarycentricCoordinates.y * TrianglePatch[1].WorldNormal +
		BarycentricCoordinates.z * TrianglePatch[2].WorldNormal;
	dOut.WorldNormal = normalize( dOut.WorldNormal );

	dOut.WorldTangent = BarycentricCoordinates.x * TrianglePatch[0].WorldTangent +
		BarycentricCoordinates.y * TrianglePatch[1].WorldTangent +
		BarycentricCoordinates.z * TrianglePatch[2].WorldTangent;
	dOut.WorldTangent = normalize( dOut.WorldTangent );

	// Interpolate texture coordinates with barycentric coordinates
	dOut.UV = BarycentricCoordinates.x * TrianglePatch[0].UV +
		BarycentricCoordinates.y * TrianglePatch[1].UV +
		BarycentricCoordinates.z * TrianglePatch[2].UV;

	// Sample normal / height map. Domain shaders cannot automatically use mip-maps (shaders don't "know" about distance until the pixel
	// shader stage), so we have to use the "SampleLevel" statement (instead of "Sample") and explicitly say which mip-map to use.
	// Just choosing the highest detail mip-map here (the final 0), but you can calculate a value (from 0 to number mip-maps) based on
	// distance, which will improve performance. See the DirectX detail tessellation sample for an example of setting an explicit mip-map
	float4 NormalHeight = NormalHeightMap.SampleLevel( TrilinearWrap, dOut.UV, 0 );

		// Displace the output vertex world position along world normal by height from the height map. Also multiply the displacement by DisplacementScale to give us
		// control over the overall depth of the effect. Note: the NormalHeight map stores the normal in xyz and the height in w
		dOut.WorldPos += dOut.WorldNormal * NormalHeight.w * DisplacementScale;

	// Transform world position with view-projection matrix
	dOut.ProjPos = mul( float4(dOut.WorldPos, 1.0f), ViewProjMatrix );

	return dOut;
}



//**************************************************************************************
// Tessellation - Pixel Shader
//**************************************************************************************

// The pixel shader for displacement mapping is essentially the same as normal mapping. We have created extra tessellated geometry and displaced the positions
// to create a real bumpy surface. We used a height map to get the geometry displacements, so just use an associated normal map to get the normals.
float4 PS_NormalMapping( DS_OUTPUT pIn ) : SV_Target
{
	// Normalize interpolated normals
	float3 worldNormal = normalize( pIn.WorldNormal );
	float3 worldTangent = normalize( pIn.WorldTangent );

	// Calculate binormal and form a matrix for tangent space (can store the binormal in the source mesh data to avoid this calculation if we wish)
	float3 worldBinormal = cross( worldNormal, worldTangent );
	float3x3 invTangentMatrix = float3x3(worldTangent, worldBinormal, worldNormal);

	// Get the texture normal from the normal map, convert from rgb to xyz
	float3 textureNormal = 2.0f * NormalHeightMap.Sample( TrilinearWrap, pIn.UV ).xyz - 1.0f; // Scale from 0->1 to -1->1

	// Convert texture normal into model space using the inverse tangent matrix, and then convert into world space using the world
	// matrix. Normalise, because of the effects of texture filtering and in case the world matrix contains scaling
	worldNormal = normalize( mul( textureNormal, invTangentMatrix ) );


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 CameraDir = normalize( CameraPos - pIn.WorldPos.xyz ); // Position of camera - position of current vertex (or pixel) (in world space)

		//// LIGHT 1
		float3 Light1Dir = normalize( Light1Pos - pIn.WorldPos.xyz );   // Direction for each light is different
		float3 Light1Dist = length( Light1Pos - pIn.WorldPos.xyz );
		float3 DiffuseLight1 = Light1Colour * max( dot( worldNormal.xyz, Light1Dir ), 0 ) / Light1Dist;
		float3 halfway = normalize( Light1Dir + CameraDir );
		float3 SpecularLight1 = DiffuseLight1 * pow( max( dot( worldNormal.xyz, halfway ), 0 ), SpecularPower );

		//// LIGHT 2
		float3 Light2Dir = normalize( Light2Pos - pIn.WorldPos.xyz );
		float3 Light2Dist = length( Light2Pos - pIn.WorldPos.xyz );
		float3 DiffuseLight2 = Light2Colour * max( dot( worldNormal.xyz, Light2Dir ), 0 ) / Light2Dist;
		halfway = normalize( Light2Dir + CameraDir );
	float3 SpecularLight2 = DiffuseLight2 * pow( max( dot( worldNormal.xyz, halfway ), 0 ), SpecularPower );

		// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
		float3 DiffuseLight = AmbientColour + DiffuseLight1 + DiffuseLight2;
		float3 SpecularLight = SpecularLight1 + SpecularLight2;


		////////////////////
		// Sample texture

		// Extract diffuse material colour for this pixel from a texture
		float4 DiffuseMaterial = DiffuseMap.Sample( TrilinearWrap, pIn.UV );

		// Assume specular material colour is white (i.e. highlights are a full, untinted reflection of light)
		float3 SpecularMaterial = DiffuseMaterial.a;


		////////////////////
		// Combine colours 

		// Combine maps and lighting for final pixel colour
		float4 combinedColour;
	combinedColour.rgb = DiffuseMaterial.rgb * DiffuseLight + SpecularMaterial * SpecularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}



//--------------------------------------------------------------------------------------
// Other Vertex Shaders
//--------------------------------------------------------------------------------------

// This vertex shader passes on the vertex position and normal to the pixel shader for per-pixel lighting
PS_LIGHTING_INPUT VS_PixelLitTex( VS_INPUT vIn )
{
	PS_LIGHTING_INPUT vOut;

	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
		float4 worldPos = mul( modelPos, WorldMatrix );
		vOut.WorldPos = worldPos.xyz;

	// Use camera matrices to further transform the vertex from world space into view space (camera's point of view) and finally into 2D "projection" space for rendering
	float4 viewPos = mul( worldPos, ViewMatrix );
		vOut.ProjPos = mul( viewPos, ProjMatrix );

	// Transform the vertex normal from model space into world space (almost same as first lines of code above)
	float4 modelNormal = float4(vIn.Normal, 0.0f); // Set 4th element to 0.0 this time as normals are vectors
		vOut.WorldNormal = mul( modelNormal, WorldMatrix ).xyz;

	// Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
	vOut.UV = vIn.UV;

	return vOut;
}


// Basic vertex shader to transform 3D model vertices to 2D and pass UVs to the pixel shader
PS_BASIC_INPUT VS_BasicTransformTex( VS_INPUT vIn )
{
	PS_BASIC_INPUT vOut;

	// Use world matrix passed from C++ to transform the input model vertex position into world space
	float4 modelPos = float4(vIn.Pos, 1.0f); // Promote to 1x4 so we can multiply by 4x4 matrix, put 1.0 in 4th element for a point (0.0 for a vector)
		float4 worldPos = mul( modelPos, WorldMatrix );
		float4 viewPos = mul( worldPos, ViewMatrix );
		vOut.ProjPos = mul( viewPos, ProjMatrix );

	// Pass texture coordinates (UVs) on to the pixel shader
	vOut.UV = vIn.UV;

	return vOut;
}



//--------------------------------------------------------------------------------------
// Other Pixel Shaders
//--------------------------------------------------------------------------------------

// Pixel shader that calculates per-pixel lighting and combines with diffuse and specular map
//
float4 PS_PixelLitDiffuseMap( PS_LIGHTING_INPUT pIn ) : SV_Target  // The ": SV_Target" bit just indicates that the returned float4 colour goes to the render target (i.e. it's a colour to render)
{
	// Can't guarantee the normals are length 1 now (because the world matrix may contain scaling), so renormalise
	// If lighting in the pixel shader, this is also because the interpolation from vertex shader to pixel shader will also rescale normals
	float3 worldNormal = normalize( pIn.WorldNormal );


	///////////////////////
	// Calculate lighting

	// Calculate direction of camera
	float3 CameraDir = normalize( CameraPos - pIn.WorldPos.xyz ); // Position of camera - position of current vertex (or pixel) (in world space)

	//// LIGHT 1
	float3 Light1Dir = normalize( Light1Pos - pIn.WorldPos.xyz );   // Direction for each light is different
	float3 Light1Dist = length( Light1Pos - pIn.WorldPos.xyz );
	float3 DiffuseLight1 = Light1Colour * max( dot( worldNormal.xyz, Light1Dir ), 0 ) / Light1Dist;
	float3 halfway = normalize( Light1Dir + CameraDir );
	float3 SpecularLight1 = DiffuseLight1 * pow( max( dot( worldNormal.xyz, halfway ), 0 ), SpecularPower );

	//// LIGHT 2
	float3 Light2Dir = normalize( Light2Pos - pIn.WorldPos.xyz );
	float3 Light2Dist = length( Light2Pos - pIn.WorldPos.xyz );
	float3 DiffuseLight2 = Light2Colour * max( dot( worldNormal.xyz, Light2Dir ), 0 ) / Light2Dist;
	halfway = normalize( Light2Dir + CameraDir );
	float3 SpecularLight2 = DiffuseLight2 * pow( max( dot( worldNormal.xyz, halfway ), 0 ), SpecularPower );

		// Sum the effect of the two lights - add the ambient at this stage rather than for each light (or we will get twice the ambient level)
		float3 DiffuseLight = AmbientColour + DiffuseLight1 + DiffuseLight2;
		float3 SpecularLight = SpecularLight1 + SpecularLight2;


		////////////////////
		// Sample texture

		// Extract diffuse material colour for this pixel from a texture
		float4 DiffuseMaterial = DiffuseMap.Sample( TrilinearWrap, pIn.UV );

		// Assume specular material colour is white (i.e. highlights are a full, untinted reflection of light)
		float3 SpecularMaterial = DiffuseMaterial.a;


		////////////////////
		// Combine colours 

		// Combine maps and lighting for final pixel colour
		float4 combinedColour;
	combinedColour.rgb = DiffuseMaterial.rgb * DiffuseLight + SpecularMaterial * SpecularLight;
	combinedColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

	return combinedColour;
}


// A pixel shader that just tints a (diffuse) texture with a fixed colour
//
float4 PS_TintDiffuseMap( PS_BASIC_INPUT pIn ) : SV_Target
{
	// Extract diffuse material colour for this pixel from a texture
	float4 diffuseMapColour = DiffuseMap.Sample( TrilinearWrap, pIn.UV );

	// Tint by global colour (set from C++)
	diffuseMapColour.rgb *= TintColour / 10;

	return diffuseMapColour;
}



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

// Techniques are used to render models in our scene. They select a combination of vertex, geometry and pixel shader from those provided above. Can also set states.

// Displacement mapping using tessellation
technique11 DisplacementMapping
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS_TriTessellation()) );
		SetHullShader( CompileShader(hs_5_0, HS_TriTessellation()) );
		SetDomainShader( CompileShader(ds_5_0, DS_DisplacementMapping()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS_NormalMapping()) );

		// Switch off blending states
		SetBlendState( NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetRasterizerState( CullNone );
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


// Displacement mapping using tessellation
technique11 DisplacementMappingWireframe
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS_TriTessellation()) );
		SetHullShader( CompileShader(hs_5_0, HS_TriTessellation()) );
		SetDomainShader( CompileShader(ds_5_0, DS_DisplacementMapping()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS_NormalMapping()) );

		// Switch off blending states
		SetBlendState( NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetRasterizerState( Wireframe );
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


// Per-pixel lighting with diffuse map
technique11 PixelLitTex
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS_PixelLitTex()) );
		SetHullShader( NULL );
		SetDomainShader( NULL );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS_PixelLitDiffuseMap()) );

		// Switch off blending states
		SetBlendState( NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetRasterizerState( CullBack );
		SetDepthStencilState( DepthWritesOn, 0 );
	}
}


// Additive blended texture. No lighting, but uses a global colour tint. Used for light models
technique11 AdditiveTexTint
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS_BasicTransformTex()) );
		SetHullShader( NULL );
		SetDomainShader( NULL );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS_TintDiffuseMap()) );

		SetBlendState( AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF );
		SetRasterizerState( CullNone );
		SetDepthStencilState( DepthWritesOff, 0 );
	}
}
