#include "pch.h"

//Demo de script
DECLARE_SCRIPT(ParentScript)
int m_entityID = -1;
virtual void OnStart()
{
	ECS& ecs = ECS::Get();
	int entity = ecs.CreateEntity();
	ecs.AddComponent<TransformComponent>(entity);
	std::cout << "ParentScript Created" << std::endl;
};
virtual void OnUpdate() { std::cout << "ParentScript updated" << std::endl; }

END_SCRIPT

DECLARE_CHILD_SCRIPT(ChildScript, ParentScript)
float m_timer = 0.0f;

float i = 0.0f;
float j = 0.0f;
float k = 0.0f;

int m_cameraEntityID2 = -1;
int m_cameraEntityID = -1;
int m_cameraEntityID3 = -1;
int m_secondEntityID = -1;
FS_Sprite* m_sprite = nullptr;

ReplayMode* m_replayScript = nullptr;

enum ChildStates
{
	IDLE,
	MOVING,
	REVERSING,
	COUNT
};

void TransparencePSOTest()
{
	for (int i = 1000; i >= 0; i--)
	{
		int e1 = m_ecs->CreateEntity();
		m_ecs->AddComponent<TransformComponent>(e1)->SetWorldPosition({ 0, 0, (float) -i});
		MeshComponent* mesh1 = m_ecs->AddComponent<MeshComponent>(e1);
		mesh1->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
		mesh1->SetMaterial("treeMat");
	}

}

virtual void OnStart()
{
	//Load new Textures (can be used everywhere after loading)
	TextureManager::LoadTexture("woodCrateTex", L"../../res/Textures/WoodCrate01.dds");
	TextureManager::LoadTexture("treeTex", L"../../res/Textures/Tree01S.dds");
	TextureManager::LoadTexture("WaterTex", L"../../res/Textures/water1.dds");
	//Create new material (can be used everywhere after creation)
	MaterialManager::CreateMaterial(SHADER_TEXTURE_NAME, "woodCrateMat", "woodCrateTex");
	MaterialManager::CreateMaterial(SHADER_TEXTURE_A_NAME, "treeMat", "treeTex");
	MaterialManager::CreateMaterial(SHADER_TEXTURE_NAME, "WaterMat", "WaterTex");

	ECS& ecs = ECS::Get();
	GeometryManager::Get();

	m_replayScript = ecs.CreateScript<ReplayMode>();
	m_entityID = ecs.CreateEntity();
	m_cameraEntityID = ecs.CreateEntity();

	//First Entity (pyramid and particules)
	TransformComponent* trs = ecs.AddComponent<TransformComponent>(m_entityID);
	MeshComponent* mesh = ecs.AddComponent<MeshComponent>(m_entityID);
	mesh->SetGeometry(GeometryManager::Get()->GetGeometry(GEO_TYPE::BOX));
	ecs.AddComponent<VelocityComponent>(m_entityID);

	//Material (Texture)
	mesh->SetColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	mesh->SetMaterial("woodCrateMat");

	//Collider
	ColliderComponent* collider = ecs.AddComponent<ColliderComponent>(m_entityID);
	FS_SphereCollider* fsCollider = collider->SetType<FS_SphereCollider>();
	fsCollider->SetRadius(1.0f);

	//Light
	/*int entityLight = ecs.CreateEntity();
	TransformComponent* trsLight = ecs.AddComponent<TransformComponent>(entityLight);
	LightComponent* light = ecs.AddComponent<LightComponent>(entityLight);
	PointLight* pLight = new PointLight();
	pLight->Strength = { 1, 1, 1 };
	light->SetPointLight(pLight);
	trsLight->SetLocalPosition({ 0, 0, -2 });*/

	//Emitter (particules)
	EmitterSettings emitterSettings;
	emitterSettings.partStartColor = { 1, 0, 0, 1 };
	emitterSettings.emitterShape = EMITER_SHAPE::CONE;
	emitterSettings.partDir = { 0, 0, -1 };
	emitterSettings.emitterMaxPart = 1;
	emitterSettings.emitterDensity = 20;	//moins que emitterMaxPart si on veut qq chose de continue
	emitterSettings.partGeotype = GEO_TYPE::BOX;
	emitterSettings.partMaterialName = MATERIAL_DEFAULT_COLOR_NAME;
	ecs.AddComponent<EmitterComponent>(m_entityID)->SetSettings(emitterSettings);

	//Cameras
	TransformComponent* camArm = ecs.AddComponent<TransformComponent>(m_cameraEntityID);
	CameraComponent* CamComp = ecs.AddComponent<CameraComponent>(m_cameraEntityID);
	CamComp->SetMainCamera(true);
	camArm->SetLocalPosition({ 0, 0, -10 }); //Offset

	m_cameraEntityID2 = ecs.CreateEntity();
	TransformComponent* camArm2 = ecs.AddComponent<TransformComponent>(m_cameraEntityID2);
	CameraComponent* CamComp2 = ecs.AddComponent<CameraComponent>(m_cameraEntityID2);
	camArm2->SetLocalPosition({ 0, 0, -30 });

	m_cameraEntityID3 = ecs.CreateEntity();
	TransformComponent* camArm3 = ecs.AddComponent<TransformComponent>(m_cameraEntityID3);
	CameraComponent* CamComp3 = ecs.AddComponent<CameraComponent>(m_cameraEntityID3);
	//CamComp2->SetMainCamera(true);
	camArm3->SetLocalPosition({ 0, -10, 0 }); //Offset
	camArm3->SetRotation(0, -1.5708f, 0); //Rotation pour voir la scene de dessus

	//State machine
	StateMachineComponent* compSM = ecs.AddComponent<StateMachineComponent>(m_entityID);
	compSM->InitializeStateMachine(ChildStates::COUNT);

	//Idle
	{
		ActionTest_Idle* pIdle = compSM->CreateAction<ActionTest_Idle>(ChildStates::IDLE);

		//-> Moving
		{
			auto transition = pIdle->CreateTransition(ChildStates::MOVING);

			transition->AddCondition<ConditionTest_SpacePressed>();
		}

		//-> Reversing
		{
			auto transition = pIdle->CreateTransition(ChildStates::REVERSING);

			transition->AddCondition<ConditionTest_EnterPressed>();
		}
	}

	//Moving
	{
		ActionTest_Moving* pMoving = compSM->CreateAction<ActionTest_Moving>(ChildStates::MOVING);

		//-> Idle
		{
			auto transition = pMoving->CreateTransition(ChildStates::IDLE);

			transition->AddCondition<ConditionTest_SpacePressed>();
		}
	}

	//Reversing	
	{
		ActionTest_Reverse* pReverse = compSM->CreateAction<ActionTest_Reverse>(ChildStates::REVERSING);

		//-> Idle
		{
			auto transition = pReverse->CreateTransition(ChildStates::IDLE);

			transition->AddCondition<ConditionTest_EnterPressed>();
		}
	}

	compSM->SetState(ChildStates::IDLE);

	//Child de l'entity du Script (m_entityID)
	m_secondEntityID = ecs.CreateEntity();
	TransformComponent* trs2 = ecs.AddComponent<TransformComponent>(m_secondEntityID);
	ecs.AddComponent<MeshComponent>(m_secondEntityID)->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::PYRAMID));
	ecs.GetEntity(m_entityID)->AddChild(m_secondEntityID);
	trs2->SetLocalPosition({ 1.75f, 0, 0 }); //SetPosition fonctionne comme un Offset par rapport au parent lorsque TransformComponent est child

	//UI
	m_sprite = ecs.AddSprite("woodCrateTex");
	//m_sprite->SetWorldPosition(0, 0);
	FS_Sprite* treeSpr = ecs.AddSprite("treeTex");
	treeSpr->SetPosition(600, 450);
	treeSpr->SetScaling(0.25f);


	TransparencePSOTest();
};

float m_angleYaw = 0.0f;
virtual void OnUpdate() 
{
	//Test

	ECS& ecs = ECS::Get();

	if (FS_InputsManager::Get()->GetKeyDown(Keyboard::NUMPAD2))
	{
		CameraComponent* cam = ecs.GetComponent<CameraComponent>(m_cameraEntityID3);
		cam->SetMainCamera(true);
	}
	if (FS_InputsManager::Get()->GetKeyDown(Keyboard::NUMPAD1))
	{
		ecs.GetComponent<CameraComponent>(m_cameraEntityID2)->SetMainCamera(true);
	}
	if (FS_InputsManager::Get()->GetKeyDown(Keyboard::NUMPAD0))
	{
		ecs.GetComponent<CameraComponent>(m_cameraEntityID)->SetMainCamera(true);
	}

	//Dont move the entity when replaying
	if (m_replayScript->IsPlaying())
		return;

	if (FS_InputsManager::Get()->GetKey(Keyboard::UP))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(0.0f, 1.0f * ecs.GetDeltaTime(), 0.0f);
	}
	if (FS_InputsManager::Get()->GetKey(Keyboard::DOWN))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(0.0f, -1.0f * ecs.GetDeltaTime(), 0.0f);
	}
	if (FS_InputsManager::Get()->GetKey(Keyboard::LEFT))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(-1.0f * ecs.GetDeltaTime(), 0.0f, 0.0f);
	}
	if (FS_InputsManager::Get()->GetKey(Keyboard::RIGHT))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(1.0f * ecs.GetDeltaTime(), 0.0f, 0.0f);
	}

	//Slow motion
	if (FS_InputsManager::Get()->GetKeyDown(Keyboard::R))
	{
		if(ecs.GetClockSpeed() > 1.0f)
			ecs.SetClockSpeed(0.2);
		else
			ecs.SetClockSpeed(5.0);
	}
	if (FS_InputsManager::Get()->GetKey(Keyboard::LSHIFT) && FS_InputsManager::Get()->GetKeyDown(Keyboard::R))
	{
		ecs.SetClockSpeed(1.0);
	}

	//Remove Sprite
	if (FS_InputsManager::Get()->GetKeyDown(Keyboard::S))
	{
		if (m_sprite != nullptr)
		{
			ecs.RemoveSprite(m_sprite);
			m_sprite = nullptr;
		}
			
	}
};
END_SCRIPT

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	DEBUG_ALLOC_CONSOLE

	try
	{
		ECS& ecs = ECS::Get();
		ecs.Create();
		ecs.CreateScript<ChildScript>();
		ecs.Run(hInstance, ECSSettings());

	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}