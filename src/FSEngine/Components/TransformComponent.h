#pragma once

class TransformComponent : public Component
{
private:

	Transform localTransform;
	XMFLOAT4X4 worldMatrix;

	std::vector<int> m_children;
	int m_parent = -1;

	bool m_dirty = true;
	bool m_isUpdatedThisFrame = true;

	void MarkDirty();
	void UpdateChildrenDirty();
	void UpdateWorldMatrix();

public:

	void Reset() override;
	TransformComponent();

	Transform& GetLocalTransform();

	XMFLOAT3 GetLocalPosition();
	XMFLOAT3 GetWorldPosition();

	XMFLOAT3 GetScale() { return localTransform.sca; }

	XMFLOAT3 GetRotation();
	XMFLOAT3 GetForward();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	XMFLOAT4X4 GetLocalMatrix();
	XMFLOAT4X4 GetWorldMatrix();

	bool IsUpdatedThisFrame() { return m_isUpdatedThisFrame; }
	void SetUpdatedThisFrame(bool updated) { m_isUpdatedThisFrame = updated; }

	void SetLocalPosition(XMFLOAT3 pos);
	void SetWorldPosition(XMFLOAT3 pos);

	void Move(float distance);
	void TranslateWorld(XMFLOAT3 delta);
	void TranslateLocal(XMFLOAT3 delta);

	void SetScale(float s);

	void RotateWorld(float yaw, float pitch, float roll);
	void RotateLocal(float yaw, float pitch, float roll);
	void SetRotation(float yaw, float pitch, float roll);
	void SetForward(XMFLOAT3 forward);
	void ResetRotation();

	void AddChild(TransformComponent* child);
	void RemoveChild(TransformComponent* child);

	TransformComponent* GetParent();

	friend class ECS;
	friend class Entity;
};

