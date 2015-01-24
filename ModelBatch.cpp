#include "ModelBatch.h"


ModelBatch::ModelBatch()
{
	mi_count = 0;
	md_colour = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
	mp_model = new CModel( DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ), 1.0f );
}


ModelBatch::~ModelBatch()
{
	//mp_model->ReleaseResources();
	delete mp_model;
}


DirectX::XMMATRIX* ModelBatch::AddInstance( DirectX::XMFLOAT3 position )
{
	mp_model->SetPosition( position );
	mp_model->SetScale(1.0f);
	mp_model->SetRotation(DirectX::XMFLOAT3(0.0f,0.0f,0.0f));
	mp_model->UpdateMatrix();
	md_matrix[mi_count] =  mp_model->GetWorldMatrix();
	mi_count++;
	return &md_matrix[mi_count-1];
}

DirectX::XMMATRIX* ModelBatch::AddInstance( DirectX::XMFLOAT3 position, float scale )
{
	mp_model->SetPosition( position );
	mp_model->SetScale( scale );
	mp_model->SetRotation( DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	mp_model->UpdateMatrix();
	md_matrix[mi_count] = mp_model->GetWorldMatrix();
	mi_count++;
	return &md_matrix[mi_count - 1];
}

DirectX::XMMATRIX* ModelBatch::AddInstance( DirectX::XMFLOAT3 position, float scale, DirectX::XMFLOAT3 rotation )
{
	mp_model->SetPosition( position );
	mp_model->SetScale( 1.0f );
	mp_model->SetRotation( DirectX::XMFLOAT3( 0.0f, 0.0f, 0.0f ) );
	mp_model->UpdateMatrix();
	md_matrix[mi_count] = mp_model->GetWorldMatrix();
	mi_count++;
	return &md_matrix[mi_count - 1];
}

bool ModelBatch::Load( std::string fileName, ID3DX11EffectTechnique* shaderCode, bool tangents )
{
	mp_technique = shaderCode;
	return mp_model->Load(fileName, shaderCode, tangents);
}

void ModelBatch::RenderBatch( ID3DX11EffectMatrixVariable* WorldMatrixPointer, ID3DX11EffectVectorVariable* ModelColourPointer )
{
	//V3 temp = XMF3ToFloat3( md_colour );
	mp_model->SendGeometry();
	mp_model->SetTechnique(mp_technique);
	ModelColourPointer->SetRawValue( &md_colour, 0, 12 );
	for( uint32_t i = 0; i < mi_count; i++ )
	{
		WorldMatrixPointer->SetMatrix((float*) md_matrix[i].r );
		mp_model->QuickRender();
	}
}