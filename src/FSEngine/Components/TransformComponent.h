#pragma once

namespace FSE
{
	class TransformComponent : public Component
	{
	private:

		Transform m_LocalTransform;
		XMFLOAT4X4 m_WorldMatrix;

		std::vector<int> m_Children;
		int m_ParentID = -1;

		bool m_IsDirty = true;
		bool m_IsUpdatedThisFrame = true;

		void MarkDirty();
		void UpdateChildrenDirty();
		void UpdateWorldMatrix();

	public:

		void Reset() override;
		TransformComponent();

		Transform& GetLocalTransform();

		XMFLOAT3 GetLocalPosition();
		XMFLOAT3 GetWorldPosition();

		XMFLOAT3 GetScale();

		XMFLOAT3 GetRotation();
		XMFLOAT3 GetForward();
		XMFLOAT3 GetUp();
		XMFLOAT3 GetRight();

		XMFLOAT4X4 GetLocalMatrix();
		XMFLOAT4X4 GetWorldMatrix();

		bool IsNowDirty() { return m_IsDirty; }
		bool IsUpdatedThisFrame() { return m_IsUpdatedThisFrame; }
		void SetUpdatedThisFrame(bool updated) { m_IsUpdatedThisFrame = updated; }

		void SetLocalPosition(XMFLOAT3 pos);
		void SetWorldPosition(XMFLOAT3 pos);

		void Move(float distance);
		void TranslateWorld(XMFLOAT3 delta);
		void TranslateLocal(XMFLOAT3 delta);

		void SetScale(float x, float y, float z);
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
}

