#pragma once
#include "Vector4.h"
#include "common_math.h"

namespace Tga2D
{
	class Matrix44
	{
	public:
		__forceinline Matrix44()
		{
			myMatrix[0][0] = 1.0f;
			myMatrix[0][1]  = 0.0f;
			myMatrix[0][2]  = 0.0f;
			myMatrix[0][3]  = 0.0f;

			myMatrix[1][0] = 0.0f;
			myMatrix[1][1] = 1.0f;
			myMatrix[1][2] = 0.0f;
			myMatrix[1][3] = 0.0f;

			myMatrix[2][0] = 0.0f;
			myMatrix[2][1] = 0.0f;
			myMatrix[2][2] = 1.0f;
			myMatrix[2][3] = 0.0f;

			myMatrix[3][0] = 0.0f;
			myMatrix[3][1] = 0.0f;
			myMatrix[3][2] = 0.0f;
			myMatrix[3][3] = 1.0f;
		}
		__forceinline void Transpose();

		union
		{
			float myMatrix[4][4];
			float myMatrixSingle[16];
		};
	};



	void Matrix44::Transpose()
	{
		Matrix44 newMatrix;
		newMatrix.myMatrix[0][0] = myMatrix[0][0];
		newMatrix.myMatrix[1][0] = myMatrix[0][1];
		newMatrix.myMatrix[2][0] = myMatrix[0][2];
		newMatrix.myMatrix[3][0] = myMatrix[0][3];
		newMatrix.myMatrix[0][1] = myMatrix[1][0];
		newMatrix.myMatrix[1][1] = myMatrix[1][1];
		newMatrix.myMatrix[2][1] = myMatrix[1][2];
		newMatrix.myMatrix[3][1] = myMatrix[1][3];
		newMatrix.myMatrix[0][2] = myMatrix[2][0];
		newMatrix.myMatrix[1][2] = myMatrix[2][1];
		newMatrix.myMatrix[2][2] = myMatrix[2][2];
		newMatrix.myMatrix[3][2] = myMatrix[2][3];
		newMatrix.myMatrix[0][3] = myMatrix[3][0];
		newMatrix.myMatrix[1][3] = myMatrix[3][1];
		newMatrix.myMatrix[2][3] = myMatrix[3][2];
		newMatrix.myMatrix[3][3] = myMatrix[3][3];

		myMatrix[0][0] = newMatrix.myMatrix[0][0];
		myMatrix[1][0] = newMatrix.myMatrix[1][0];
		myMatrix[2][0] = newMatrix.myMatrix[2][0];
		myMatrix[3][0] = newMatrix.myMatrix[3][0];
		myMatrix[0][1] = newMatrix.myMatrix[0][1];
		myMatrix[1][1] = newMatrix.myMatrix[1][1];
		myMatrix[2][1] = newMatrix.myMatrix[2][1];
		myMatrix[3][1] = newMatrix.myMatrix[3][1];
		myMatrix[0][2] = newMatrix.myMatrix[0][2];
		myMatrix[1][2] = newMatrix.myMatrix[1][2];
		myMatrix[2][2] = newMatrix.myMatrix[2][2];
		myMatrix[3][2] = newMatrix.myMatrix[3][2];
		myMatrix[0][3] = newMatrix.myMatrix[0][3];
		myMatrix[1][3] = newMatrix.myMatrix[1][3];
		myMatrix[2][3] = newMatrix.myMatrix[2][3];
		myMatrix[3][3] = newMatrix.myMatrix[3][3];
	}



}
