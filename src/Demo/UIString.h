class UIString : public UI
{
protected:
	FS_UIString m_string;

public:
	UIString();

	void SetString(std::wstring str);
	void SetPosition(float x, float y) { m_string.SetPosition(x, y); }

	virtual void OnUpdate() override {};
};


