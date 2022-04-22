#include "stdafx.h"
#include "CAnimation.h"
#include <tga2d/animation/AnimationController.h>
#include <tga2d/engine.h>

using namespace Tga2D;
Tga2D::CAnimation::~CAnimation()
{
	SAFE_DELETE(controller);
}

void CAnimation::Init(const char* aRig, std::vector<std::string>& somePathsToAnimations)
{
	controller = new AnimationController(aRig);
	controller->Import3DFromFile(aRig);
	for (std::string s : somePathsToAnimations)
	{
		controller->Add3DAnimFromFile(s);
	}
	
}

void CAnimation::BoneTransform(Matrix44* Transforms)
{
	if (!controller)
	{
		return;
	}
	controller->BoneTransform(Transforms);
}

unsigned int Tga2D::CAnimation::GetNumberOfBones() const
{
	if (!controller)
	{
		return 0;
	}
	return controller->myNumBones;
}

EModelStatus CAnimation::Step(float aDelta)
{
	if (!controller)
	{
		return EModelStatus_Nothing;
	}
	return controller->Update(aDelta);
}

void Tga2D::CAnimation::SetCurrentAnimationIndex(unsigned short aIndex)
{
	if (!controller)
	{
		return;
	}
	controller->SetAnimIndexNoBlend(aIndex);
}

void Tga2D::CAnimation::SetCurrentAnimationTime(float aAnimationFrame)
{
	if (!controller)
	{
		return;
	}
	controller->_animationTime0[controller->_curScene] = aAnimationFrame;
}

void Tga2D::CAnimation::RegisterAnimationEventCallback(const std::string& aEvent, std::function<void()> aFunctionToCall)
{
	if (!controller)
	{
		return;
	}
	controller->RegisterCallback(aEvent, aFunctionToCall);
}
