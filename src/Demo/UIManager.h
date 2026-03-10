class UIManager final
{
	ECS* m_ecs = nullptr;
	std::vector<UI*> m_uiList = std::vector<UI*>();

public:
	UIManager();
	void Update();

	void AddUI(UI* _ui) { m_uiList.push_back(_ui); }
	UIBar* CreateBar();
	UIString* CreateText();
	UIImage* CreateImage(std::string texName);
};

