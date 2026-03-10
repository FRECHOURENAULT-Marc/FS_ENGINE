class LivingEntity
{
protected:

	ECS* m_ecs = nullptr;
	int m_entityID = -1;
	float m_health = 0.0f;
	float m_maxHealth = 0.0f;
	float m_speed = 1.0f;
	float m_colliderRadius = 1.0f;

	bool m_isAlive = true;

	void SetupCollider();
	FS_SphereCollider* GetFSCollider();

public:
	LivingEntity(bool haveMesh = true);

	float GetHealth() { return m_health; }
	float GetMaxHealth() { return m_maxHealth; }
	float GetSpeed() { return m_speed; }
	int GetEntityID() { return m_entityID; }
	bool IsAlive() { return m_isAlive; }
	float GetColliderRadius() { return m_colliderRadius; };
	bool IsCollidingWith(int other);

	void SetHealth(float health);
	void ApplyDamage(float damage) { SetHealth(m_health - damage); }
	void SetMaxHealth(float maxHealth) { m_maxHealth = maxHealth; }
	void SetSpeed(float speed) { m_speed = speed; }
	void SetColliderRadius(float radius);
	void SetGeometry(GEO_TYPE type);

	virtual void Update() {};
};
