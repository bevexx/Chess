#pragma once
#include <vector>
#include <string>
#include <tga2d/model/ModelStatus.h>
namespace Tga2D
{

	class CAnimation
	{
	public:
		~CAnimation();
		void Init(const char* aRig, std::vector<std::string>& somePathsToAnimations);

		void BoneTransform(Matrix44* Transforms);
		unsigned int GetNumberOfBones() const;
		EModelStatus Step(float aDelta);

		void SetCurrentAnimationIndex(unsigned short aIndex);
		void SetCurrentAnimationTime(float aAnimationTime);
		void RegisterAnimationEventCallback(const std::string& aEvent, std::function<void()> aFunctionToCall);

		class SceneAnimator* myAnimator;
		class SceneAnimator* myBindPose = nullptr;
		std::vector<int> myActiveAnimations;
		float myAnimTime = 0;
		int myAnimSpeed = 60;

		class AnimationController* controller;
	};
}

