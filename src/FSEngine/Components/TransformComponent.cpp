#include "pch.h"

void TransformComponent::MarkDirty()
{
	if (m_dirty)
		return;

	m_dirty = true;
	UpdateChildrenDirty();
}
void TransformComponent::UpdateChildrenDirty()
{
	ECS& ecs = ECS::Get();
	for (auto childID : m_children)
	{
		TransformComponent* child = ecs.GetComponent<TransformComponent>(childID);

		child->MarkDirty();
	}
}
void TransformComponent::UpdateWorldMatrix()
{
	XMFLOAT4X4 _local = GetLocalMatrix();
	XMMATRIX local = XMLoadFloat4x4(&_local);
	if (m_parent == -1)
	{
		XMStoreFloat4x4(&worldMatrix, local);
	}
	else
	{
		auto parent = ECS::Get().GetComponent<TransformComponent>(m_parent);

		XMFLOAT4X4 _parentWorld = parent->GetWorldMatrix();
		XMMATRIX parentWorld = XMLoadFloat4x4(&_parentWorld);

		XMMATRIX world = XMMatrixMultiply(local, parentWorld);

		XMStoreFloat4x4(&worldMatrix, world);
	}

	m_dirty = false;
}

////////////////
/// PUBLIC
////////////////

void TransformComponent::Reset()
{
	localTransform.Identity();
	worldMatrix = Maths::Identity4x4();

	m_children.clear();
	m_parent = -1;

	m_dirty = true;
}
TransformComponent::TransformComponent() : Component()
{
	worldMatrix = Maths::Identity4x4();
}

Transform& TransformComponent::GetLocalTransform()
{
	if (m_dirty)
		UpdateWorldMatrix();
	return localTransform;
}

XMFLOAT3 TransformComponent::GetLocalPosition()
{
	return localTransform.pos;
}
XMFLOAT3 TransformComponent::GetWorldPosition()
{
	XMFLOAT4X4 world = GetWorldMatrix();
	return { world._41, world._42, world._43 };
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
	return localTransform.GetWorld();
}
XMFLOAT4X4 TransformComponent::GetWorldMatrix()
{
	if (m_dirty)
		UpdateWorldMatrix();

	return worldMatrix;
}

void TransformComponent::SetLocalPosition(XMFLOAT3 pos)
{
	localTransform.SetPosition(pos.x, pos.y, pos.z);
	MarkDirty();
}
void TransformComponent::SetWorldPosition(XMFLOAT3 pos)
{
	if (m_parent == -1)
	{
		SetLocalPosition(pos);
		return;
	}

	auto parent = ECS::Get().GetComponent<TransformComponent>(m_parent);

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
	localTransform.Move(distance);
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
	localTransform.pos.x += delta.x;
	localTransform.pos.y += delta.y;
	localTransform.pos.z += delta.z;

	MarkDirty();
}

void TransformComponent::SetScale(float s)
{
	localTransform.SetScaling(s, s, s);
	MarkDirty();
}

void TransformComponent::RotateWorld(float yaw, float pitch, float roll)
{
	if (m_parent == -1)
	{
		RotateLocal(yaw, pitch, roll);
		return;
	}

	auto parent = ECS::Get().GetComponent<TransformComponent>(m_parent);

	XMFLOAT4X4 _parentWorld = parent->GetWorldMatrix();
	XMMATRIX parentWorld = XMLoadFloat4x4(&_parentWorld);
	XMMATRIX invParent = XMMatrixInverse(nullptr, parentWorld);

	XMVECTOR axis = XMVector3TransformNormal(XMVectorSet(yaw, pitch, roll, 0), invParent);

	XMFLOAT3 localAxis;
	XMStoreFloat3(&localAxis, axis);

	localTransform.AddYPR(localAxis.x, localAxis.y, localAxis.z);
	MarkDirty();
}
void TransformComponent::RotateLocal(float yaw, float pitch, float roll)
{
	localTransform.AddYPR(yaw, pitch, roll);
	MarkDirty();
}
void TransformComponent::SetRotation(float yaw, float pitch, float roll)
{
	localTransform.SetYPR(yaw, pitch, roll);
	MarkDirty();
}
void TransformComponent::SetForward(XMFLOAT3 forward)
{
	localTransform.forward = forward;
	MarkDirty();
}
void TransformComponent::ResetRotation()
{
	localTransform.ResetRotation();
	MarkDirty();
}

void TransformComponent::AddChild(TransformComponent* child)
{
	if (child->m_parent != -1)
		return;

	XMFLOAT3 worldPos = child->GetWorldPosition();

	m_children.push_back(child->GetID());
	child->m_parent = GetID();

	child->SetWorldPosition(worldPos);
	child->MarkDirty();
}
void TransformComponent::RemoveChild(TransformComponent* child)
{
	int indexToRemove = child->GetID();
	for (int i = 0; i < m_children.size(); i++)
	{
		auto trsID = m_children[i];
		if (trsID != indexToRemove)
			continue;

		child->MarkDirty();
		child->m_parent = -1;
		m_children.erase(m_children.begin() + i);
		return;
	}
}

TransformComponent* TransformComponent::GetParent()
{
	if (m_parent == -1)
		return nullptr;

	return ECS::Get().GetComponent<TransformComponent>(m_parent);
}
