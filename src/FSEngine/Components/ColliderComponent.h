#pragma once

namespace FSE
{

	class ColliderComponent : public Component
	{
		virtual void Reset() override;

		std::string m_Tag = "";

		std::vector<ColliderComponent*> m_CollidingWith = std::vector<ColliderComponent*>();
		Collider* m_Collider = nullptr;

		bool IsColliding(ColliderComponent* other);
		void AddColliding(ColliderComponent* other) { m_CollidingWith.push_back(other); }
		void ResetColliding() { m_CollidingWith.clear(); }
		void AddToDefaultLayer();
		void EnsureTransform();
		void Update(); 

	public:
		ColliderComponent() : Component() {};

		Collider* GetCollider() { return m_Collider; };
		const std::vector<ColliderComponent*>& GetColliding() { return m_CollidingWith; }
		std::string GetTag() { return m_Tag; }
		ColliderComponent* IsCollidingWith(int otherEntityID);
		ColliderComponent* IsCollidingWith(ColliderComponent* other);
		ColliderComponent* IsCollidingWith(std::string tag);

		void AddToLayer(std::string _layerName);
		void SetTag(std::string tag) { m_Tag = tag; }
		template<typename C>
		C* SetType();

		friend class ECS;
		friend class ColliderSystem;
	};

}

#include "ColliderComponent.inl"


