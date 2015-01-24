//--------------------------------------------------------------------------------------
//	Model.cpp
//
//	The model class collects together a model's geometry (vertex and index data) and
//	also manages it's positioning with a world matrix
//--------------------------------------------------------------------------------------

#include "Model.h"   // Declaration of this class

#include "CImportXFile.h"

ID3D11Device* CModel::mpd3dDev;
ID3D11DeviceContext* CModel::mp_d3dDeviceContext;

///////////////////////////////
// Constructors / Destructors

// Constructor - initialise all camera settings - look at the constructor declaration in the header file to see that there are defaults provided for everything
CModel::CModel( DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, float scale )
{
	m_Position = position;
	m_Rotation = rotation;
	SetScale( scale );
	UpdateMatrix();

	// Good practice to ensure all private data is sensibly initialised
	m_VertexBuffer = NULL;
	m_NumVertices = 0;
	m_VertexSize = 0;
	m_VertexLayout = NULL;

	m_IndexBuffer = NULL;
	m_NumIndices = 0;

	m_HasGeometry = false;

	MoveSpeed = 50.0f;
	RotSpeed = 2.0f;

	mi_meshSubsets = 0;
}

// Model destructor
CModel::~CModel()
{
	ReleaseResources();
}

// Release resources used by model
void CModel::ReleaseResources()
{
	// Release resources
	SAFE_RELEASE( m_IndexBuffer );  // Using a DirectX helper macro to simplify code here - look it up in Defines.h
	SAFE_RELEASE( m_VertexBuffer );
	SAFE_RELEASE( m_VertexLayout );
	m_HasGeometry = false;
	mi_meshSubsets = 0;
}


/////////////////////////////
// Model Loading

bool CModel::LoadX(const string& fileName, ID3D10EffectTechnique* exampleTechnique, bool tangents )
{
	gen::CImportXFile mesh;
	if( mesh.ImportFile( fileName.c_str() ) != gen::kSuccess )
	{
		return false;
	}

	// Get first sub-mesh from loaded file
	gen::SSubMesh subMesh;
	if( mesh.GetSubMesh( 0, &subMesh, tangents ) != gen::kSuccess )
	{
		return false;
	}


	// Create vertex element list & layout. We need a vertex layout to say what data we have per vertex in this model (e.g. position, normal, uv, etc.)
	// In previous projects the element list was a manually typed in array as we knew what data we would provide. However, as we can load models with
	// different vertex data this time we need flexible code. The array is built up one element at a time: ask the import class if it loaded normals, 
	// if so then add a normal line to the array, then ask if it loaded UVS...etc
	unsigned int numElts = 0;
	unsigned int offset = 0;
	// Position is always required
	m_VertexElts[numElts].SemanticName = "POSITION";   // Semantic in HLSL (what is this data for)
	m_VertexElts[numElts].SemanticIndex = 0;           // Index to add to semantic (a count for this kind of data, when using multiple of the same type, e.g. TEXCOORD0, TEXCOORD1)
	m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32_FLOAT; // Type of data - this one will be a float3 in the shader. Most data communicated as though it were colours
	m_VertexElts[numElts].AlignedByteOffset = offset;  // Offset of element from start of vertex data (e.g. if we have position (float3), uv (float2) then normal, the normal's offset is 5 floats = 5*4 = 20)
	m_VertexElts[numElts].InputSlot = 0;               // For when using multiple vertex buffers (e.g. instancing - an advanced topic)
	m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // Use this value for most cases (only changed for instancing)
	m_VertexElts[numElts].InstanceDataStepRate = 0;                     // --"--
	offset += 12;
	++numElts;
	// Repeat for each kind of vertex data
	if( subMesh.hasSkinningData ) // If sub-mesh contains skinning data
	{
		m_VertexElts[numElts].SemanticName = "BLENDWEIGHTS";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 16;
		++numElts;
		m_VertexElts[numElts].SemanticName = "BLENDINDICES";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R8G8B8A8_UINT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 4;
		++numElts;
	}
	if( subMesh.hasNormals )
	{
		m_VertexElts[numElts].SemanticName = "NORMAL";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 12;
		++numElts;
	}
	if( subMesh.hasTangents )
	{
		m_VertexElts[numElts].SemanticName = "TANGENT";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 12;
		++numElts;
	}
	if( subMesh.hasTextureCoords )
	{
		m_VertexElts[numElts].SemanticName = "TEXCOORD";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R32G32_FLOAT;
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 8;
		++numElts;
	}
	if( subMesh.hasVertexColours )
	{
		m_VertexElts[numElts].SemanticName = "COLOR";
		m_VertexElts[numElts].SemanticIndex = 0;
		m_VertexElts[numElts].Format = DXGI_FORMAT_R8G8B8A8_UNORM; // A RGBA colour with 1 byte (0-255) per component
		m_VertexElts[numElts].AlignedByteOffset = offset;
		m_VertexElts[numElts].InputSlot = 0;
		m_VertexElts[numElts].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		m_VertexElts[numElts].InstanceDataStepRate = 0;
		offset += 4;
		++numElts;
	}
	m_VertexSize = offset;

	// Given the vertex element list, pass it to DirectX to create a vertex layout. We also need to pass an example of a technique that will
	// render this model. We will only be able to render this model with techniques that have the same vertex input as the example we use here
	D3DX11_PASS_DESC PassDesc;
	exampleTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	mpd3dDev->CreateInputLayout( m_VertexElts, numElts, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_VertexLayout );


	// Create the vertex buffer and fill it with the loaded vertex data
	m_NumVertices = subMesh.numVertices;
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT; // Not a dynamic buffer
	bufferDesc.ByteWidth = m_NumVertices * m_VertexSize; // Buffer size
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData; // Initial data
	initData.pSysMem = subMesh.vertices;
	if( FAILED( mpd3dDev->CreateBuffer( &bufferDesc, &initData, &m_VertexBuffer ) ) )
	{
		return false;
	}


	// Create the index buffer - assuming 2-byte (WORD) index data
	m_NumIndices = static_cast<unsigned int>(subMesh.numFaces) * 3;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = m_NumIndices * sizeof( WORD );
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	initData.pSysMem = subMesh.faces;
	if( FAILED( mpd3dDev->CreateBuffer( &bufferDesc, &initData, &m_IndexBuffer ) ) )
	{
		return false;
	}

	m_HasGeometry = true;
	return true;
}


bool CModel::LoadOBJ(const string& fileName, ID3D10EffectTechnique* shaderCode, bool tangents )
{
	HRESULT hr = 0;
	std::wifstream fileInput(fileName.c_str() );
	std::wstring meshMatLib;
	
	std::vector<uint32_t> indicies;
	std::vector<DirectX::XMFLOAT3> vertexPos;
	std::vector<DirectX::XMFLOAT3> vertexNormal;
	std::vector<DirectX::XMFLOAT2> UV;
	std::vector<std::string> materials;

	std::vector<uint32_t> vertexPosIndex;
	std::vector<uint32_t> vertexNormIndex;
	std::vector<uint32_t> UVIndex;

	bool hasUV = false;
	bool hasNormal = false;

	std::string materialTemp;
//	char buffer[1024];
	uint32_t vertexPosIndexTemp;
	uint32_t vertexNormIndexTemp;
	uint32_t UVTemp;

	char charInput;
	std::string face;
	uint32_t vIndex = 0;
	uint32_t numTriangles = 0;
	uint32_t numVerts = 0;
	uint32_t meshTriangles = 0;

	float x,y,z;

	if(! fileInput.is_open() )
	{
		return false;
	}

	while( fileInput )
	{
		charInput = (char) fileInput.get(); // get a single character.
		switch( charInput )
		{
		case '#': //comment
			while( charInput != '\n' )
			{
				charInput = (char) fileInput.get();
			}
			break;
		case 'v': //vertex something

			charInput = (char) fileInput.get();
			switch( charInput )
			{
			case ' ': //v - position
				fileInput >> x >> y >> z;
				vertexPos.push_back(DirectX::XMFLOAT3(x,y,z));
				break;
			case 't': //vt - UV
				fileInput >> x, y;
				UV.push_back(DirectX::XMFLOAT2(x,y));
				hasUV = true;
				break;
			case 'n': //vn - normals
				fileInput >> x >> y >> z;
				vertexNormal.push_back(DirectX::XMFLOAT3(x,y,z));
				hasNormal = true;
				break;
			}//end inner switch (second character)
			break;

		case 'g': //group
			charInput = (char) fileInput.get();
			if(charInput == ' ')
			{
				mv_meshSubsetIndexStart.push_back(vIndex);
				mi_meshSubsets++;
			}
		case 'f': //f - face
			charInput = (char) fileInput.get();
			if(charInput == ' ')
			{
				face = "";
				std::string vertDef;
				numTriangles = 0;

				charInput = (char) fileInput.get();
				while( charInput != '\n' )
				{
					face += charInput;
					charInput = (char) fileInput.get();
					if( charInput == ' ')
					{
						numTriangles++;
					}
				}

				//decrement count as first 3 nodes define one triangle
				numTriangles--;
				
				//check for a trailing space
				if( face[face.length()-1] == ' ' )
				{
					numTriangles--;
				}

				std::stringstream ss(face);
				
				if( face.length() > 0)
				{
					uint32_t firstVertex, lastVertex;

					for(int i = 0; i < 3; i++)
					{
						ss >> vertDef;
						std::string vertPart;
						uint32_t whichPart = 0;
						
						for(uint32_t j = 0; j < vertDef.length(); j++ )
						{
							if(vertDef[j] != '/')
							{
								vertPart += vertDef[j];
							}

							if(vertDef[j] == '/' || j == vertDef.length()-1 )
							{
								std::stringstream stringToInt(vertPart);
								switch(whichPart)
								{
								case 0:// position
									stringToInt >> vertexPosIndexTemp;
									vertexPosIndexTemp--; //OBJ starts at 1, not 0. Heathens. 

									if( j == vertDef.length()-1 )
									{
										vertexNormIndexTemp = 0;
										UVTemp = 0;
									}
									break;
								case 1://UV
									if(vertPart != "")
									{
										stringToInt >> UVTemp;
										UVTemp--; //starting at 1, who does that?
									}
									else
									{
										UVTemp = 0;
									}
									if( j == vertDef.length() - 1)
									{
										vertexNormIndexTemp = 0;
									}
									break;
								case 2://normal
									std::stringstream sToI(vertPart);
									sToI >> vertexNormIndexTemp;
									vertexNormIndexTemp--; //seriously? who does that?
									break;
								}//end switch whichPart

								vertPart = "";
								whichPart++;
							}// end if j == / or length -1
						}// end for loop


						if(mi_meshSubsets == 0)
						{
							mv_meshSubsetIndexStart.push_back(vIndex);
							mi_meshSubsets++;
						}

						bool vertExists = false;
						if(numVerts >= 3)
						{
							for(uint32_t i = 0; i < numVerts; i++)
							{
								if(vertexPosIndexTemp == vertexPosIndex[i] && !vertExists)
								{
									if(UVTemp == UVIndex[i])
									{
										indicies.push_back(i);
										vertExists = true;
									}
								}
							}
						}

						if(!vertExists)
						{
							vertexPosIndex.push_back(vertexPosIndexTemp);
							UVIndex.push_back(UVTemp);
							vertexNormIndex.push_back(vertexNormIndexTemp);
							indicies.push_back(numVerts);
							numVerts++;
						}

						if(i==0)
						{
							firstVertex = indicies[vIndex];
						}

						if(i==2)
						{
							lastVertex = indicies[vIndex];
						}
						vIndex++;

					}

					meshTriangles++;

					for( uint32_t i = 0; i < numTriangles-1; i++ )
					{
						indicies.push_back(firstVertex);
						vIndex++;

						indicies.push_back(lastVertex);
						vIndex++;

						ss >> vertDef;

						std::string vertPart;
						uint32_t whichPart = 0;

						for (uint32_t j =0; j < vertDef.length(); j++)
						{
							if(vertDef[j] != '/')
							{
								vertPart += vertDef[j];
							}

							if(vertDef[j] == '/' || j == vertDef.length()-1 )
							{
								std::stringstream stringToInt(vertPart);
								switch(whichPart)
								{
								case 0:// position
									stringToInt >> vertexPosIndexTemp;
									vertexPosIndexTemp--; //OBJ starts at 1, not 0. Heathens. 

									if( j == vertDef.length()-1 )
									{
										vertexNormIndexTemp = 0;
										UVTemp = 0;
									}
									break;
								case 1://UV
									if(vertPart != "")
									{
										stringToInt >> UVTemp;
										UVTemp--; //starting at 1, who does that?
									}
									else
									{
										UVTemp = 0;
									}
									if( j == vertDef.length() - 1)
									{
										vertexNormIndexTemp = 0;
									}
									break;
								case 2://normal
									std::stringstream sToI(vertPart);
									sToI >> vertexNormIndexTemp;
									vertexNormIndexTemp--; //seriously? who does that?
									break;
								}//end switch whichPart

								vertPart = "";
								whichPart++;
							}// end if j == / or length -1
						}

						bool vertExists = false;
						if(numVerts >= 3)
						{
							for(uint32_t i = 0; i < numVerts; i++)
							{
								if(vertexPosIndexTemp == vertexPosIndex[i] && !vertExists)
								{
									if(UVTemp == UVIndex[i])
									{
										indicies.push_back(i);
										vertExists = true;
									}
								}
							}
						}
						
						if(!vertExists)
						{
							vertexPosIndex.push_back(vertexPosIndexTemp);
							UVIndex.push_back(UVTemp);
							vertexNormIndex.push_back(vertexNormIndexTemp);
							indicies.push_back(numVerts);
							numVerts++;
						}

						lastVertex = indicies[vIndex];

						numTriangles++;
						vIndex++;
					}
				}
			}
			break;// end case f - face. 

		case 'm': // material library
			charInput = (char) fileInput.get();
			if(charInput == 't')
			{
				charInput = (char)fileInput.get();
				if(charInput == 'l')
				{
					charInput = (char) fileInput.get();
					if(charInput == 'l')
					{
						charInput = (char)fileInput.get();
						if(charInput == 'i')
						{
							charInput = (char) fileInput.get();
							if(charInput == ' b')
							{
								charInput = (char) fileInput.get();
								if(charInput == ' ')
								{
									fileInput >> meshMatLib;
								}
							}
						}
					}
				}
			}
			break;//end case m
		case 'u': //which material to use
			charInput = (char)fileInput.get();
			if(charInput == 's')
			{
				charInput = (char) fileInput.get();
				if(charInput == 'e')
				{
					charInput = (char) fileInput.get();
					if(charInput == 'm')
					{
						charInput = (char) fileInput.get();
						if(charInput == 't')
						{
							charInput = (char) fileInput.get();
							if(charInput == ' l')
							{
								charInput = (char) fileInput.get();
								if(charInput == ' ')
								{
									std::wstring meshMatTemp = L"";
									fileInput >> meshMatTemp;
								}
							}
						}
					}
				}
			}
			break; //end case u

		default:
			break;
		}//end outer switch (first character)
	}//end while ( read file)

	mv_meshSubsetIndexStart.push_back(vIndex);

	if(mv_meshSubsetIndexStart[1] == 0)
	{
		mv_meshSubsetIndexStart.erase(mv_meshSubsetIndexStart.begin()+1);
		mi_meshSubsets--;
	}

	if(!hasNormal)
		vertexNormal.push_back( DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) );
	if(!hasUV)
		UV.push_back( DirectX::XMFLOAT2(0.0f, 0.0f) );

	fileInput.close();

	//Done with geometry, fetching materials. 

	fileInput.open(meshMatLib.c_str());

	std::wstring stringInput;
	uint32_t matCount = mv_material.size();

	bool kdset = false;

	if(fileInput)
	{
		while(fileInput)
		{
			charInput = (char) fileInput.get();
			
			switch(charInput)
			{
			case '#':
				while(charInput != '\n')
				{
					charInput = (char) fileInput.get();
				}
				break;
			case 'K':
				charInput = (char) fileInput.get();
				switch(charInput)
				{
				case 'd':
					charInput = (char) fileInput.get();
					fileInput >> mv_material[matCount].difColor.x;
					fileInput >> mv_material[matCount].difColor.y;
					fileInput >> mv_material[matCount].difColor.z;
					kdset = true;
					break;
				case 'a':
					charInput = (char) fileInput.get();
					if(!kdset)
					{
						fileInput >> mv_material[matCount].difColor.x;
						fileInput >> mv_material[matCount].difColor.y;
						fileInput >> mv_material[matCount].difColor.z;
					}
					break;
				}//switch inside case K
				break;//end case'K'
			case 'T':
				charInput = (char) fileInput.get();
				if(charInput == 'r')
				{
					charInput = (char) fileInput.get();
					float transparency;
					fileInput >> transparency;

					mv_material[matCount].difColor.w = 1.0f - transparency;
					if( transparency < 1.0f )
					{
						mv_material[matCount].transparent = true;
					}
				}
				break;//end case 'T'

			case 'd':
				charInput = (char) fileInput.get();
				if( charInput == ' ' )
				{
					float transparency;
					fileInput >> transparency;

					mv_material[matCount].difColor.w = 1.0f - transparency;
					if( transparency < 1.0f )
					{
						mv_material[matCount].transparent = true;
					}
				}
				break; //end case 'd'
			case 'm':
				charInput = (char) fileInput.get();
				if(charInput == 'a')
				{
					charInput = (char) fileInput.get();
					if(charInput == 'p')
					{
						charInput = (char) fileInput.get();
						if(charInput == '_')
						{
							charInput = (char) fileInput.get();
							if(charInput == 'K')
							{
								charInput = (char) fileInput.get();
								if(charInput == 'd')
								{
									std::wstring fileNamePath;

									fileInput.get();

									bool textFilePathEnd = false;

									while(! textFilePathEnd )
									{
										charInput = (char) fileInput.get();

										fileNamePath += charInput;

										if(charInput == '.')
										{
											for(uint32_t i =0; i< 3; i++ )
											{
												fileNamePath += fileInput.get();
											}
											textFilePathEnd = true;
										}
									}

									/*bool alreadyLoaded = false;
									for(int i = 0; i < textureNameArray.size(); ++i)
									{
										if(fileNamePath == textureNameArray[i])
										{
											alreadyLoaded = true;
											material[matCount-1].texArrayIndex = i;
											material[matCount-1].hasTexture = true;
										}
									}

									//if the texture is not already loaded, load it now
									if(!alreadyLoaded)
									{
										ID3D11ShaderResourceView* tempMeshSRV;
										hr = D3DX11CreateShaderResourceViewFromFile( d3d11Device, fileNamePath.c_str(),
											NULL, NULL, &tempMeshSRV, NULL );
										if(SUCCEEDED(hr))
										{
											textureNameArray.push_back(fileNamePath.c_str());
											material[matCount-1].texArrayIndex = meshSRV.size();
											meshSRV.push_back(tempMeshSRV);
											material[matCount-1].hasTexture = true;
										}
									}*/
								}
							}
						}
					}
				}
			case 'n':	//newmtl - Declare new material
				/*checkChar = fileIn.get();
				if(checkChar == 'e')
				{
					checkChar = fileIn.get();
					if(checkChar == 'w')
					{
						checkChar = fileIn.get();
						if(checkChar == 'm')
						{
							checkChar = fileIn.get();
							if(checkChar == 't')
							{
								checkChar = fileIn.get();
								if(checkChar == 'l')
								{
									checkChar = fileIn.get();
									if(checkChar == ' ')
									{
										//New material, set its defaults
										SurfaceMaterial tempMat;
										material.push_back(tempMat);
										fileIn >> material[matCount].matName;
										material[matCount].transparent = false;
										material[matCount].hasTexture = false;
										material[matCount].texArrayIndex = 0;
										matCount++;
										kdset = false;
									}
								}
							}
						}
					}
				}*/
				break;

			default:
				break;
							
			}//end switch first character.



		}
	}

	for(uint32_t i = 0; i < mi_meshSubsets; i++)
	{
		bool hasMat = false;
		for( uint32_t j = 0; j < mv_material.size(); j++ )
		{
			/*if(meshMaterials[i] == material[j].matName)
			{
				subsetMaterialArray.push_back(j);
				hasMat = true;
			}*/
		}
		mv_meshSubsetTexture.size();

	}
	struct Vertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT2 UV;
	};

	std::vector<Vertex> vertices;
	Vertex tempVert;

	for(uint32_t i = 0; i < numVerts; i++)
	{
		tempVert.pos = vertexPos[vertexPosIndex[i]];
		tempVert.norm = vertexNormal[vertexNormIndex[i]];
		tempVert.UV = UV[UVIndex[i]];

		vertices.push_back(tempVert);
	}

	/*if(computeNormals)
	{
		std::vector<XMFLOAT3> tempNormal;

		//normalized and unnormalized normals
		XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

		//Used to get vectors (sides) from the position of the verts
		float vecX, vecY, vecZ;

		//Two edges of our triangle
		XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Compute face normals
		for(int i = 0; i < meshTriangles; ++i)
		{
			//Get the vector describing one edge of our triangle (edge 0,2)
			vecX = vertices[indices[(i*3)]].pos.x - vertices[indices[(i*3)+2]].pos.x;
			vecY = vertices[indices[(i*3)]].pos.y - vertices[indices[(i*3)+2]].pos.y;
			vecZ = vertices[indices[(i*3)]].pos.z - vertices[indices[(i*3)+2]].pos.z;		
			edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

			//Get the vector describing another edge of our triangle (edge 2,1)
			vecX = vertices[indices[(i*3)+2]].pos.x - vertices[indices[(i*3)+1]].pos.x;
			vecY = vertices[indices[(i*3)+2]].pos.y - vertices[indices[(i*3)+1]].pos.y;
			vecZ = vertices[indices[(i*3)+2]].pos.z - vertices[indices[(i*3)+1]].pos.z;		
			edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

			//Cross multiply the two edge vectors to get the un-normalized face normal
			XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
			tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
		}

		//Compute vertex normals (normal Averaging)
		XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		int facesUsing = 0;
		float tX;
		float tY;
		float tZ;

		//Go through each vertex
		for(int i = 0; i < totalVerts; ++i)
		{
			//Check which triangles use this vertex
			for(int j = 0; j < meshTriangles; ++j)
			{
				if(indices[j*3] == i ||
					indices[(j*3)+1] == i ||
					indices[(j*3)+2] == i)
				{
					tX = XMVectorGetX(normalSum) + tempNormal[j].x;
					tY = XMVectorGetY(normalSum) + tempNormal[j].y;
					tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

					normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum
					facesUsing++;
				}
			}

			//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
			normalSum = normalSum / facesUsing;

			//Normalize the normalSum vector
			normalSum = XMVector3Normalize(normalSum);

			//Store the normal in our current vertex
			vertices[i].normal.x = XMVectorGetX(normalSum);
			vertices[i].normal.y = XMVectorGetY(normalSum);
			vertices[i].normal.z = XMVectorGetZ(normalSum);

			//Clear normalSum and facesUsing for next vertex
			normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			facesUsing = 0;

		}
	}*/

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * meshTriangles*3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indicies[0];
	mpd3dDev->CreateBuffer(&indexBufferDesc, &iinitData, &m_IndexBuffer);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * numVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &vertices[0];
	hr = mpd3dDev->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &m_VertexBuffer);

	return true;
}

bool CModel::Load( const string& fileName, ID3D10EffectTechnique* exampleTechnique, bool tangents /*= false*/ ) // The commented out bit is the default parameter (can't write it here, only in the declaration)
{
	// Release any existing geometry in this object
	ReleaseResources();

	//Use CImportXFile class (from another application) to load the given file. The import code is wrapped in the namespace 'gen'
	std::size_t pos = fileName.find( "." );
	std::string extension = fileName.substr(pos);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if(extension == ".obj")
	{
		return LoadOBJ(fileName, exampleTechnique, tangents);
	}

	if(extension == ".x" )
	{
		return LoadX(fileName, exampleTechnique, tangents);
	}


	return false;
}


/////////////////////////////
// Model Usage

// Update the world matrix of the model from its position, rotation and scaling
void CModel::UpdateMatrix()
{
	// Make a matrix for position and scaling, and one each for X, Y & Z rotation
	DirectX::XMMATRIX matrixXRot, matrixYRot, matrixZRot, matrixTranslation, matrixScaling;
	matrixXRot = DirectX::XMMatrixRotationAxis( XAXIS, m_Rotation.x );
	matrixYRot = DirectX::XMMatrixRotationAxis( YAXIS, m_Rotation.y );
	matrixZRot = DirectX::XMMatrixRotationAxis( ZAXIS, m_Rotation.z );
	//D3DXMatrixTranslation( &matrixTranslation, m_Position.x, m_Position.y, m_Position.z );
	matrixTranslation = DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	matrixScaling = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z );
	//D3DXMatrixScaling( &matrixScaling, m_Scale.x, m_Scale.y, m_Scale.z );

	// Multiply above matrices together to get the effect of them all combined - this makes the world matrix for the rendering pipeline
	// Order of multiplication is important, get slightly different control mechanism depending on order
	m_WorldMatrix = matrixScaling * matrixZRot * matrixXRot * matrixYRot * matrixTranslation;
}


// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
void CModel::Control( float frameTime, EKeyCode turnUp, EKeyCode turnDown, EKeyCode turnLeft, EKeyCode turnRight,  
                      EKeyCode turnCW, EKeyCode turnCCW, EKeyCode moveForward, EKeyCode moveBackward )
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
	if (KeyHeld( turnCW ))
	{
		m_Rotation.z += RotSpeed * frameTime;
	}
	if (KeyHeld( turnCCW ))
	{
		m_Rotation.z -= RotSpeed * frameTime;
	}

	// Local Z movement - move in the direction of the Z axis, get axis from world matrix
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


// Render the model with the given technique. Assumes any shader variables for the technique have already been set up (e.g. matrices and textures)
void CModel::Render( ID3DX11EffectTechnique* technique )
{
	// Don't render if no geometry
	if (!m_HasGeometry)
	{
		return;
	}

	// Don't redner without a technique. Causes odd problems. 
	if( technique == nullptr )
	{
		return;
	}

	if( ! technique->IsValid() )
	{
		return;
	}

	// Select vertex and index buffer - assuming all data will be as triangle lists
	UINT offset = 0;
	
	mp_d3dDeviceContext->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &m_VertexSize, &offset );
	mp_d3dDeviceContext->IASetInputLayout( m_VertexLayout );
	mp_d3dDeviceContext->IASetIndexBuffer( m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	mp_d3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Render the model. All the data and shader variables are prepared, now select the technique to use and draw.
	// The loop is for advanced techniques that need multiple passes - we will only use techniques with one pass
	D3DX11_TECHNIQUE_DESC techDesc;
	technique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		technique->GetPassByIndex( p )->Apply( 0, mp_d3dDeviceContext );
		mp_d3dDeviceContext->DrawIndexed( m_NumIndices, 0, 0 );
	}
	mp_d3dDeviceContext->DrawIndexed( m_NumIndices, 0, 0 );
}

void CModel::SendGeometry()
{
	// Don't render if no geometry
	if (!m_HasGeometry)
	{
		return;
	}

	// Select vertex and index buffer - assuming all data will be as triangle lists
	UINT offset = 0;
	mp_d3dDeviceContext->IASetVertexBuffers( 0, 1, &m_VertexBuffer, &m_VertexSize, &offset );
	mp_d3dDeviceContext->IASetInputLayout( m_VertexLayout );
	mp_d3dDeviceContext->IASetIndexBuffer( m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	mp_d3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

}

void CModel::SetTechnique(  ID3DX11EffectTechnique* technique )
{
	m_technique = technique;
	technique->GetDesc( &m_techDesc );
	technique->GetPassByIndex(0)->Apply(0, mp_d3dDeviceContext);
}

void CModel::QuickRender()
{
	m_technique->GetDesc( &m_techDesc );
	m_technique->GetPassByIndex(0)->Apply(0, mp_d3dDeviceContext);
	mp_d3dDeviceContext->DrawIndexed( m_NumIndices, 0, 0 );
}