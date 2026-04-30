#include "pch.h"

namespace FSE
{
	template<typename S>
	inline S* SceneManager::CreateScene()
	{
		S* scene = new S();
		m_newScenes.push_back(scene);
		return scene;
	}
}

