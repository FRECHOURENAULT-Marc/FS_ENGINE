
class GameManager;

class Bullet
{
	int MaxLife = 100;
	int CurrentLife = 100;

	float lifeTime = 2.0f;
	float duringTime = 0.0f;

public:
	int entityID;
	bool IsDead = false;
	void Create(XMFLOAT3 rot, XMFLOAT3 _pos);
	void Update(GameManager* GM);
};

