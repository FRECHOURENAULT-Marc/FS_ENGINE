#pragma once

class ColliderComponent : public Component
{
	virtual void Reset() override;

	FS_Collider* mCollider = nullptr;
	//TransformComponent* mTrs = nullptr; // To avoid multiple Get

	std::vector<ColliderComponent*> mCollidingWith = std::vector<ColliderComponent*>();
	bool IsColliding(ColliderComponent* other);
	void AddColliding(ColliderComponent* other) { mCollidingWith.push_back(other); }
	void ResetColliding() { mCollidingWith.clear(); }

	void EnsureTransform();
	void Update();

public:
	ColliderComponent() : Component() {};

	FS_Collider* GetCollider() { return mCollider; };
	bool IsCollidingWith(int other);
	bool IsCollidingWith(ColliderComponent* other);


	template<typename C>
	C* SetType();

	friend class ECS;
	friend class ColliderSystem;
};

#include "ColliderComponent.inl"


