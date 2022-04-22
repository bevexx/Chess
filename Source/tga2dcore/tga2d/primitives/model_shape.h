#pragma once
#include "tga2d/render/render_object.h"
#include <tga2d/engine_defines.h>
namespace Tga2D
{

	class CModelShape : public CRenderObject
	{
	public:
		CModelShape();
		~CModelShape();

		bool Init(class CLoaderMesh* aModel);
		void Render(std::vector<class CTexture*>& myTextures, Matrix44* someBones, VECTOR2F aPosition, float  aRotation, VECTOR2F aScale, bool aIsFlipped);

		void Draw(CDirectEngine* aRenderer) override;

		struct ID3D11Buffer* myVertexBuffer = nullptr;
		struct ID3D11Buffer* myIndexBuffer = nullptr;


		unsigned int myIndexCount;
		unsigned int myVertexStride;

		class CShaderModel* myShader = nullptr;
		Vector2f myOffsetPosition;
		EulerAngles myOffsetRotation;
		std::string myName;
		class CTexture* myTexture = nullptr;
		struct SFrameByFrameData
		{
			Matrix44* myFrameByFrameBones = nullptr;
			VECTOR2F myPosition;
			float myRotation;
			VECTOR2F myScale;
			bool myIsFlippedX = false;
		};
		SFrameByFrameData myFrameByFrameData;
		
		
	};

}