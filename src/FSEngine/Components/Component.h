#pragma once

namespace FSE
{

	class Component : public Object
	{
	public:
		Component() : Object() {};

		friend class ECS;
	};

}


