#pragma once

namespace FSC
{

	struct GeometryData
	{
		std::vector<Vertex> m_Vertices;
		std::vector<uint16_t> m_Indices;
	};

	//RULE => Tout mettre en static 
	class GeometryGenerator
	{
		//Utils
		static void CreateCylinderCap(float radius, float height, int sliceCount, std::vector<Vertex>& vertices, std::vector<uint16_t>& indices, bool isTop);

	public:

		static GeometryData* CreateBox();
		static GeometryData* CreateBox_UVMapped();
		static GeometryData* CreatePyramid();
		static GeometryData* CreateSphere(float radius, int sliceCount, int stackCount);
		static GeometryData* CreatePlane(float width, float height);
		static GeometryData* CreatePlaneScreenOriented(float width, float height);
		static GeometryData* CreateCylinder(float bottomRadius, float topRadius, float height, int sliceCount, int stackCount);
		static GeometryData* CreateCapsule(float radius, float height, int sliceCount, int stackCount);
	};

}