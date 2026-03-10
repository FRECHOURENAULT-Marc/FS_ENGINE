#pragma once

class ECS;

class System
{
protected:
	ECS* m_ecs = nullptr;

	System();
public:

	virtual void Update(float dt) {}; //override this

	friend class ECS;
};