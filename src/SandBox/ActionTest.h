#pragma once

class ActionTest_Idle : public Action<Entity>
{
public:
	void OnStart(Entity* pOwner) override;
	void OnUpdate(Entity* pOwner) override {}
	void OnEnd(Entity* pOwner) override {}
};

class ActionTest_Moving : public Action<Entity>
{
public:
	void OnStart(Entity* pOwner) override;
	void OnUpdate(Entity* pOwner) override {}
	void OnEnd(Entity* pOwner) override {}
};

class ActionTest_Reverse : public Action<Entity>
{
public:
	void OnStart(Entity* pOwner) override;
	void OnUpdate(Entity* pOwner) override {}
	void OnEnd(Entity* pOwner) override {}
};
