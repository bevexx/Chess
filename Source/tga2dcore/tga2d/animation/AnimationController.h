#include <string>
#include <map>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/LogStream.hpp"
#include <fstream>
#include <tga2d/engine_defines.h>

#include <tga2d/animation/Math/MathFunc.h>

#define NUM_BONES_PER_VEREX 4

namespace Tga2D
{
	struct BoneInfoAnim
	{
		aiMatrix4x4 BoneOffset;
		aiMatrix4x4 FinalTransformation;
	};

	class AnimationController
	{
	public:
		struct SEvent
		{
			aiMatrix4x4 myNodeTransformation;
			bool isSet = false;
			bool isTriggerFrame = false;
			bool myWasTriggeredThisCycle = false;
		};
		struct SAnimEvent
		{
			std::map<std::string, SEvent> myEvents;

		};

		std::string m_ModelPath;
		aiVector3D _rotation;
		uint _prevAnimIndex = 0;
		std::vector<Assimp::Importer*> _importers;
		std::vector<const aiScene*> _scenes;
		uint _curScene;
		aiMatrix4x4 m_GlobalInverseTransform;
		std::map<std::string, uint> m_BoneMapping;
		std::vector<BoneInfoAnim> m_BoneInfo;
		std::vector<BoneInfoAnim> m_BoneInfoEvent;
		uint myNumBones;
		aiMatrix4x4** myTransforms[32];
		SAnimEvent* myEventFrames[32];
		unsigned int myTransformSize[32];
		float myTotalAnimTime[32];
		float _animationTime0[32];

		
		std::map<std::string, std::vector<std::function<void()>>> myEventCallbacks;
		void RegisterCallback(const std::string& aEvent, std::function<void()> aFunctionToCall)
		{
			myEventCallbacks[aEvent].push_back(aFunctionToCall);
		}





		void SetRotation(const aiVector3D& r) { _rotation = r; }
		aiVector3D GetRotation() { return _rotation; }

		~AnimationController()
		{
			Release();
		}

		void Release()
		{
			for (uint i = 0; i < _importers.size(); ++i)
			{
				delete _importers[i];
			}
			_importers.clear();

			for (int i=0; i< _scenes.size(); i++)
			{
				for (uint j = 0; j < myTransformSize[i]; j++)
				{
					delete[] myTransforms[i][j];
				}
				delete[]myTransforms[i];
			}

		
		}

		AnimationController(const char* modelpath)
			: myNumBones(0)
			, _curScene(NULL)
			, m_ModelPath(modelpath)
			, _rotation(0.f)
			, _prevAnimIndex(UINT_MAX)
		{
			if constexpr (ANIMATION_PRECISION > 4)
			{
				INFO_TIP("%s", "Animation precision is set very high, this will impact loadtimes and memory. Please consider lower as this impacts every animation ingame");
			}
		}

		void ReadAsEvent(std::string& aName, const aiScene* scene, float AnimationTime, const aiMatrix4x4& ParentTransform, aiNode* pNode, int stopAnimLevel)
		{
			pNode;
			ParentTransform;
			float time(AnimationTime);
			const aiNodeAnim* pNodeAnim = FindNodeAnim(scene->mAnimations[0], aName);
			aiMatrix4x4 NodeTransformation;
			if (pNodeAnim)
			{

				aiVector3D Translation;
				{
				//	float time2(stopAnimLevel <= 0 ? AnimationTime : 0.f);
					CalcPosition(Translation, AnimationTime, pNodeAnim);

				}
				aiMatrix4x4 TranslationM;
				aiMatrix4x4::Translation(Translation, TranslationM);

				// Combine the above transformations
				NodeTransformation = TranslationM;
			}
			stopAnimLevel--;


			SEvent& origin = myEventFrames[_curScene][0].myEvents[aName];
			if (time == 0 && !origin.isSet)
			{
				origin.isSet = true;
				myEventFrames[_curScene][(unsigned int)time].myEvents[aName].myNodeTransformation = NodeTransformation;
			}

			if (origin.myNodeTransformation != NodeTransformation)
			{
				myEventFrames[_curScene][(unsigned int)time].myEvents[aName].isTriggerFrame = true;
			}

			

		}

		void ReadNodeHeirarchyEvents(const aiScene* scene, float AnimationTime, aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel)
		{
			std::string NodeName(pNode->mName.data);
			aiMatrix4x4 NodeTransformation(pNode->mTransformation);

			std::size_t found = NodeName.find("event");
			bool ignoreChild = false;
			if (found != std::string::npos)
			{
				ReadAsEvent(NodeName, scene, AnimationTime, ParentTransform, pNode, stopAnimLevel);
				ignoreChild = true;

			}
			aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;
			for (uint i = 0; i < pNode->mNumChildren; i++)
			{
				ReadNodeHeirarchyEvents(scene, AnimationTime, pNode->mChildren[i], GlobalTransformation, stopAnimLevel);
			}
		}
		void ReadNodeHeirarchy(const aiScene* scene, float AnimationTime, aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel)
		{
			float time(AnimationTime);
			std::string NodeName(pNode->mName.data);
			aiMatrix4x4 NodeTransformation(pNode->mTransformation);
			const aiNodeAnim* pNodeAnim = FindNodeAnim(scene->mAnimations[0], NodeName);
			if (pNodeAnim)
			{
				// Interpolate scaling and generate scaling transformation matrix
				aiVector3D Scaling;
				CalcInterpolatedScaling(Scaling, time, pNodeAnim);
				aiMatrix4x4 ScalingM;
				aiMatrix4x4::Scaling(Scaling, ScalingM);

				// Interpolate rotation and generate rotation transformation matrix
				aiQuaternion RotationQ;
				CalcInterpolatedRotation(RotationQ, time, pNodeAnim);
				aiMatrix4x4 RotationM;
				InitM4FromM3(RotationM, RotationQ.GetMatrix());

				// Interpolate translation and generate translation transformation matrix
				aiVector3D Translation;
				{
					float time2(stopAnimLevel <= 0 ? AnimationTime : 0.f);
					CalcInterpolatedPosition(Translation, time2, pNodeAnim);

				}
				aiMatrix4x4 TranslationM;
				aiMatrix4x4::Translation(Translation, TranslationM);

				// Combine the above transformations
				NodeTransformation = TranslationM * RotationM * ScalingM;
			}
			stopAnimLevel--;

			aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;
			if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
			{
				uint BoneIndex = m_BoneMapping[NodeName];
				m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
			}

			for (uint i = 0; i < pNode->mNumChildren; i++)
			{
				ReadNodeHeirarchy(scene, AnimationTime, pNode->mChildren[i], GlobalTransformation, stopAnimLevel);
			}

		}


		void ResetEvents(int aMaxTime)
		{
			for (int i=0; i< aMaxTime; i++)
			{
				for (auto& it : myEventFrames[_curScene][i].myEvents)
				{
					it.second.myWasTriggeredThisCycle = false;
				}
			}

		}
		void UpdateEvents(float aAtWhatTime)
		{
			for (auto &it : myEventFrames[_curScene][(int)aAtWhatTime].myEvents)
			{
				if (it.second.isTriggerFrame && !it.second.myWasTriggeredThisCycle)
				{
					it.second.myWasTriggeredThisCycle = true;

					for (auto& listener : myEventCallbacks[it.first])
					{
						listener();
					}
			
				}
			}
		}

		
		void BoneTransform(Matrix44*& Transforms)
		{
			aiMatrix4x4 Identity;
			InitIdentityM4(Identity);

			float TicksPerSecond = (float)_scenes[_curScene]->mAnimations[0]->mTicksPerSecond != 0 ? (float)_scenes[_curScene]->mAnimations[0]->mTicksPerSecond : 25.0f;
			float TimeInTicks = _animationTime0[_curScene] * TicksPerSecond;

			float AnimationTime = fmod(TimeInTicks, (float)_scenes[_curScene]->mAnimations[0]->mDuration);
			UpdateEvents(AnimationTime);
			AnimationTime *= ANIMATION_PRECISION;
			AnimationTime = min(AnimationTime, myTransformSize[_curScene] -1);
			uint animTimeInt = static_cast<uint>(AnimationTime);
			assert(animTimeInt <= myTransformSize[_curScene]);
			animTimeInt;
	

			memcpy(&Transforms[0], &myTransforms[_curScene][static_cast<int>(AnimationTime)][0], sizeof(aiMatrix4x4) * myNumBones);


		}

		void LoadBones( const aiMesh* pMesh)
		{
			for (uint i = 0; i < pMesh->mNumBones; i++) {
				uint BoneIndex = 0;
				std::string BoneName(pMesh->mBones[i]->mName.data);

				if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
					BoneIndex = myNumBones;
					myNumBones++;
					BoneInfoAnim bi;
					m_BoneInfo.push_back(bi);
				}
				else {
					BoneIndex = m_BoneMapping[BoneName];
				}

				m_BoneMapping[BoneName] = BoneIndex;
				m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;

			}

		}

		void BakeAnimation()
		{
			aiMatrix4x4 Identity;
			InitIdentityM4(Identity);
			float duration = (float)_scenes[_curScene]->mAnimations[0]->mDuration * ANIMATION_PRECISION;
			myTransforms[_curScene] = new aiMatrix4x4*[(int)duration];

		
			myTransformSize[_curScene] = (int)duration;
			for (int i = 0; i < (int)duration; i++)
			{
				myTransforms[_curScene][i] = new aiMatrix4x4[myNumBones];
			}
			for (int i = 0; i < duration; i++)
			{
				float currentTime = (((float)i / duration) * duration) / ANIMATION_PRECISION;
				ReadNodeHeirarchy(_scenes[_curScene], currentTime, _scenes[_curScene]->mRootNode, Identity, 2);

				for (uint k = 0; k < myNumBones; k++)
				{
					myTransforms[_curScene][i][k] = m_BoneInfo[k].FinalTransformation;
				}
			}

			float realDuration = (float)_scenes[_curScene]->mAnimations[0]->mDuration;
			myEventFrames[_curScene] = new SAnimEvent[(int)realDuration];
			for (int i = 0; i < realDuration; i++)
			{
				float currentTime = (((float)i / realDuration) * realDuration);
				ReadNodeHeirarchyEvents(_scenes[_curScene], currentTime, _scenes[_curScene]->mRootNode, Identity, 2);

			}

		}


		bool InitFromScene(const aiScene* pScene)
		{
			_animationTime0[_curScene] = 0.f;

			for (uint i = 0; i < pScene->mNumMeshes; ++i)
			{
				LoadBones(pScene->mMeshes[i]);
			}

			BakeAnimation();
			return true;
		}

		bool Import3DFromFile(const std::string& pFile = "")
		{
			if (pFile.length())
				m_ModelPath = pFile;

			// Check if file exists
			std::ifstream fin(m_ModelPath.c_str());
			if (!fin.fail())
			{
				fin.close();
			}
			else
			{
				MessageBoxA(NULL, ("Couldn't open file: " + m_ModelPath).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
				return false;
			}

			_curScene = static_cast<uint>(_importers.size());
			_importers.push_back(new Assimp::Importer);
			_scenes.push_back(_importers[_curScene]->ReadFile(m_ModelPath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));

			if (_scenes.back()->mNumAnimations == 0)
			{
				ERROR_PRINT("%s %s %s", "Animation rig:", pFile.c_str(), "failed as it has no animations, we need at lease one frame animated to get the data from maya");
				return false;
			}

			bool ret = false;
			// If the import failed, report it
			if (_scenes[_curScene]) 
			{
				m_GlobalInverseTransform = _scenes[_curScene]->mRootNode->mTransformation;
				m_GlobalInverseTransform.Inverse();
				ret = InitFromScene(_scenes[_curScene]);
				// Now we can access the file's contents.
				logInfo("Import of _curScene " + m_ModelPath + " succeeded.");
			}
			else {
				logInfo(_importers[_curScene]->GetErrorString());
			}

			// We're done. Everything will be cleaned up by the importer destructor
			return ret;
		}


		EModelStatus Update(float dt)
		{

			EModelStatus status = EModelStatus_Nothing;
			float TicksPerSecond = (float)_scenes[_curScene]->mAnimations[0]->mTicksPerSecond != 0 ? (float)_scenes[_curScene]->mAnimations[0]->mTicksPerSecond : 25.0f;
			myTotalAnimTime[_curScene] += dt * TicksPerSecond;
			float realDuration = (float)_scenes[_curScene]->mAnimations[0]->mDuration;
			if (myTotalAnimTime[_curScene] >= realDuration)
			{
				ResetEvents(static_cast<int>(realDuration));
				myTotalAnimTime[_curScene] = 0.0f;
				status = EModelStatus_Animation_End;
			}
			_animationTime0[_curScene] += dt ;	
			return status;
		}

		bool Add3DAnimFromFile(const std::string& fileName)
		{
			// Check if file exists
			std::ifstream fin(fileName.c_str());
			if (!fin.fail())
			{
				fin.close();
			}
			else
			{
				MessageBoxA(NULL, ("Couldn't open file: " + fileName).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
				return false;
			}

			_curScene = static_cast<uint>(_importers.size());
			myTotalAnimTime[_curScene] = 0.0f;
			_importers.push_back(new Assimp::Importer);
			_scenes.push_back(_importers[_curScene]->ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));
			// If the import failed, report it
			if (_scenes[_curScene]) 
			{
				logInfo(_importers[_curScene]->GetErrorString());
				m_GlobalInverseTransform = _scenes[_curScene]->mRootNode->mTransformation;
				m_GlobalInverseTransform.Inverse();
				return InitFromScene(_scenes[_curScene]);
			}
			return true;
		}

		bool SetAnimIndexNoBlend(uint index)
		{
			if (index == _curScene || index >= _scenes.size())
			{
				return false;
			}
			_prevAnimIndex = _curScene;
			_curScene = index;
			_animationTime0[_curScene] = 0;
			return true;

			
		}

		bool SetAnimIndex(uint index)
		{
			if (index == _curScene || index >= _scenes.size())
			{
				return false;
			}
			_prevAnimIndex = _curScene;
			_curScene = index;
			return true;
		}



		uint GetMaxIndex()
		{
			return static_cast<uint>(_scenes.size());
		}

	
	};
}
