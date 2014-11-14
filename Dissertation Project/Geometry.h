#pragma once

#include <vector>
#include <string>

#include "Defines.h"
#include "Input.h"

#include "CImportXFile.h"


class CGeometry
{
public:
private:
	char sourceFile[100];

	static ID3D10Device* mpd3dDev;

	float MoveSpeed;
	float RotSpeed;

	// Positions, rotations and scaling for the model
	D3DXVECTOR3   m_Position;
	D3DXVECTOR3   m_Rotation;
	D3DXVECTOR3   m_Scale;

	// World matrix for the model - built from the above
	D3DXMATRIXA16 m_WorldMatrix;

	
	//-----------------
	// Geometry data

	// Does this model have any geometry to render
	bool                     m_HasGeometry;

	// Vertex data for the model stored in a vertex buffer and the number of the vertices in the buffer
	ID3D10Buffer*            m_VertexBuffer;
	UINT32			         m_NumVertices;
	
	// Description of the elements in a single vertex (position, normal, UVs etc.)
	static const UINT32      MAX_VERTEX_ELTS = 64;
	D3D10_INPUT_ELEMENT_DESC m_VertexElts[MAX_VERTEX_ELTS];
	ID3D10InputLayout*       m_VertexLayout; // Layout of a vertex (derived from above)
	UINT32                   m_VertexSize;   // Size of vertex calculated from contained elements

	// Index data for the model stored in a index buffer and the number of indices in the buffer
	ID3D10Buffer*            m_IndexBuffer;
	UINT32                   m_NumIndices;




public:
	CGeometry(void);
	CGeometry( D3DXVECTOR3 position, D3DXVECTOR3 rotation, float scale );
	~CGeometry(void);

	bool CompareSource(const char input[100]);
	inline bool CGeometry::Load( const char fileName[], const int length, ID3D10EffectTechnique* myTechnique, bool tangents = false );
	bool CGeometry::Load( const string& fileName, ID3D10EffectTechnique* exampleTechnique, bool tangents = false );


	static void SetDevice(ID3D10Device* newDevice) { mpd3dDev = newDevice; }

	// Release resources used by model
	void ReleaseResources();


	/////////////////////////////
	// Data access

	// Getters
	D3DXVECTOR3 GetPosition()
	{
		return m_Position;
	}
	D3DXVECTOR3 GetRotation()
	{
		return m_Rotation;
	}
	D3DXVECTOR3 GetScale()
	{
		return m_Scale;
	}

	D3DXMATRIXA16 GetWorldMatrix()
	{
		return m_WorldMatrix;
	}


	// Setters
	void SetPosition( D3DXVECTOR3 position )
	{
		m_Position = position;
	}
	void SetRotation( D3DXVECTOR3 rotation )
	{
		m_Rotation = rotation;
	}
	void SetScale( D3DXVECTOR3 scale ) // Overloaded setter, two versions: this one sets x,y,z scale separately, the next sets all to the same value
	{
		m_Scale = scale;
	}
	void SetScale( float scale )
	{
		m_Scale = D3DXVECTOR3( scale, scale, scale );
	}


	/////////////////////////////
	// Model Usage

	// Update the world matrix of the model from its position, rotation and scaling
	void UpdateMatrix();
	
	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	void Control( float frameTime, EKeyCode turnUp, EKeyCode turnDown, EKeyCode turnLeft, EKeyCode turnRight,  
				  EKeyCode turnCW, EKeyCode turnCCW, EKeyCode moveForward, EKeyCode moveBackward );

	// Render the model with the given technique. Assumes any shader variables for the technique have already been set up (e.g. matrices and textures)
	void Render( ID3D10EffectTechnique* technique );

};

