DECLARE_SCRIPT(GeneratingRail)

#include <cstdlib>
#include <ctime>

std::vector<XMFLOAT3> m_curvePoints;
std::vector<Target*> m_targets;

MeshGeometry* m_newGeo;

GameManager* m_gm = nullptr;
ECS* m_ecs = nullptr;
Player* m_player = nullptr;
int m_currentPointIndex = 0;

virtual void OnStart();
virtual void OnUpdate();

void SetPlayer(Player* p) { m_player = p; };
void SetGM(GameManager* gm) { m_gm = gm; };

END_SCRIPT
