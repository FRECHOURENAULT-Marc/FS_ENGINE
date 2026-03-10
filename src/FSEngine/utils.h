#pragma once

#define FLOAT_MAX 3.402823466e+38F

namespace utils
{
	template <typename T>
	bool VectorContain(std::vector<T*> vector, T* element);
}

#include "utils.inl"