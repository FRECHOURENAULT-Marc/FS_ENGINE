#pragma once

class Component : public Object
{
public:
	Component() : Object() {};

	friend class ECS;
};
