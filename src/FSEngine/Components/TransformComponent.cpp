#include "pch.h"

namespace FSE
{
	void TransformComponent::MarkDirty()
	{
		if (m_IsDirty)
			return;
		m_IsDirty = true;
		m_IsUpdatedThisFrame = true;
		UpdateChildrenDirty();
	}
	void TransformComponent::UpdateChildrenDirty()
	{
		ECS& ecs = ECS::Get();
		for (auto childID : m_Children)
		{
			TransformComponent* child = ecs.GetComponent<TransformComponent>(childID);

			child->MarkDirty();
		}
	}
	void TransformComponent::UpdateWorldMatrix()
	{
		XMFLOAT4X4 _local = GetLocalMatrix();
		XMMATRIX local = XMLoadFloat4x4(&_local);
		if (m_ParentID == -1)
		{
			XMStoreFloat4x4(&m_WorldMatrix, local);
		}
		else
		{
			auto parent = ECS::Get().GetComponent<TransformComponent>(m_ParentID);

			XMFLOAT4X4 _parentWorld = parent->GetWorldMatrix();
			XMMATRIX parentWorld = XMLoadFloat4x4(&_parentWorld);

			XMMATRIX world = XMMatrixMultiply(local, parentWorld);

			XMStoreFloat4x4(&m_WorldMatrix, world);
		}

		m_IsDirty = false;
	}

	////////////////
	/// PUBLIC
	////////////////

	void TransformComponent::Reset()
	{
		m_LocalTransform.Identity();
		m_WorldMatrix = Maths::Identity4x4();

		m_Children.clear();
		m_ParentID = -1;

		m_IsDirty = true;
	}
	TransformComponent::TransformComponent() : Component()
	{
		m_WorldMatrix = Maths::Identity4x4();
	}

	Transform& TransformComponent::GetLocalTransform()
	{
		if (m_IsDirty)
			UpdateWorldMatrix();
		return m_LocalTransform;
	}

	XMFLOAT3 TransformComponent::GetLocalPosition()
	{
		return m_LocalTransform.m_Pos;
	}
	XMFLOAT3 TransformComponent::GetWorldPosition()
	{
		XMFLOAT4X4 world = GetWorldMatrix();
		return { world._41, world._42, world._43 };
	}

	XMFLOAT3 TransformComponent::GetScale()
	{
		if(m_ParentID == -1)
			return m_LocalTransform.m_Scale;

		XMFLOAT4X4 m = GetWorldMatrix();
		XMFLOAT3 translation;
		XMFLOAT3 scale;
		XMFLOAT3 rotation;
		utils::MatrixDecompose(m, &scale, &rotation, &translation);
		return scale;
	}

	//REWORK this
	XMFLOAT3 TransformComponent::GetRotation()
	{
		XMFLOAT4X4 m = GetWorldMatrix();
		XMFLOAT3 yawPitchRoll;
		utils::MatrixDecomposeRot(m, &yawPitchRoll);

		return yawPitchRoll;
	}
	XMFLOAT3 TransformComponent::GetForward()
	{
		XMFLOAT4X4 world = GetWorldMatrix();
		return { world.m[2][0], world.m[2][1], world.m[2][2] };
	}
	XMFLOAT3 TransformComponent::GetUp()
	{
		XMFLOAT4X4 world = GetWorldMatrix();
		return { world.m[1][0], world.m[1][1], world.m[1][2] };
	}
	XMFLOAT3 TransformComponent::GetRight()
	{
		XMFLOAT4X4 world = GetWorldMatrix();
		return { world.m[0][0], world.m[0][1], world.m[0][2] };
	}

	XMFLOAT4X4 TransformComponent::GetLocalMatrix()
	{
		return m_LocalTransform.GetWorld();
	}
	XMFLOAT4X4 TransformComponent::GetWorldMatrix()
	{
		if (m_IsDirty)
			UpdateWorldMatrix();

		return m_WorldMatrix;
	}

	void TransformComponent::SetLocalPosition(XMFLOAT3 pos)
	{
		m_LocalTransform.SetPosition(pos.x, pos.y, pos.z);
		MarkDirty();
	}
	void TransformComponent::SetWorldPosition(XMFLOAT3 pos)
	{
		if (m_ParentID == -1)
		{
			SetLocalPosition(pos);
			return;
		}

		auto parent = ECS::Get().GetComponent<TransformComponent>(m_ParentID);

		XMFLOAT4X4 _parentWorld = parent->GetWorldMatrix();
		XMMATRIX parentWorld = XMLoadFloat4x4(&_parentWorld);
		XMMATRIX invParent = XMMatrixInverse(nullptr, parentWorld);

		XMVECTOR worldPos = XMVectorSet(pos.x, pos.y, pos.z, 1);
		XMVECTOR local = XMVector3TransformCoord(worldPos, invParent);

		XMFLOAT3 l;
		XMStoreFloat3(&l, local);

		SetLocalPosition(l);
	}

	void TransformComponent::Move(float distance)
	{
		m_LocalTransform.Move(distance);
		MarkDirty();
	}
	void TransformComponent::TranslateWorld(XMFLOAT3 delta)
	{
		XMFLOAT3 pos = GetWorldPosition();

		pos.x += delta.x;
		pos.y += delta.y;
		pos.z += delta.z;

		SetWorldPosition(pos);
	}
	void TransformComponent::TranslateLocal(XMFLOAT3 delta)
	{
		XMFLOAT3 pos = m_LocalTransform.m_Pos;
		m_LocalTransform.SetPosition(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);

		MarkDirty();
	}

	void TransformComponent::SetScale(float x, float y, float z)
	{
		m_LocalTransform.SetScaling(x, y, z);
		MarkDirty();
	}

	void TransformComponent::SetScale(float s)
	{
		m_LocalTransform.SetScaling(s, s, s);
		MarkDirty();
	}

	void TransformComponent::RotateWorld(float yaw, float pitch, float roll)
	{
		if (m_ParentID == -1)
		{
			RotateLocal(yaw, pitch, roll);
			return;
		}

		auto parent = ECS::Get().GetComponent<TransformComponent>(m_ParentID);

		XMFLOAT4X4 _parentWorld = parent->GetWorldMatrix();
		XMMATRIX parentWorld = XMLoadFloat4x4(&_parentWorld);
		XMMATRIX invParent = XMMatrixInverse(nullptr, parentWorld);

		XMVECTOR axis = XMVector3TransformNormal(XMVectorSet(yaw, pitch, roll, 0), invParent);

		XMFLOAT3 localAxis;
		XMStoreFloat3(&localAxis, axis);

		m_LocalTransform.AddYPR(localAxis.x, localAxis.y, localAxis.z);
		MarkDirty();
	}
	void TransformComponent::RotateLocal(float yaw, float pitch, float roll)
	{
		m_LocalTransform.AddYPR(yaw, pitch, roll);
		MarkDirty();
	}
	void TransformComponent::SetRotation(float yaw, float pitch, float roll)
	{
		m_LocalTransform.SetYPR(yaw, pitch, roll);
		MarkDirty();
	}
	void TransformComponent::SetForward(XMFLOAT3 forward)
	{
		m_LocalTransform.m_Forward = forward;
		MarkDirty();
	}
	void TransformComponent::ResetRotation()
	{
		m_LocalTransform.ResetRotation();
		MarkDirty();
	}

	void TransformComponent::AddChild(TransformComponent* child)
	{
		if (child->m_ParentID != -1)
			return;

		XMFLOAT3 worldPos = child->GetWorldPosition();

		m_Children.push_back(child->GetID());
		child->m_ParentID = GetID();

		child->SetWorldPosition(worldPos);
		child->MarkDirty();
	}
	void TransformComponent::RemoveChild(TransformComponent* child)
	{
		int indexToRemove = child->GetID();
		for (int i = 0; i < m_Children.size(); i++)
		{
			auto trsID = m_Children[i];
			if (trsID != indexToRemove)
				continue;

			child->MarkDirty();
			child->m_ParentID = -1;
			m_Children.erase(m_Children.begin() + i);
			return;
		}
	}

	TransformComponent* TransformComponent::GetParent()
	{
		if (m_ParentID == -1)
			return nullptr;

		return ECS::Get().GetComponent<TransformComponent>(m_ParentID);
	}
}


