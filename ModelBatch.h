#pragma once
#include "Defines.h"

#include "Model.h"

class ModelBatch
{
private:
	CModel* mp_model;
	DirectX::XMFLOAT3 md_colour;
	DirectX::XMMATRIX md_matrix[MAX_MODEL_INSTANCES];
	uint32_t mi_count;
	
public:

	ID3DX11EffectTechnique* mp_technique;

	void SetColour(DirectX::XMFLOAT3 colour){ md_colour = colour;}

	DirectX::XMMATRIX* AddInstance( DirectX::XMFLOAT3 position );
	DirectX::XMMATRIX* AddInstance( DirectX::XMFLOAT3 position, float scale );
	DirectX::XMMATRIX* AddInstance( DirectX::XMFLOAT3 position, float scale, DirectX::XMFLOAT3 rotation );

	bool Load( std::string fileName, ID3DX11EffectTechnique* shaderCode, bool tangents = false );

	void RenderBatch( ID3DX11EffectMatrixVariable* WorldMatrixPointer, ID3DX11EffectVectorVariable* ModelColourPointer );

	ModelBatch();
	~ModelBatch();
};

