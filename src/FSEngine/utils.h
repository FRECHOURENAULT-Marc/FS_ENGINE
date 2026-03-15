#pragma once

#define FLOAT_MAX 3.402823466e+38F

namespace utils
{
	template <typename T>
	bool VectorContain(std::vector<T*> vector, T* element);

	bool MatrixDecompose(XMFLOAT4X4 matrix, XMFLOAT3* _scale, XMFLOAT3* _rotation, XMFLOAT3* _pos);
	bool MatrixDecomposeRot(XMFLOAT4X4 matrix, XMFLOAT3* _rotation);
}

#include "utils.inl"