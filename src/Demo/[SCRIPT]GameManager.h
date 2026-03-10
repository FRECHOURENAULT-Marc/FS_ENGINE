enum GAME_STATE
{
	PLAYING,
	REPLAY,
	MENU
};

DECLARE_SCRIPT(GameManager)

GeneratingRail* m_railgen;
ReplayMode* m_replayScript;

UIManager* m_uiManager;
UIString* m_uiScore;
UIBar* m_uiHealth;
Player* m_player = nullptr;

int m_score = 0;
int m_currentState = GAME_STATE::PLAYING;

void UpdateUIs();

//std::vector<Bullet*> BulletsList;

virtual void OnStart();
virtual void OnUpdate();
void StartReplay();
void ResetGame();
void GenerateDecor();



END_SCRIPT
