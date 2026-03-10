class UI
{
protected:
	ECS* m_ecs = nullptr;
	FS_Sprite* m_spr = nullptr;
	float m_timer = 0;
	float m_time = 0.1f;
public:
	UI();

	void Update();
	virtual void OnUpdate() {};

	FS_Sprite* GetSprite() { return m_spr; }
};


