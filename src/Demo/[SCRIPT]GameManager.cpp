#include "pch.h"

void GameManager::UpdateUIs()
{
    float hp = m_player->GetHealth();
    m_uiHealth->SetLoading(hp);
    m_uiScore->SetString(L"Score: " + std::to_wstring(m_score));

    m_uiManager->Update();
}

void GameManager::OnStart()
{
    ECS& ecs = ECS::Get();

    m_player = new Player(this);
	m_player->SetMaxHealth(100.0f);
	m_player->SetHealth(100.0f);
    m_railgen = ecs.CreateScript<GeneratingRail>();
	m_railgen->SetPlayer(m_player);
	m_railgen->SetGM(this);
    m_replayScript = ecs.CreateScript<ReplayMode>();

    int winWidth = FS_Device::Window()->Width();
    int winHeight = FS_Device::Window()->Height();

    TextureManager::LoadTexture("Crosshair", L"../../res/Textures/Crosshair.dds");
	ecs.AddSprite("Crosshair")->SetPosition(winWidth/2.0f, winHeight/2.0f);
	TextureManager::Get()->LoadTexture("Tree", L"../../res/Textures/tree01S.dds");
    MaterialManager::CreateMaterial(SHADER_TEXTURE_A_NAME, "TreeMat", "Tree");
    TextureManager::Get()->LoadTexture("Grass", L"../../res/Textures/grass.dds");
    MaterialManager::CreateMaterial(SHADER_TEXTURE_NAME, "GrassMat", "Grass");
    TextureManager::Get()->LoadTexture("Target", L"../../res/Textures/Target.dds");
    MaterialManager::CreateMaterial(SHADER_TEXTURE_NAME, "TargetMat", "Target");

    GenerateDecor();

    FS_InputsManager::Get()->LockMouseCursor();
    ShowCursor(FALSE);

    //UI
	m_uiManager = new UIManager();
	m_uiManager->AddUI(new UIFPS());
	m_uiScore = m_uiManager->CreateText();
	m_uiScore->SetString(L"Score: 0");
	m_uiScore->SetPosition(0, 100);
    m_uiHealth = m_uiManager->CreateBar();
	m_uiHealth->SetCapacity(100.0f);
	m_uiHealth->SetLoading(100.0f);
}

void GameManager::OnUpdate()
{
    // R�cup�ration ECS et delta time
    ECS& ecs = ECS::Get();
    float dt = ecs.GetDeltaTime();

    //REPLAY
    if (m_currentState == GAME_STATE::REPLAY)
    {
        if (FS_InputsManager::Get()->GetKeyDown(Keyboard::E))
        {
            m_replayScript->Reset();
            ResetGame();
        }

        if (FS_InputsManager::Get()->GetKey(Keyboard::LEFT))
        {
            m_replayScript->SetPlayingSpeed(-1.0f);
        }

        if (FS_InputsManager::Get()->GetKey(Keyboard::RIGHT))
        {
            m_replayScript->SetPlayingSpeed(2.0f);
        }

        return;
    }

    m_player->Update();
    //m_player->ApplyDamage(dt * 10.0f); // FOR TEST

    UpdateUIs();

    ///Replay mode controls to move somewhere else
    if (FS_InputsManager::Get()->GetKeyDown(Keyboard::SPACE))
    {
        ecs.SetClockSpeed(0.25f);
    }
    if (FS_InputsManager::Get()->GetKeyUp(Keyboard::SPACE))
    {
        ecs.SetClockSpeed(1.00f);
    }

    //Dont move the entity when replaying
    if (m_replayScript->IsPlaying())
        return;

    for (auto target : m_railgen->m_targets)
    {
        if (target->IsDead != true)
            target->Update();
    }
}

void GameManager::GenerateDecor()
{
	ECS& ecs = ECS::Get();
	int groundID = ecs.CreateEntity();
    TransformComponent* trs = ecs.AddComponent<TransformComponent>(groundID);
    MeshComponent* mesh = ecs.AddComponent<MeshComponent>(groundID);

    mesh->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::PLANE));
    mesh->SetMaterial("GrassMat");

    trs->SetLocalPosition({ 0, -2.0f, 10.0f });
    trs->SetRotation(0.0f, 3.141592 / 2, 0.0f);
    trs->SetScale(200.f, 0.0f, 200.0f);

    int ground2ID = ecs.CreateEntity();
    TransformComponent* trs2 = ecs.AddComponent<TransformComponent>(ground2ID);
    MeshComponent* mesh2 = ecs.AddComponent<MeshComponent>(ground2ID);

    mesh2->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::PLANE));
    mesh2->SetMaterial("GrassMat");

    trs2->SetLocalPosition({ 0, -3.0f, 10.0f });
    trs2->SetRotation(0.0f, 3.141592 / 2, 0.0f);
    trs2->SetScale(2000.f, 0.0f, 2000.0f);

    for (int i = 0; i < 10; i++)
    {
        Tree* tree = new Tree();
        tree->SetPosition({ (float)(rand() % 200 - 100), 3.0f, (float)(rand() % 200 - 100) });
        tree->SetScale({ 10.0f, 10.0f, 10.0f });
    }
}

void GameManager::StartReplay()
{ 
	m_replayScript->SetRecording(false);
    m_replayScript->SetPlaying(true);
    m_currentState = GAME_STATE::REPLAY;
}

void GameManager::ResetGame()
{
    m_railgen->m_currentPointIndex = 0;
    auto* armTrs = ECS::Get().GetComponent<TransformComponent>(m_player->GetEntityID());
    armTrs->SetLocalPosition({ 0.0f, 0.0f, 0.0f });
    m_currentState = GAME_STATE::PLAYING;
}