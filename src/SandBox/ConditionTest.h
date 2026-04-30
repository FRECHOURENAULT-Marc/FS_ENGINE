#pragma once

using namespace FSC;
using namespace FSE;

class ConditionTest_SpacePressed : public Condition<Entity>
{
public:
	bool OnTest(Entity* pOwner) override;
};

class ConditionTest_EnterPressed : public Condition<Entity>
{
public:
	bool OnTest(Entity* pOwner) override;
};
