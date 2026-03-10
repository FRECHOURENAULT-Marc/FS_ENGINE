#pragma once

enum class GEO_TYPE
{
	BOX, 
	PYRAMID, 
	SPHERE, 
	PLANE, 
	CYLINDER, 
	CAPSULE
};

class GeometryManager
{
	std::vector<MeshGeometry*> m_geometries;

	static inline GeometryManager* m_instance = nullptr;

public:
	GeometryManager();

	static GeometryManager* Get();
	static MeshGeometry* GetGeometry(GEO_TYPE index) { return m_instance->m_geometries[(int)index]; }
	static MeshGeometry* BuildGeometry(GeometryData* goeData);
};

