#pragma once

enum class COLLIDER_TYPE
{
	NONE,
	SPHERE,
	RAY
};
class FS_Collider
{
protected:
	XMFLOAT3 mPos = { 0, 0, 0 };
	float mScale = 1.0f;
	COLLIDER_TYPE mType = COLLIDER_TYPE::NONE;

public:

	FS_Collider() {};
	virtual bool IsColliding(FS_Collider* other) { return false; }; //override this
	XMFLOAT3 GetPosition() { return mPos; };
	float GetScale() { return mScale; };
	COLLIDER_TYPE GetColliderType() { return mType; };

	void SetPosition(XMFLOAT3 pos) { mPos = pos; };
	void SetScale(float scale) { mScale = scale; };
	void SetType(COLLIDER_TYPE type) { mType = type; };

	static bool IsCollidingSphereSphere(FS_Collider* sphere1, FS_Collider* sphere2);
};

class FS_SphereCollider : public FS_Collider
{
protected:
	float mRadius = 0.0f;
public:
	FS_SphereCollider();

	virtual bool IsColliding(FS_Collider* other) override;
	float GetRadius() { return mRadius; };

	void SetRadius(float radius) { mRadius = radius; };

};

