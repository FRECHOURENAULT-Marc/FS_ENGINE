#pragma once

namespace FSC
{

	class Object3D : public RenderObject
	{
	protected:
		XMFLOAT4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float m_DistanceToCamera = 0.0f; //For sorting transparent objects

		Object3D() = default;
	public:
		Object3D(MeshGeometry* geo);
		~Object3D() {};

		XMFLOAT4 GetColor() { return m_Color; }
		float GetDistanceToCamera() { return m_DistanceToCamera; };

		void SetColor(XMFLOAT4 color);
		void SetDistanceToCamera(float distance) { m_DistanceToCamera = distance; };
	};

}

