#pragma once

namespace FSE
{
	class ECS;

	class System
	{
	protected:
		ECS* m_Ecs = nullptr;

		System();
	public:

		virtual void Update(float dt) {}; //override this
		virtual void AfterUpdate(float dt) {}; //override this

		friend class ECS;
	};
}

