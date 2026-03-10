#include "pch.h"

GeometryData* GeometryGenerator::CreateBox()
{
	std::vector<Vertex> vertices =
	{
		// FRONT (-Z)
		{{-1,-1,-1},{0,1},{0,0,-1},{1,0,0}},
		{{-1, 1,-1},{0,0},{0,0,-1},{1,0,0}},
		{{ 1, 1,-1},{1,0},{0,0,-1},{1,0,0}},
		{{ 1,-1,-1},{1,1},{0,0,-1},{1,0,0}},

		// BACK (+Z)
		{{-1,-1, 1},{1,1},{0,0,1},{-1,0,0}},
		{{ 1,-1, 1},{0,1},{0,0,1},{-1,0,0}},
		{{ 1, 1, 1},{0,0},{0,0,1},{-1,0,0}},
		{{-1, 1, 1},{1,0},{0,0,1},{-1,0,0}},

		// LEFT (-X)
		{{-1,-1, 1},{0,1},{-1,0,0},{0,0,-1}},
		{{-1, 1, 1},{0,0},{-1,0,0},{0,0,-1}},
		{{-1, 1,-1},{1,0},{-1,0,0},{0,0,-1}},
		{{-1,-1,-1},{1,1},{-1,0,0},{0,0,-1}},

		// RIGHT (+X)
		{{ 1,-1,-1},{0,1},{1,0,0},{0,0,1}},
		{{ 1, 1,-1},{0,0},{1,0,0},{0,0,1}},
		{{ 1, 1, 1},{1,0},{1,0,0},{0,0,1}},
		{{ 1,-1, 1},{1,1},{1,0,0},{0,0,1}},

		// TOP (+Y)
		{{-1,1,-1},{0,1},{0,1,0},{1,0,0}},
		{{-1,1, 1},{0,0},{0,1,0},{1,0,0}},
		{{ 1,1, 1},{1,0},{0,1,0},{1,0,0}},
		{{ 1,1,-1},{1,1},{0,1,0},{1,0,0}},

		// BOTTOM (-Y)
		{{-1,-1, 1},{0,1},{0,-1,0},{1,0,0}},
		{{-1,-1,-1},{0,0},{0,-1,0},{1,0,0}},
		{{ 1,-1,-1},{1,0},{0,-1,0},{1,0,0}},
		{{ 1,-1, 1},{1,1},{0,-1,0},{1,0,0}},
	};

	std::vector<uint16_t> indices;
	for (int i = 0; i < 6; ++i)
	{
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}

	GeometryData* geoData = new GeometryData();
	geoData->vertices = vertices;
	geoData->indices = indices;
	return geoData;
}

GeometryData* GeometryGenerator::CreatePyramid()
{
	// Vertices
	std::vector<Vertex> vertices =
	{
		// ===== BASE (normale vers le bas, y=0) =====
		{{-1,0,-1},{0,1},{0,-1,0},{0,0,0}}, // 0
		{{ 1,0,-1},{1,1},{0,-1,0},{0,0,0}}, // 1
		{{ 1,0, 1},{1,0},{0,-1,0},{0,0,0}}, // 2
		{{-1,0, 1},{0,0},{0,-1,0},{0,0,0}}, // 3

		// ===== FACE AVANT (base 0,1, tip 4) =====
		{{-1,0,-1},{0,1},{0,0,-1},{0,0,0}}, // 4
		{{ 1,0,-1},{1,1},{0,0,-1},{0,0,0}}, // 5
		{{ 0,1,0},{0.5f,0},{0,0,-1},{0,0,0}}, // 6

		// ===== FACE DROITE (base 1,2, tip 4) =====
		{{ 1,0,-1},{0,1},{1,0,0},{0,0,0}}, // 7
		{{ 1,0, 1},{1,1},{1,0,0},{0,0,0}}, // 8
		{{ 0,1,0},{0.5f,0},{1,0,0},{0,0,0}}, // 9

		// ===== FACE ARRIERE (base 2,3, tip 4) =====
		{{ 1,0, 1},{0,1},{0,0,1},{0,0,0}}, // 10
		{{-1,0, 1},{1,1},{0,0,1},{0,0,0}}, // 11
		{{ 0,1, 0},{0.5f,0},{0,0,1},{0,0,0}}, // 12

		// ===== FACE GAUCHE (base 3,0, tip 4) =====
		{{-1,0, 1},{0,1},{-1,0,0},{0,0,0}}, // 13
		{{-1,0,-1},{1,1},{-1,0,0},{0,0,0}}, // 14
		{{ 0,1, 0},{0.5f,0},{-1,0,0},{0,0,0}} // 15
	};

	// Indices : ordre horaire pour culling front-face
	std::vector<uint16_t> indices =
	{
		// Base
		0,1,2, 0,2,3,

		// Faces latérales (tip en dernier, inversé pour front-face)
		6,5,4,    // Face avant
		9,8,7,    // Face droite
		12,11,10, // Face arrière
		15,14,13  // Face gauche
	};

	GeometryData* geoData = new GeometryData();
	geoData->vertices = vertices;
	geoData->indices = indices;
	return geoData;
}

GeometryData* GeometryGenerator::CreateSphere(float radius, int sliceCount, int stackCount)
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	for (int i = 0; i <= stackCount; ++i)
	{
		float phi = XM_PI * i / stackCount;

		for (int j = 0; j <= sliceCount; ++j)
		{
			float theta = 2.0f * XM_PI * j / sliceCount;

			Vertex v;

			v.Pos.x = radius * sinf(phi) * cosf(theta);
			v.Pos.y = radius * cosf(phi);
			v.Pos.z = radius * sinf(phi) * sinf(theta);

			// NORMAL = normalize(position)
			XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&v.Pos));
			XMStoreFloat3(&v.Normal, n);

			// TANGENT
			v.Tangent = {
				-sinf(theta),
				0.0f,
				cosf(theta)
			};

			XMStoreFloat3(&v.Tangent,
				XMVector3Normalize(XMLoadFloat3(&v.Tangent)));

			// UV
			v.UV.x = theta / (XM_2PI);
			v.UV.y = phi / XM_PI;

			vertices.push_back(v);
		}
	}

	int ring = sliceCount + 1;
	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			indices.push_back(i * ring + j);
			indices.push_back((i + 1) * ring + j + 1);
			indices.push_back((i + 1) * ring + j);

			indices.push_back(i * ring + j);
			indices.push_back(i * ring + j + 1);
			indices.push_back((i + 1) * ring + j + 1);
		}
	}

	GeometryData* geoData = new GeometryData();
	geoData->vertices = vertices;
	geoData->indices = indices;
	return geoData;
}

GeometryData* GeometryGenerator::CreatePlane(float width, float height)
{
	std::vector<Vertex> vertices =
	{
		{{-width / 2, -height / 2, 0},{0,1},{0,1,0},{1,0,0}},
		{{ width / 2, -height / 2, 0},{1,1},{0,1,0},{1,0,0}},
		{{ width / 2, height / 2,  0},{1,0},{0,1,0},{1,0,0}},
		{{-width / 2, height / 2,  0},{0,0},{0,1,0},{1,0,0}},
	};

	std::vector<uint16_t> indices =
	{
		0,2,1,
		0,3,2
	};

	GeometryData* geoData = new GeometryData();
	geoData->vertices = vertices;
	geoData->indices = indices;
	return geoData;
}

void GeometryGenerator::CreateCylinderCap(float radius, float height, int sliceCount, std::vector<Vertex>& vertices, std::vector<uint16_t>& indices, bool isTop)
{
	int baseIndex = static_cast<int>(vertices.size());
	float y = 0.5f * height * (isTop ? 1 : -1);
	float dTheta = 2.0f * XM_PI / sliceCount;

	Vertex vertex = {};

	// crée les sommets de l'anneau
	for (int i = 0; i <= sliceCount; ++i)
	{
		float x = radius * cosf(i * dTheta);
		float z = radius * sinf(i * dTheta);

		vertex.Pos = { x, y , z };
		vertex.Normal = { 0.0f,  isTop ? 1.0f : -1.0f, 0.0f };
		vertex.Tangent = { 1.0f, 0.0f, 0.0f };
		vertex.UV = { 0.5f * (x / radius + 1.0f), 0.5f * (z / radius + 1.0f) };

		vertices.push_back(vertex);
	}

	// centre du cap
	vertex.Pos = { 0.0f, y , 0.0f };
	vertex.Normal = { 0.0f, isTop ? 1.0f : -1.0f, 0.0f };
	vertex.Tangent = { 1.0f, 0.0f, 0.0f };
	vertex.UV = { 0.5f, 0.5f };
	vertices.push_back(vertex);

	int centerIndex = static_cast<int>(vertices.size()) - 1;

	// indices : ordre correct pour top (normale +Y) et bottom (normale -Y)
	for (int i = 0; i < sliceCount; ++i)
	{
		int v0 = baseIndex + i;
		int v1 = baseIndex + i + 1;

		if (isTop)
		{
			// normale vers +Y
			indices.push_back(centerIndex);
			indices.push_back(v1);
			indices.push_back(v0);

		}
		else
		{
			//normale vers -Y
			indices.push_back(centerIndex);
			indices.push_back(v0);
			indices.push_back(v1);
		}
	}
}
GeometryData* GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, int sliceCount, int stackCount)
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	float stackHeight = height / stackCount;
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	for (int i = 0; i <= stackCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		for (int j = 0; j <= sliceCount; ++j)
		{
			float theta = XM_2PI * j / sliceCount;
			float c = cosf(theta);
			float s = sinf(theta);

			Vertex v;
			v.Pos = { r * c, y, r * s };

			XMFLOAT3 Normal(c, 0, s);
			XMStoreFloat3(&v.Normal,
				XMVector3Normalize(XMLoadFloat3(&Normal)));

			v.Tangent = { -s,0,c };
			XMStoreFloat3(&v.Tangent,
				XMVector3Normalize(XMLoadFloat3(&v.Tangent)));

			v.UV = { j / (float)sliceCount, 1.0f - i / (float)stackCount };

			vertices.push_back(v);
		}
	}

	int ring = sliceCount + 1;
	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			indices.push_back(i * ring + j);
			indices.push_back((i + 1) * ring + j);
			indices.push_back((i + 1) * ring + j + 1);

			indices.push_back(i * ring + j);
			indices.push_back((i + 1) * ring + j + 1);
			indices.push_back(i * ring + j + 1);
		}
	}

	CreateCylinderCap(bottomRadius, height, sliceCount, vertices, indices, false);
	CreateCylinderCap(bottomRadius, height, sliceCount, vertices, indices, true);

	GeometryData* geoData = new GeometryData();
	geoData->vertices = vertices;
	geoData->indices = indices;
	return geoData;
}

GeometryData* GeometryGenerator::CreateCapsule(float radius, float height, int sliceCount, int stackCount)
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// Définitions robustes des subdivisions
	int hemiStacks = max(2, stackCount); // piles par hémisphère
	int slices = max(3, sliceCount);

	// Calcul d'un nombre de segments cylindriques proportionnel à la hauteur
	int cylinderSegments = 0;
	if (height > 0.0f)
	{
		// proportionne le nombre de segments cylindre à la hauteur relative au rayon
		cylinderSegments = max(0, (int)std::round((height / radius) * hemiStacks));
	}

	float halfHeight = height * 0.5f;
	float totalHeight = height + 2.0f * radius;

	// ----- Anneaux du haut : hémisphère supérieur (du pole au plan équatorial) -----
	for (int i = 0; i <= hemiStacks; ++i)
	{
		float t = i / (float)hemiStacks; // 0..1
		float phi = t * (XM_PIDIV2);    // 0..pi/2

		float ringRadius = radius * sinf(phi);
		float y = halfHeight + radius * cosf(phi); // centre hémisphère en y = +halfHeight

		for (int j = 0; j <= slices; ++j)
		{
			float u = j / (float)slices;
			float theta = XM_2PI * u;

			float x = ringRadius * cosf(theta);
			float z = ringRadius * sinf(theta);

			Vertex v;
			v.Pos = { x, y, z };

			// Normale calculée depuis le centre de l'hémisphère supérieur (0, +halfHeight, 0)
			XMFLOAT3 fromCenter = { x, y - halfHeight, z };
			XMStoreFloat3(&v.Normal, XMVector3Normalize(XMLoadFloat3(&fromCenter)));

			// Tangente approximative
			XMFLOAT3 tangent = { -sinf(theta), 0.0f, cosf(theta) };
			XMStoreFloat3(&v.Tangent, XMVector3Normalize(XMLoadFloat3(&tangent)));

			// UV en y basé sur la position verticale dans la hauteur totale
			v.UV = { u, (y + totalHeight * 0.5f) / totalHeight };

			vertices.push_back(v);
		}
	}

	// ----- Anneaux du cylindre : segments intermédiaires (excluant les plans d'équateur) -----
	if (cylinderSegments > 0)
	{
		for (int i = 1; i <= cylinderSegments; ++i)
		{
			float f = i / (float)(cylinderSegments + 1); // fraction le long du cylindre (exclut les bords)
			float y = halfHeight - f * height;
			for (int j = 0; j <= slices; ++j)
			{
				float u = j / (float)slices;
				float theta = XM_2PI * u;

				float x = radius * cosf(theta);
				float z = radius * sinf(theta);

				Vertex v;
				v.Pos = { x, y, z };

				// Normale cylindrique (horizontale)
				XMFLOAT3 n = { cosf(theta), 0.0f, sinf(theta) };
				XMStoreFloat3(&v.Normal, XMVector3Normalize(XMLoadFloat3(&n)));

				XMFLOAT3 tangent = { -sinf(theta), 0.0f, cosf(theta) };
				XMStoreFloat3(&v.Tangent, XMVector3Normalize(XMLoadFloat3(&tangent)));

				v.UV = { u, (y + totalHeight * 0.5f) / totalHeight };

				vertices.push_back(v);
			}
		}
	}

	// ----- Anneaux du bas : hémisphère inférieur (du plan équatorial au pôle) -----
	// On commence à i=1 pour éviter de dupliquer l'anneau d'équateur quand il existe déjà
	for (int i = 1; i <= hemiStacks; ++i)
	{
		float t = i / (float)hemiStacks; // 1..hemiStacks / hemiStacks
		float phi = XM_PIDIV2 + t * (XM_PIDIV2); // pi/2 .. pi

		float ringRadius = radius * sinf(phi);
		float y = -halfHeight + radius * cosf(phi); // centre hémisphère en y = -halfHeight

		for (int j = 0; j <= slices; ++j)
		{
			float u = j / (float)slices;
			float theta = XM_2PI * u;

			float x = ringRadius * cosf(theta);
			float z = ringRadius * sinf(theta);

			Vertex v;
			v.Pos = { x, y, z };

			// Normale depuis le centre de l'hémisphère inférieur (0, -halfHeight, 0)
			XMFLOAT3 fromCenter = { x, y + halfHeight, z }; // y - (-halfHeight) = y + halfHeight
			XMStoreFloat3(&v.Normal, XMVector3Normalize(XMLoadFloat3(&fromCenter)));

			XMFLOAT3 tangent = { -sinf(theta), 0.0f, cosf(theta) };
			XMStoreFloat3(&v.Tangent, XMVector3Normalize(XMLoadFloat3(&tangent)));

			v.UV = { u, (y + totalHeight * 0.5f) / totalHeight };

			vertices.push_back(v);
		}
	}

	// Indices : connexion entre anneaux consécutifs
	int ringVertexCount = slices + 1;
	int ringCount = static_cast<int>(vertices.size()) / ringVertexCount;

	for (int r = 0; r < ringCount - 1; ++r)
	{
		for (int s = 0; s < slices; ++s)
		{
			uint16_t i0 = static_cast<uint16_t>(r * ringVertexCount + s);
			uint16_t i1 = static_cast<uint16_t>((r + 1) * ringVertexCount + s);
			uint16_t i2 = static_cast<uint16_t>((r + 1) * ringVertexCount + s + 1);
			uint16_t i3 = static_cast<uint16_t>(r * ringVertexCount + s + 1);

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}

	GeometryData* geoData = new GeometryData();
	geoData->vertices = vertices;
	geoData->indices = indices;
	return geoData;
}