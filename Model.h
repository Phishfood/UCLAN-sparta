#pragma once
//--------------------------------------------------------------------------------------
//	Model.h
//
//	The model class collects together a model's geometry (vertex and index data) and
//	also manages it's positioning with a world
//--------------------------------------------------------------------------------------

#ifndef MODEL_H_INCLUDED // Header guard - prevents file being included more than once (would cause errors)
#define MODEL_H_INCLUDED

#include <string>
#include <vector>
#include <algorithm>
#include <vector>
#include <fstream>
#include <istream>
#include <sstream>
using namespace std;

#include "Input.h"

#include "Defines.h"

struct SurfaceMaterial
{
	std::wstring matName;
	DirectX::XMFLOAT4 difColor;
	int texArrayIndex;
	bool hasTexture;
	bool transparent;
};


class CModel
{
/////////////////////////////
// Private member variables
private:
	static ID3D11Device* mpd3dDev;
	static ID3D11DeviceContext* mp_d3dDeviceContext;
	//-----------------
	// Postioning

	float MoveSpeed;
	float RotSpeed;


	// Positions, rotations and scaling for the model
	DirectX::XMFLOAT3   m_Position;
	DirectX::XMFLOAT3  m_Rotation;
	DirectX::XMFLOAT3   m_Scale;

	// World matrix for the model - built from the above
	DirectX::XMMATRIX m_WorldMatrix;

	
	//-----------------
	// Geometry data

	// Does this model have any geometry to render
	bool                     m_HasGeometry;

	// Vertex data for the model stored in a vertex buffer and the number of the vertices in the buffer
	ID3D11Buffer*            m_VertexBuffer;
	unsigned int             m_NumVertices;

	// Description of the elements in a single vertex (position, normal, UVs etc.)
	static const int         MAX_VERTEX_ELTS = 64;
	D3D11_INPUT_ELEMENT_DESC m_VertexElts[MAX_VERTEX_ELTS];
	ID3D11InputLayout*       m_VertexLayout; // Layout of a vertex (derived from above)
	unsigned int             m_VertexSize;   // Size of vertex calculated from contained elements

	// Index data for the model stored in a index buffer and the number of indices in the buffer
	ID3D11Buffer*            m_IndexBuffer;
	unsigned int             m_NumIndices;

	D3DX11_TECHNIQUE_DESC  m_techDesc;
	ID3DX11EffectTechnique* m_technique;

	uint32_t mi_meshSubsets;
	std::vector<uint32_t> mv_meshSubsetIndexStart;
	std::vector<uint32_t> mv_meshSubsetTexture;

	std::vector<SurfaceMaterial> mv_material;

/////////////////////////////
// Private functions

	bool LoadX(const string& fileName, ID3DX11EffectTechnique* shaderCode, bool tangents = false );
	bool LoadOBJ(const string& fileName, ID3DX11EffectTechnique* shaderCode, bool tangents = false );

/////////////////////////////
// Public member functions
public:
	
	static void SetDevice(ID3D11Device* newDevice) { mpd3dDev = newDevice; }
	static void SetDeviceContext(ID3D11DeviceContext* newDevCont) { mp_d3dDeviceContext = newDevCont; }

	///////////////////////////////
	// Constructors / Destructors

	// Constructor - initialise all settings, sensible defaults provided for everything.
	CModel( DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0,0,0), float scale = 1.0f );

	// Destructor
	~CModel();

	// Release resources used by model
	void ReleaseResources();


	/////////////////////////////
	// Data access

	// Getters
	DirectX::XMFLOAT3 GetPosition()
	{
		return m_Position;
	}
	DirectX::XMFLOAT3 GetRotation()
	{
		return m_Rotation;
	}
	DirectX::XMFLOAT3 GetScale()
	{
		return m_Scale;
	}

	DirectX::XMMATRIX GetWorldMatrix()
	{
		return m_WorldMatrix;
	}


	// Setters
	void SetPosition( DirectX::XMFLOAT3 position )
	{
		m_Position = position;
	}
	void SetRotation( DirectX::XMFLOAT3 rotation )
	{
		m_Rotation = rotation;
	}
	void SetScale( DirectX::XMFLOAT3 scale ) // Overloaded setter, two versions: this one sets x,y,z scale separately, the next sets all to the same value
	{
		m_Scale = scale;
	}
	void SetScale( float scale )
	{
		m_Scale = DirectX::XMFLOAT3( scale, scale, scale );
	}


	/////////////////////////////
	// Model Loading

	// Load the model geometry from a file. This function only reads the geometry using the first material in the file, so multi-material
	// models will load but will have parts missing. May optionally request for tangents to be created for the model (for normal or parallax mapping)
	// We need to pass an example technique that the model will use to help DirectX understand how to connect this data with the vertex shaders
	// Returns true if the load was successful
	bool Load( const string& fileName, ID3DX11EffectTechnique* shaderCode, bool tangents = false );


	/////////////////////////////
	// Model Usage

	// Update the world matrix of the model from its position, rotation and scaling
	void UpdateMatrix();
	
	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	void Control( float frameTime, EKeyCode turnUp, EKeyCode turnDown, EKeyCode turnLeft, EKeyCode turnRight,  
				  EKeyCode turnCW, EKeyCode turnCCW, EKeyCode moveForward, EKeyCode moveBackward );

	// Render the model with the given technique. Assumes any shader variables for the technique have already been set up (e.g. matrices and textures)
	void Render( ID3DX11EffectTechnique* technique );

	void SendGeometry();
	void SetTechnique( ID3DX11EffectTechnique* technique );
	void QuickRender();
};


#endif // End of header guard - see top of file
