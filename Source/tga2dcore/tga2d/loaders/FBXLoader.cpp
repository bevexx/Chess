#include "stdafx.h"
#include "FBXLoader.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/metadata.h>

#include <map>
#include <string>
#include <future>
#include <windows.h>
#include <fstream>
#include <tga2d/math/matrix44.h>
#include <tga2d/primitives/model_shape.h>
#include <tga2d/texture/texture.h>
#include <tga2d/texture/texture_manager.h>

#include VECTORFILE

//#pragma comment (lib, "..\\Libs\\assimp_d.lib")
#pragma comment (lib, "assimp-vc140-mt.lib")

#define TEXTURE_SET_0 0
#define TEXTURE_SET_1 1
#define TEXTURE_SET_2 2
#define TEXTURE_SET_3 3

#define VERTEX_COLOR_SET_0 0
#define VERTEX_COLOR_SET_1 1
#define VERTEX_COLOR_SET_2 2
#define VERTEX_COLOR_SET_3 3

using namespace Tga2D;
void LoadMaterials(const struct aiScene* sc, CMeshNode* aModel, std::string aModelPath);
CFBXLoader::CFBXLoader()
{
}


CFBXLoader::~CFBXLoader()
{
}

Matrix44 ConvertToEngineMatrix33(const aiMatrix3x3& AssimpMatrix)
{

	Matrix44 mat;
	mat.myMatrix[0][0] = AssimpMatrix.a1;	mat.myMatrix[0][1] = AssimpMatrix.a2;	mat.myMatrix[0][2] = AssimpMatrix.a3;	mat.myMatrix[0][3] = 0.0f;
	mat.myMatrix[1][0] = AssimpMatrix.b1;	mat.myMatrix[1][1] = AssimpMatrix.b2;	mat.myMatrix[1][2] = AssimpMatrix.b3;	mat.myMatrix[1][3] = 0.0f;
	mat.myMatrix[2][0] = AssimpMatrix.c1;	mat.myMatrix[2][1] = AssimpMatrix.c2;	mat.myMatrix[2][2] = AssimpMatrix.c3;	mat.myMatrix[2][3] = 0.0f;
	mat.myMatrix[3][0] = 0.0f;				mat.myMatrix[3][1] = 0.0f;				mat.myMatrix[3][2] = 0.0f;				mat.myMatrix[3][3] = 1.0f;
	return mat;
}

// constructor from Assimp matrix
Matrix44 ConvertToEngineMatrix44(const aiMatrix4x4& AssimpMatrix)
{
	Matrix44 mat;
	mat.myMatrix[0][0] = AssimpMatrix.a1; mat.myMatrix[0][1] = AssimpMatrix.a2; mat.myMatrix[0][2] = AssimpMatrix.a3; mat.myMatrix[0][3] = AssimpMatrix.a4;
	mat.myMatrix[1][0] = AssimpMatrix.b1; mat.myMatrix[1][1] = AssimpMatrix.b2; mat.myMatrix[1][2] = AssimpMatrix.b3; mat.myMatrix[1][3] = AssimpMatrix.b4;
	mat.myMatrix[2][0] = AssimpMatrix.c1; mat.myMatrix[2][1] = AssimpMatrix.c2; mat.myMatrix[2][2] = AssimpMatrix.c3; mat.myMatrix[2][3] = AssimpMatrix.c4;
	mat.myMatrix[3][0] = AssimpMatrix.d1; mat.myMatrix[3][1] = AssimpMatrix.d2; mat.myMatrix[3][2] = AssimpMatrix.d3; mat.myMatrix[3][3] = AssimpMatrix.d4;
	return mat;
}

int DetermineAndLoadVerticies(aiMesh* fbxMesh, CLoaderMesh* aLoaderMesh)
{
	if (!fbxMesh->HasTextureCoords(TEXTURE_SET_0))
	{
		assert("Error: No UV:s found");
		return 0;
	}

	int vertexBufferSize = 0;
	vertexBufferSize += (fbxMesh->HasPositions() ? sizeof(float) * 4 : 0);
	vertexBufferSize += (fbxMesh->HasTextureCoords(TEXTURE_SET_0) ? sizeof(float) * 2 : 0);
	vertexBufferSize += (fbxMesh->HasBones() ? sizeof(float) * 8 : 0);

	vertexBufferSize = 16 + 8 + 32;

	aLoaderMesh->myVertexBufferSize = vertexBufferSize;
	aLoaderMesh->myVerticies = new char[vertexBufferSize * fbxMesh->mNumVertices];
	aLoaderMesh->myVertexCount = fbxMesh->mNumVertices;

	std::vector<VertexBoneData> collectedBoneData;
	if (fbxMesh->HasBones())
	{
		collectedBoneData.resize(fbxMesh->mNumVertices);

		unsigned int BoneIndex = 0;
		aLoaderMesh->myModel = new CLoaderModel();
		for (unsigned int i = 0; i < fbxMesh->mNumBones; i++) 
		{
			
			std::string BoneName(fbxMesh->mBones[i]->mName.data);
			if (aLoaderMesh->myModel->m_BoneMapping.find(BoneName) == aLoaderMesh->myModel->m_BoneMapping.end())
			{
				// Allocate an index for a new bone
				BoneIndex = aLoaderMesh->myModel->m_NumBones;
				aLoaderMesh->myModel->m_NumBones++;
				BoneInfo bi;
				aLoaderMesh->myModel->m_BoneInfo.push_back(bi);


				Matrix44 NodeTransformation = ConvertToEngineMatrix44(fbxMesh->mBones[i]->mOffsetMatrix);

				aLoaderMesh->myModel->m_BoneInfo[BoneIndex].BoneOffset = NodeTransformation;
				aLoaderMesh->myModel->m_BoneMapping[BoneName] = BoneIndex;
			}
			else {
				BoneIndex = aLoaderMesh->myModel->m_BoneMapping[BoneName];
			}

			for (unsigned int j = 0; j < fbxMesh->mBones[i]->mNumWeights; j++) 
			{
				unsigned int VertexID = fbxMesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = fbxMesh->mBones[i]->mWeights[j].mWeight;
				collectedBoneData[VertexID].AddBoneData(BoneIndex, Weight);
			}
		}
	}
	

	SVertexCollection vertexCollection;
	for (unsigned int i = 0; i < fbxMesh->mNumVertices; i++)
	{
		if (fbxMesh->HasPositions())
		{
			aiVector3D& mVertice = fbxMesh->mVertices[i];
			vertexCollection.PushVec4(Vector4f(mVertice.x , mVertice.y , mVertice.z, 1));
		}
		else
		{
			ERROR_PRINT("%s", "Model must at least have vertex positions, please consult the artists about their export process");
		}

		if (fbxMesh->HasBones())
		{
			VertexBoneData& boneData = collectedBoneData[i];

			aiVector3D bones;
			vertexCollection.PushVec4(Vector4f((float)boneData.IDs[0], (float)boneData.IDs[1], (float)boneData.IDs[2], (float)boneData.IDs[3]));

			aiVector3D weights;
			vertexCollection.PushVec4(Vector4f(boneData.Weights[0], boneData.Weights[1], boneData.Weights[2], boneData.Weights[3]));
		}
		else
		{
			vertexCollection.PushVec4(Vector4f(0, 0, 0, 0));
			vertexCollection.PushVec4(Vector4f(0, 0, 0, 0));
		}

		if (fbxMesh->HasTextureCoords(TEXTURE_SET_0))
		{
			vertexCollection.PushVec2(Vector2f(fbxMesh->mTextureCoords[TEXTURE_SET_0][i].x, fbxMesh->mTextureCoords[TEXTURE_SET_0][i].y));
		}
		else
		{
			vertexCollection.PushVec2(Vector2f(0, 0));
		}

	}
	
	memcpy(aLoaderMesh->myVerticies, &vertexCollection.myData[0], (size_t)vertexBufferSize * (size_t)fbxMesh->mNumVertices);
	return vertexBufferSize;
}




void OnLoaded(void* someData)
{
	CLoaderModel* model = reinterpret_cast<CLoaderModel*>(someData);
	if (model)
	{
		model->myIsLoaded = true;
		if (model->myFuncToCallOnLoad)
		{
			model->myFuncToCallOnLoad(model, "");
		}
	}	
}

bool is_file_exist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}


EulerAngles ToEulerAngles(aiQuaternion q) 
{
	EulerAngles angles;

	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	angles.roll = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		angles.pitch = std::copysign(3.14f / 2, sinp); // use 90 degrees if out of range
	else
		angles.pitch = std::asin(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	angles.yaw = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

bool IsMetaInIgnoreList(aiString& aKey)
{
	static std::vector<aiString> ignoreList;
	if (ignoreList.size() == 0)
	{
		ignoreList.push_back(aiString("UserProperties"));
		ignoreList.push_back(aiString("IsNull"));
		ignoreList.push_back(aiString("RotationActive"));
		ignoreList.push_back(aiString("InheritType"));
		ignoreList.push_back(aiString("ScalingMax"));
		ignoreList.push_back(aiString("DefaultAttributeIndex"));
	}

	for (int i=0; i< ignoreList.size(); i++)
	{
		if (ignoreList[i] == aKey)
		{
			return true;
		}

	}
	return false;

}

void TraverseAndReadTree(aiNode* aNode, CMeshNode* aMeshTree, const aiScene* aScene)
{
	if (!aNode)
	{
		//We reached the end of the tree
		return;
	}

	aNode->mName;

	aiMatrix4x4 translation = aNode->mTransformation;

	aiQuaternion decomposedRotation;
	aiVector3D decomposedPosition;
	
	translation.DecomposeNoScaling(decomposedRotation, decomposedPosition);

	Tga2D::Vector2f offsetPosition = { decomposedPosition.x, decomposedPosition.y };
	EulerAngles eu = ToEulerAngles(decomposedRotation);

	for (UINT i=0; i< aNode->mNumMeshes; i++)
	{
		CModelShape* shape = aMeshTree->CreateNewShape();
		shape->myOffsetPosition = offsetPosition ;
		shape->myOffsetRotation = eu;
		shape->myName = aNode->mName.C_Str(); 

		int meshIndex = aNode->mMeshes[i];
		CLoaderMesh loaderMesh;
		DetermineAndLoadVerticies(aScene->mMeshes[meshIndex], &loaderMesh);
		for (unsigned int k = 0; k < aScene->mMeshes[meshIndex]->mNumFaces; k++)
		{
			for (unsigned int j = 0; j < aScene->mMeshes[meshIndex]->mFaces[k].mNumIndices; j++)
			{
				loaderMesh.myIndexes.push_back(aScene->mMeshes[meshIndex]->mFaces[k].mIndices[j]);
			}
		}
		shape->Init(&loaderMesh);
	}



	if (aNode->mMetaData)
	{
		for (UINT i=0; i< aNode->mMetaData->mNumProperties; i++)
		{
			aiString& key = aNode->mMetaData->mKeys[i];
			if (IsMetaInIgnoreList(key))
			{
				continue;
			}
			aiMetadataEntry& entry = aNode->mMetaData->mValues[i];
			metadataEntry copyFromAI;
			copyFromAI.mType = (metadataType)entry.mType;
			copyFromAI.mData = entry.mData;
			aMeshTree->GetRoot()->AddMetaData(std::string(key.C_Str()), copyFromAI);
	
		}
	}

	for (UINT i=0; i< aNode->mNumChildren; i++)
	{
		CMeshNode* child = aMeshTree->CreateNewChild();
		TraverseAndReadTree(aNode->mChildren[i], child, aScene);
	}
}

void VerifyFBXAndShowError(const aiScene* aScene, const char* aModelPath)
{
	if (aScene->mNumMaterials > 1 || aScene->mNumTextures > 1)
	{
		ERROR_PRINT("%s %s", "FBX has more than one texture! Only one texture is supported! ", aModelPath);
	}

	if (aScene->mNumMeshes > 1)
	{
		ERROR_PRINT("%s %s", "FBX has more than one mesh! Only one mesh is supported! ", aModelPath);
	}

	bool hasBones = false;
	for (unsigned int meshIndex = 0; meshIndex < aScene->mNumMeshes; meshIndex++)
	{
		if (aScene->mMeshes[meshIndex]->HasBones() == true)
		{
			hasBones = true;
			break;
		}
	}
	if (hasBones == false)
	{
		ERROR_PRINT("%s %s", "This model has no bones! It won't render properly! ", aModelPath);
	}
}


CMeshNode* LoadModelInternal(void* someInput)
{
	CLoaderModel* model = reinterpret_cast<CLoaderModel*>(someInput);
	const aiScene* scene = NULL;

	if (!is_file_exist(model->myModelPath.c_str()))
	{
		OutputDebugStringA("File not found");
	}

	aiPropertyStore* store = aiCreatePropertyStore();
	scene = aiImportFileExWithProperties(model->myModelPath.c_str(), aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded, nullptr, store);
	if (!scene)
	{
		std::string error = std::string(aiGetErrorString());
		if (model->myFuncToCallOnLoad)
		{
			model->myFuncToCallOnLoad(nullptr, error);
		}
		
		if (error.find("Tokensize") == std::string::npos)
		{
			error += " :File error, Basic layout is missing, are the UV Coordinates present?";
		}
		if (model->myFuncToCallOnLoad)
		{
			model->myFuncToCallOnLoad(nullptr, error);
		}

		return nullptr;
	}

	CMeshNode* meshTree = new CMeshNode();
	aiNode* node = scene->mRootNode;
	if (node)
	{
		TraverseAndReadTree(node, meshTree, scene);
	}
	
	if (!scene)
	{
		std::string error = std::string(aiGetErrorString());
		if (error.find("Tokensize") == std::string::npos)
		{
			error += " :FBX 2016/2017 not supported. Export with 2014/2015 instead.";
		}
		if (model->myFuncToCallOnLoad)
		{
			model->myFuncToCallOnLoad(nullptr, error);
		}
		OutputDebugStringA(aiGetErrorString());
		return nullptr;
	}
	LoadMaterials(scene, meshTree, model->myModelPath);

	VerifyFBXAndShowError(scene, model->myModelPath.c_str());
	
	return meshTree;
}



void LoadTexture(int aType, CTexture*& aTex, aiMaterial* aMaterial, std::string aModelPath)
{

	int texIndex = 0;
	aiReturn texFound = AI_SUCCESS;

	aiString path;	// filename

	unsigned int uvSet = 0;
	texFound = aMaterial->GetTexture((aiTextureType)aType, texIndex, &path, nullptr, &uvSet);
	if (texFound == AI_FAILURE)
	{
		return;
	}

	std::string filePath = std::string(path.data);

	const size_t last_slash_idx = filePath.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		filePath.erase(0, last_slash_idx + 1);
	}

	std::string combinedPath = aModelPath;
	combinedPath = combinedPath.substr(0, combinedPath.find_last_of("\\/"));
	combinedPath += "/" + filePath;
	aTex = Tga2D::CEngine::GetInstance()->GetTextureManager().GetTexture(combinedPath.c_str());
}

void LoadMaterials(const struct aiScene* sc, CMeshNode* aModel, std::string aModelPath)
{
	if (aModel->myTextures.size() > 0)
	{
		return;
	}
	int materialCount = sc->mNumMaterials;
	if (sc->mNumMaterials > 8)
	{
		std::string errorStr = "Sorry, only 8 different materials supported";
		errorStr += ", you have: " + std::to_string(materialCount);
		//CEngine::GetInstance()->GetScene()->SetErrorString(errorStr, 0);
		materialCount = 8;
	}

	for (int i = 0; i < materialCount; i++)
	{
		aModel->myTextures.push_back(Tga2D::CEngine::GetInstance()->GetTextureManager().GetWhiteSquareTexture());
	}

	for (int m = 0; m < materialCount; m++)
	{
		LoadTexture(aiTextureType_DIFFUSE, aModel->myTextures[m], sc->mMaterials[m], aModelPath); // TEXTURE_DEFINITION_ALBEDO
		//LoadTexture(aiTextureType_SPECULAR, aModel->myTextures[m], sc->mMaterials[m]); // TEXTURE_DEFINITION_ROUGHNESS
		//LoadTexture(aiTextureType_AMBIENT, aModel->myTextures[m], sc->mMaterials[m]); // TEXTURE_DEFINITION_AMBIENTOCCLUSION
		//LoadTexture(aiTextureType_EMISSIVE, aModel->myTextures[m], sc->mMaterials[m]); // TEXTURE_DEFINITION_EMISSIVE
		//LoadTexture(aiTextureType_HEIGHT, aModel->myTextures[m], sc->mMaterials[m]);
		//LoadTexture(aiTextureType_NORMALS, aModel->myTextures[m], sc->mMaterials[m]); // TEXTURE_DEFINITION_NORMAL
		//LoadTexture(aiTextureType_SHININESS, aModel->myTextures[m], sc->mMaterials[m]);
		//LoadTexture(aiTextureType_OPACITY, aModel->myTextures[m], sc->mMaterials[m]);
		//LoadTexture(aiTextureType_DISPLACEMENT, aModel->myTextures[m], sc->mMaterials[m]);
		//LoadTexture(aiTextureType_LIGHTMAP, aModel->myTextures[m], sc->mMaterials[m]);
		//LoadTexture(aiTextureType_REFLECTION, aModel->myTextures[m], sc->mMaterials[m]); // TEXTURE_DEFINITION_METALNESS
	}
}


CMeshNode* CFBXLoader::LoadModel(std::string aPath, FUNCTION_ON_LOAD aFunc, unsigned int aExtraShaderType)
{
	CLoaderModel* newModel = new CLoaderModel();
	newModel->myFuncToCallOnLoad = aFunc;
	newModel->SetData(aPath.c_str());
	newModel->myModelPath = aPath;
	newModel->myTexturePath = aPath;
	newModel->myExtraShaderType = aExtraShaderType;
	
	return LoadModelInternal((void*)newModel);
}

Tga2D::CMeshNode::~CMeshNode()
{
	for (CMeshNode* child : myChildren)
	{
		delete child;
	}
	for (CModelShape* shape : myShapes)
	{
		delete shape;
	}
	myChildren.clear();
	myShapes.clear();
}

void Tga2D::CMeshNode::Render(Matrix44* someBones, VECTOR2F aPosition, float  aRotation, VECTOR2F aScale, bool aIsFlipped)
{
	RenderInt(myTextures, someBones, aPosition, aRotation, aScale, aIsFlipped);

}

void Tga2D::CMeshNode::RenderInt(std::vector<class CTexture*>& someTextures, Matrix44* someBones, VECTOR2F aPosition, float  aRotation, VECTOR2F aScale, bool aIsFlipped)
{
	for (CMeshNode* child : myChildren)
	{
		child->RenderInt(someTextures, someBones, aPosition, aRotation, aScale, aIsFlipped);
	}
	for (CModelShape* shape : myShapes)
	{
		shape->Render(someTextures, someBones, aPosition, aRotation, aScale, aIsFlipped);
	}
}

void Tga2D::CMeshNode::SetMap(const EShaderMap aMapType, const char* aPath)
{
	(aMapType);
	CTexture* texture = CEngine::GetInstance()->GetTextureManager().GetTexture(aPath);
	if (texture->myIsFailedTexture == false)
	{
		myTextures.push_back(texture);
	}
}

CMeshNode* Tga2D::CMeshNode::CreateNewChild()
{
	myChildren.push_back(new CMeshNode());
	myChildren.back()->myParent = this;
	return myChildren.back();
}

CModelShape* Tga2D::CMeshNode::CreateNewShape()
{
	myShapes.push_back(new CModelShape());
	return myShapes.back();
}

CMeshNode* Tga2D::CMeshNode::GetRoot()
{
	if (myParent != nullptr)
	{
		return myParent->GetRoot();
	}
	return this;
}
