#include "pch.h"

void Player::UpdateMousePosition(float dt)
{
    XMINT2 center =
    {
        (int)(FS_Device::Get()->m_FSWindow->Width() * 0.5f),
        (int)(FS_Device::Get()->m_FSWindow->Height() * 0.5f)
    };

    XMINT2 mousePos = FS_InputsManager::Get()->GetMousePosition();

    m_mouseMovement.x = (center.x - mousePos.x) * m_sensitivity;
    m_mouseMovement.y = (center.y - mousePos.y) * m_sensitivity;

    FS_InputsManager::Get()->SetMousePosition(center);
    m_lastMousePos = center; // important pour éviter un gros delta au prochain frame
}

void Player::UpdateCameraRotation(float dt)
{
    float deltaX = -m_mouseMovement.x * 3.14f / 180.0f * dt * m_rotSpeed;
    float deltaY = -m_mouseMovement.y * 3.14f / 180.0f * dt * m_rotSpeed;

    auto* camTrs = m_ecs->GetComponent<TransformComponent>(m_armID);
    camTrs->RotateWorld(deltaX, deltaY, 0.0f);
    //OU
    //auto* cam = m_ecs->GetComponent<CameraComponent>(m_camID);
	//cam->RotateRad(deltaX, deltaY, 0.0f);
}

Player::Player(GameManager* gm) : LivingEntity(true)
{
	m_gm = gm;
    m_armID = m_entityID;
    m_speed = 5.0f;

    m_camID = m_ecs->CreateEntity();
	m_ecs->RemoveComponent<ColliderComponent>(m_camID);
    TransformComponent* camTrs = m_ecs->AddComponent<TransformComponent>(m_camID);
    MeshComponent* camMesh = m_ecs->AddComponent<MeshComponent>(m_camID);
    camMesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
    CameraComponent* cam = m_ecs->AddComponent<CameraComponent>(m_camID);
    m_ecs->GetEntity(m_armID)->AddChild(m_camID);


    m_staticCamID = m_ecs->CreateEntity();
    TransformComponent* staticTrs = m_ecs->AddComponent<TransformComponent>(m_staticCamID);
    staticTrs->SetWorldPosition({ 0, 5, -30 });
    m_ecs->AddComponent<CameraComponent>(m_staticCamID);

    cam->SetMainCamera(true);
	cam->SetRotationLimitPitch(-Maths::Pi * 0.35f, Maths::Pi * 0.35f);
    camTrs->SetLocalPosition({ 0, 5, -5 });
}

void Player::Update()
{
	float dt = m_ecs->GetDeltaTime();
    UpdateMousePosition(dt);
    UpdateCameraRotation(dt);

    TransformComponent* camTrs = m_ecs->GetComponent<TransformComponent>(m_camID);
    if (FS_InputsManager::Get()->GetButtonDown(Mouse::RIGHT))
    {
        Bullet* bullet = new Bullet();
		XMFLOAT3 camPos = camTrs->GetWorldPosition();
		XMFLOAT3 camDir = camTrs->GetForward();
        bullet->Create(camDir, camPos);
        BulletsList.push_back(bullet);
    }
    if (FS_InputsManager::Get()->GetKeyDown(Keyboard::B))
    {
        CameraComponent* cam = m_ecs->AddComponent<CameraComponent>(m_camID);
        CameraComponent* staticCam = m_ecs->AddComponent<CameraComponent>(m_staticCamID);

        if(cam->IsMainCamera())
        {
            cam->SetMainCamera(false);
			staticCam->SetMainCamera(true);
        }
        else
        {
			cam->SetMainCamera(true);
			staticCam->SetMainCamera(false);
        }
    }
    for (auto bull : BulletsList)
    {
        if (bull->IsDead != true)
            bull->Update(m_gm);
    }
	int LastIndex = BulletsList.size() - 1;
    for (int i = LastIndex; i >= 0; i--)
    {
        auto bull = BulletsList[i];
        if (bull->IsDead != true)
            continue;
		BulletsList.erase(BulletsList.begin() + i);
    }
}
