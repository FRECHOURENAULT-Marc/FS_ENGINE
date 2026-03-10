#pragma once

class MeshComponent : public Component
{
	virtual void Reset() override;

	FS_3DObject* m_3dObject = nullptr; 

public: 
	MeshComponent();

	FS_3DObject* Get3DObject() { return m_3dObject; }
	bool SetGeometry(MeshGeometry* mesh);
	bool SetMaterial(std::string name);
	void SetColor(XMFLOAT4 color) { m_3dObject->SetColor(color); };
	void SetWorld(XMFLOAT4X4 world) { m_3dObject->SetWorld(world); };

	friend class ECS;
};
