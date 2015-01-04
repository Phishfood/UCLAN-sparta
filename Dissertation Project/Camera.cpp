//--------------------------------------------------------------------------------------
//	Camera.cpp
//
//	The camera class encapsulates the camera's view and projection matrix
//--------------------------------------------------------------------------------------

#include "Defines.h" // General definitions shared by all source files
#include "Camera.h"  // Declaration of this class

int CCamera::mViewportWidth, CCamera::mViewportHeight;	

///////////////////////////////
// Constructors / Destructors
// Constructor - initialise all camera settings - look at the constructor declaration in the header file to see that there are defaults provided for everything
CCamera::CCamera( D3DXVECTOR3 position, D3DXVECTOR3 rotation, float fov, float nearClip, float farClip )
{
	m_Position = position;
	m_Rotation = rotation;
	

	SetFOV( fov );
	SetNearClip( nearClip );
	SetFarClip( farClip );

	MoveSpeed = 50.0f;
	RotSpeed = 2.0f;

	UpdateMatrices();
}


///////////////////////////////
// Camera Usage

// Update the matrices used for the camera in the rendering pipeline. Treat the camera like a model and create a world matrix for it. Then convert that into
// the view matrix that the rendering pipeline actually uses. Also create the projection matrix, a second matrix that only cameras have
void CCamera::UpdateMatrices()
{
	// Make matrices for position and rotations, then multiply together to get a "camera world matrix"
	D3DXMATRIXA16 matrixXRot, matrixYRot, matrixZRot, matrixTranslation;
	matrixXRot = DirectX::XMMatrixRotationAxis( XAXIS, m_Rotation.x );
	matrixYRot = DirectX::XMMatrixRotationAxis( YAXIS, m_Rotation.y );
	matrixZRot = DirectX::XMMatrixRotationAxis( ZAXIS, m_Rotation.z );

	matrixTranslation = DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	//D3DXMatrixTranslation( &matrixTranslation, m_Position.x, m_Position.y, m_Position.z);
	m_WorldMatrix = matrixZRot * matrixXRot * matrixYRot * matrixTranslation;

	// The rendering pipeline actually needs the inverse of the camera world matrix - called the view matrix. Creating an inverse is easy with DirectX:
    //D3DXMatrixInverse( &m_ViewMatrix, NULL, &m_WorldMatrix );
	//don't need this, but the parameter is not optional in some versions
	DirectX::XMVECTOR tempVector;
	//m_ViewMatrix = DirectX::XMMatrixInverse(&tempVector, m_WorldMatrix );

	// Initialize the projection matrix. This determines viewing properties of the camera such as field of view (FOV) and near clip distance
	// One other factor in the projection matrix is the aspect ratio of screen (width/height) - used to adjust FOV between horizontal and vertical
	float aspect = (float)mViewportWidth / mViewportHeight; 
	//D3DXMatrixPerspectiveFovLH( &m_ProjMatrix, m_FOV, aspect, m_NearClip, m_FarClip );
	m_ProjMatrix = DirectX::XMMatrixPerspectiveFovLH( m_FOV, aspect, m_NearClip, m_FarClip );

	// Combine the view and projection matrix into a single matrix - which can (optionally) be used in the vertex shaders to save one matrix multiply per vertex
	m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;
}


// Control the camera's position and rotation using keys provided. Amount of motion performed depends on frame time
void CCamera::Control( float frameTime, EKeyCode turnUp, EKeyCode turnDown, EKeyCode turnLeft, EKeyCode turnRight,  
                       EKeyCode moveForward, EKeyCode moveBackward, EKeyCode moveLeft, EKeyCode moveRight)
{
	if (KeyHeld( turnDown ))
	{
		m_Rotation.x += RotSpeed * frameTime;
	}
	if (KeyHeld( turnUp ))
	{
		m_Rotation.x -= RotSpeed * frameTime;
	}
	if (KeyHeld( turnRight ))
	{
		m_Rotation.y += RotSpeed * frameTime;
	}
	if (KeyHeld( turnLeft ))
	{
		m_Rotation.y -= RotSpeed * frameTime;
	}

	// Local X movement - move in the direction of the X axis, get axis from camera's "world" matrix
	if (KeyHeld( moveRight ))
	{
		m_Position.x += m_WorldMatrix.r[0].m128_f32[0] * MoveSpeed * frameTime;
		m_Position.y += m_WorldMatrix.r[0].m128_f32[1] * MoveSpeed * frameTime;
		m_Position.z += m_WorldMatrix.r[0].m128_f32[2] * MoveSpeed * frameTime;
	}
	if (KeyHeld( moveLeft ))
	{
		m_Position.x -= m_WorldMatrix.r[0].m128_f32[0] * MoveSpeed * frameTime;
		m_Position.y -= m_WorldMatrix.r[0].m128_f32[1] * MoveSpeed * frameTime;
		m_Position.z -= m_WorldMatrix.r[0].m128_f32[2] * MoveSpeed * frameTime;
	}

	// Local Z movement - move in the direction of the Z axis, get axis from view matrix
	if (KeyHeld( moveForward ))
	{
		m_Position.x += m_WorldMatrix.r[2].m128_f32[0] * MoveSpeed * frameTime;
		m_Position.y += m_WorldMatrix.r[2].m128_f32[1] * MoveSpeed * frameTime;
		m_Position.z += m_WorldMatrix.r[2].m128_f32[2] * MoveSpeed * frameTime;
	}
	if (KeyHeld( moveBackward ))
	{
		m_Position.x -= m_WorldMatrix.r[2].m128_f32[0] * MoveSpeed * frameTime;
		m_Position.y -= m_WorldMatrix.r[2].m128_f32[1] * MoveSpeed * frameTime;
		m_Position.z -= m_WorldMatrix.r[2].m128_f32[2] * MoveSpeed * frameTime;
	}
}


bool CCamera::PixelFromWorldPt( D3DXVECTOR2* pixel, D3DXVECTOR3 world)
{
	D3DXVECTOR4 WorldPick = D3DXVECTOR4( world.x, world.y, world.z, 1.0f);
	//D3DXVec4Transform(&WorldPick, &WorldPick, &m_ViewProjMatrix);
	DirectX::XMVECTOR temp;
	temp = DirectX::XMVector4Transform(DirectX::XMVectorSet(WorldPick.x, WorldPick.y, WorldPick.z, WorldPick.w), m_ViewProjMatrix);
	WorldPick.x = DirectX::XMVectorGetX(temp);
	WorldPick.y = DirectX::XMVectorGetY(temp);
	WorldPick.z = DirectX::XMVectorGetZ(temp);

	if( WorldPick.w >= 0 )
	{
		WorldPick.x /= WorldPick.w;
		WorldPick.y /= WorldPick.w;

		pixel->x = (WorldPick.x + 1) * mViewportWidth / 2;
		pixel->y = (1 - WorldPick.y) * mViewportHeight / 2; 

		return true;
	}

	return false;
}