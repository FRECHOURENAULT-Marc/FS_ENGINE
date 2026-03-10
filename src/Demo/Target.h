
class Target
{
	int MaxLife = 100;
	int CurrentLife = 100;

public:
	int entityID;
	bool IsDead = false;
	void Create(XMFLOAT3 pos);
	void Update();
};


