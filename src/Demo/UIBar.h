class UIBar : public UIImage
{
protected:
	float m_capacity = 1.0f;
	float m_loading = 1.0f;
	float m_ratio = 0.0f;

	float m_maskPosY = 0.0f;
	float m_maskHeight = 0.0f;
	float m_maskWidth = 0.0f;

public:
	UIBar();

	virtual void OnUpdate() override;

	void SetCapacity(float _capacity) { m_capacity = _capacity; }
	void SetLoading(float _loading) { m_loading = _loading; }

	void AddLoading(float _loading) { m_loading += _loading; }
};


