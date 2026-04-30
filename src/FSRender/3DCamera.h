#pragma once

namespace FSR
{

	class Camera3D
	{
		//<Camera----
		std::unique_ptr<UploadBuffer<CameraOC3D>> m_CB = nullptr;

		Transform m_Transform;

		XMFLOAT4X4 m_View3D = Maths::Identity4x4();
		XMFLOAT4X4 m_Proj = Maths::Identity4x4();
		XMFLOAT4X4 m_Ortho = Maths::Identity4x4();
		XMFLOAT4X4 m_viewProj = Maths::Identity4x4();
		XMFLOAT4X4 m_viewProj2D = Maths::Identity4x4();

		float m_NearZ = 0.0f;
		float m_FarZ = 0.0f;
		float m_Aspect = 0.0f;
		float m_FovY = 0.0f;
		float m_NearWindowHeight = 0.0f;
		float m_FarWindowHeight = 0.0f;

		bool m_IsDirty = true;
		bool m_isUpdatedThisFrame = true;

		void MakeDirty();

	public:
		Camera3D();

		void Update();
		void AfterUpdate();
		XMFLOAT3 GetPosition();
		XMFLOAT4X4 GetView() { return m_View3D; }
		XMFLOAT4X4 GetProj() { return m_Proj; }
		XMFLOAT4X4 GetViewProj() { return m_viewProj; }
		XMFLOAT4X4 GetViewProj2D() { return m_viewProj2D; }
		Transform GetTransform() { return m_Transform; }
		bool IsUpdatedThisFrame() { return m_isUpdatedThisFrame; }

		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& v);
		void SetWorld(XMFLOAT4X4 world);
		void SetUpdatedThisFrame(bool updated) { m_isUpdatedThisFrame = updated; }

		void UpdateMatrix();

		void SetLens(float fovY, float aspect, float zn, float zf);

	};

}

