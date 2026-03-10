#include "pch.h"

namespace utils
{
	template <typename T>
	inline bool VectorContain(std::vector<T*> vector, T* element)
	{
		return std::find(vector.begin(), vector.end(), element) != vector.end();
	}
}