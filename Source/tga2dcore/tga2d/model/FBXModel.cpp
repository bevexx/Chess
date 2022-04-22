#include "stdafx.h"
#include "FBXModel.h"
#include <tga2d/loaders/FBXLoader.h>
#include <tga2d/animation/CAnimation.h>
#include <tga2d/drawers/debug_drawer.h>

using namespace Tga2D;
FBXModel::FBXModel()
{
	myPosition = {0, 0};
	myRotation = { 0 };
	myScale = { 0.1f, 0.1f };
}



FBXModel::~FBXModel()
{
	SAFE_DELETE(myMesh);
	SAFE_DELETE(myAnimation);
}

bool FBXModel::Init(const std::string& aModelPath)
{
	myModelPath = aModelPath;
	CFBXLoader loader;
	myMesh = loader.LoadModel(aModelPath.c_str(), nullptr);
	
	return false;
}

bool FBXModel::InitAnimations(std::vector<std::string>& someAnimationPaths)
{
	if (!myAnimation)
	{
		myAnimation = new CAnimation();	
	}

	myAnimation->Init(myModelPath.c_str(), someAnimationPaths);
	return true;
}

void Tga2D::FBXModel::SetCurrentAnimationIndex(unsigned short aIndex)
{
	if (!myAnimation)
	{
		ERROR_PRINT("%s", "SetCurrentAnimationIndex: no animation is loaded" );
	}
	myAnimation->SetCurrentAnimationIndex(aIndex);
}

void Tga2D::FBXModel::SetCurrentAnimationTime(float aAnimationFrame)
{
	if (!myAnimation)
	{
		ERROR_PRINT("%s", "SetCurrentAnimationIndex: no animation is loaded");
	}
	myAnimation->SetCurrentAnimationTime(aAnimationFrame);
}


EModelStatus FBXModel::Update(float aDelta)
{
	EModelStatus status = EModelStatus_Nothing;
	if (myAnimation)
	{
		memset(myBones, 0, sizeof(myBones));
		status = myAnimation->Step(aDelta );
		myAnimation->BoneTransform(myBones);

	}
	return status;
}

void Tga2D::FBXModel::RegisterAnimationEventCallback(const std::string& aEvent, std::function<void()> aFunctionToCall)
{
	myAnimation->RegisterAnimationEventCallback(aEvent, aFunctionToCall);
}

void FBXModel::Render()
{
	myMesh->Render(myBones, myPosition, myRotation, myScale, myIsXFlipped);
}

void FBXModel::SetMap(const EShaderMap aMapType, const char* aPath)
{
	if (myMesh == nullptr)
	{
		ERROR_PRINT("%s", "FBXModel::SetMap: Tried to set map before model is initiated. Run Init first!");
	}
	myMesh->SetMap(aMapType, aPath);
}
