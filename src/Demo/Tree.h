#pragma once
class Tree
{
	std::vector<TransformComponent*> m_transform;
	int m_SubDivision = 4;

public:
	Tree();

	void SetPosition(XMFLOAT3 position);
	void SetScale(XMFLOAT3 scale);

};

