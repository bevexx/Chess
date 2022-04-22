#include "stdafx.h"

#include "shaders/shader_model.h"
#include "engine.h"
#include "d3d/direct_3d.h"
#include "render/render_object.h"
#include "sprite/textured_quad.h"
#include "texture/texture.h"
#include "light/light_manager.h"
#include "texture/texture_manager.h"
#include <tga2d/primitives/model_shape.h>

using namespace Tga2D;

CShaderModel::CShaderModel(CEngine* aEngine)
	:CShader(aEngine)
{
}


CShaderModel::~CShaderModel()
{
}

struct SCommonBuf
{
	Vector4f myPosition;
	Vector4f myOffsetRot;
	Vector2f myOffsetPos;
	Vector2f myScale;
	Vector2f myRotation;
	Vector2f myDummy;
};

bool Tga2D::CShaderModel::Init()
{
	// GPU
	D3D11_BUFFER_DESC matrixVertexBufferDesc;
	matrixVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixVertexBufferDesc.ByteWidth = sizeof(Matrix44) * 64;
	matrixVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixVertexBufferDesc.MiscFlags = 0;
	matrixVertexBufferDesc.StructureByteStride = 0;

	HRESULT result = S_OK;
	result = myDirect3dEngine->GetDevice()->CreateBuffer(&matrixVertexBufferDesc, NULL, &myBoneBuffer);

	// GPU
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(SCommonBuf);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;


	result = S_OK;
	result = myDirect3dEngine->GetDevice()->CreateBuffer(&objectBufferDesc, NULL, &myObjectBuffer);
	
	return CShader::CreateShaders("shaders/model_shader_vs.fx", "shaders/model_shader_ps.fx", nullptr);
}

bool  Tga2D::CShaderModel::Init(std::string aPixelShaderFile, std::string aVertexShaderFile, std::string aPixelMainFunction, std::string aVertexMainFunction)
{
	return CShader::CreateShaders(aVertexShaderFile.c_str(), aPixelShaderFile.c_str(), nullptr, aPixelMainFunction, aVertexMainFunction);
}

void Tga2D::CShaderModel::Render(CModelShape* aObject, ID3D11Buffer* aVertexBuffer, ID3D11Buffer* aIndexBuffer, Tga2D::Vector2f aOffsetPos, EulerAngles aRotation)
{
	if (!myIsReadyToRender || !aObject)
	{
		return;
	}

	CModelShape& object = *aObject;

	CShader::Render(&object);

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedVResource;
	result = myDirect3dEngine->GetContext()->Map(myBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVResource);
	if (FAILED(result))
	{
		//INFO_PRINT("Error in rendering!");
		return;
	}
	char* dataVPtr = (char*)mappedVResource.pData;
	if (object.myFrameByFrameData.myFrameByFrameBones)
	{
		memcpy(dataVPtr, &object.myFrameByFrameData.myFrameByFrameBones[0], sizeof(Matrix44) * MAX_ANIMATION_BONES);
	}
	
	myDirect3dEngine->GetContext()->Unmap(myBoneBuffer, 0);
	myDirect3dEngine->GetContext()->VSSetConstantBuffers(5, 1, &myBoneBuffer);


	result = myDirect3dEngine->GetContext()->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVResource);
	if (FAILED(result))
	{
		//INFO_PRINT("Error in rendering!");
		return;
	}
	dataVPtr = (char*)mappedVResource.pData;


	SCommonBuf* dataPtrCommon;
	dataPtrCommon = (SCommonBuf*)mappedVResource.pData;

	dataPtrCommon->myOffsetPos.myX = aOffsetPos.myX;
	dataPtrCommon->myOffsetPos.myY = aOffsetPos.myY;
	dataPtrCommon->myOffsetRot.x = aRotation.roll;
	dataPtrCommon->myOffsetRot.y = aRotation.pitch;
	dataPtrCommon->myOffsetRot.z = aRotation.yaw;
	int xFlipped = object.myFrameByFrameData.myIsFlippedX == false ? 1 : -1;
	dataPtrCommon->myScale.myX = object.myFrameByFrameData.myScale.myX * xFlipped;
	dataPtrCommon->myScale.myY = object.myFrameByFrameData.myScale.myY;
	dataPtrCommon->myPosition.x = object.myFrameByFrameData.myPosition.x;
	dataPtrCommon->myPosition.y = object.myFrameByFrameData.myPosition.y;
	dataPtrCommon->myRotation.x = object.myFrameByFrameData.myRotation;


	myDirect3dEngine->GetContext()->Unmap(myObjectBuffer, 0);
	myDirect3dEngine->GetContext()->VSSetConstantBuffers(4, 1, &myObjectBuffer);



	myDirect3dEngine->GetContext()->IASetIndexBuffer(aIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	SetShaderParameters(&object, nullptr, aVertexBuffer);

	myDirect3dEngine->GetContext()->DrawIndexed(object.myIndexCount, 0, 0);

}

void CreateLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& anArray, LPCSTR aName, int aSemanticIndex, DXGI_FORMAT aFormat, int aInputSlot, D3D11_INPUT_CLASSIFICATION aClassification, int aInstanceStep)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout;
	polygonLayout.SemanticName = aName;
	polygonLayout.SemanticIndex = aSemanticIndex;
	polygonLayout.Format = aFormat;
	polygonLayout.InputSlot = aInputSlot;
	polygonLayout.AlignedByteOffset = anArray.size() > 0 ? D3D11_APPEND_ALIGNED_ELEMENT : 0;
	polygonLayout.InputSlotClass = aClassification;
	polygonLayout.InstanceDataStepRate = aInstanceStep;
	anArray.push_back(polygonLayout);
}

bool Tga2D::CShaderModel::CreateInputLayout(ID3D10Blob* aVS)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

	CreateLayout(layout, "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	CreateLayout(layout, "BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	CreateLayout(layout, "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	CreateLayout(layout, "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	unsigned int numElements = (unsigned int)layout.size();
	HRESULT result = myDirect3dEngine->GetDevice()->CreateInputLayout(&layout[0], numElements, aVS->GetBufferPointer(), aVS->GetBufferSize(), &myLayout);
	if (FAILED(result))
	{
		ERROR_PRINT("%s", "Layout error");
	}
	return true;
}

void CShaderModel::SetShaderParameters(CModelShape* aObject, ID3D11Buffer* aObjectBuffer, ID3D11Buffer* aVertexBuffer)
{
	const unsigned int strides = aObject->myVertexStride;
	const unsigned int offsets = 0;
	myDirect3dEngine->GetContext()->IASetVertexBuffers(0, 1, &aVertexBuffer, &strides, &offsets);
	myDirect3dEngine->GetContext()->VSSetConstantBuffers(EConstantBufferSlot_Object, 1, &aObjectBuffer);
}