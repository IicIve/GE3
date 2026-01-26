#pragma once
#include "Vector.h"
#include "Matrix.h"

using namespace Vector;
using namespace Matrix;

Matrix4x4 MakeIdentity4x4();

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

