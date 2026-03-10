class Player : public LivingEntity
{
	std::vector<Bullet*> BulletsList;

	GameManager* m_gm = nullptr;

	XMINT2 m_lastMousePos = { 0,0 };
	XMINT2 m_mouseMovement = { 0,0 };
	float m_sensitivity = 1.0f;
	float m_rotSpeed = 50.0f;

	int m_camID;
	int m_armID;

	void UpdateMousePosition(float dt);
	void UpdateCameraRotation(float dt);
public:
	Player(GameManager* gm);
	virtual void Update() override;
};
