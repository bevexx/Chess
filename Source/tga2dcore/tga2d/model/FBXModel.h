#pragma once
#include <tga2d/math/matrix44.h>
#include <tga2d/engine_defines.h>
#include <tga2d/model/ModelStatus.h>

namespace Tga2D
{
	class FBXModel
	{
	public:
		FBXModel();
		~FBXModel();
		bool Init(const std::string& aModelPath);
		bool InitAnimations(std::vector<std::string>& aAnimationPath);
		void SetCurrentAnimationIndex(unsigned short aIndex);
		void SetFlipX(bool aSetFlipped) {myIsXFlipped = aSetFlipped;}
		void SetCurrentAnimationTime(float aAnimationFrame);
		EModelStatus Update(float aDelta);
		void RegisterAnimationEventCallback(const std::string& aEvent, std::function<void()> aFunctionToCall);
		void Render();

		void SetPosition(const VECTOR2F& aPosition) { myPosition = aPosition; }
		const VECTOR2F& GetPosition() const { return myPosition; }

		/* Set a rotation in radians*/
		void SetRotation(const float aRotationInRadians) { myRotation = aRotationInRadians; }
		float GetRotation() const {return myRotation;}

		void SetScale(const VECTOR2F aScale) { myScale = aScale; }
		VECTOR2F GetScale() const { return myScale; }

		void SetMap(const EShaderMap aMapType, const char* aPath);
	private:
		VECTOR2F myPosition;
		float myRotation;
		VECTOR2F myScale;
		bool myIsXFlipped = false;

		class CMeshNode* myMesh = nullptr;
		class CAnimation* myAnimation = nullptr;
		Matrix44 myBones[MAX_ANIMATION_BONES];
		Matrix44 myBonesDE[MAX_ANIMATION_BONES];
		std::string myModelPath;
		float test = 0;
	};

}