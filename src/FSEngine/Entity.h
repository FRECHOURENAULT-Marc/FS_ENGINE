#pragma once

class Entity : public Object
{
	void Reset() override {};

public:
	Entity();

	void AddChild(int id);
	void RemoveChild(int id);

	friend class ECS;
};

