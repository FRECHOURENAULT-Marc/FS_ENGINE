#include "pch.h"

///////////// 
///////////// Scripts
///////////// 

/// ParentScript
DECLARE_SCRIPT(ParentScript)
int m_entityID = -1;
virtual void OnStart()
{
	ECS& ecs = ECS::Get();
	int entity = ecs.CreateEntity();
	ecs.AddComponent<TransformComponent>(entity);
	std::cout << "ParentScript Created" << std::endl;
}
virtual void OnUpdate() { std::cout << "ParentScript updated" << std::endl; }

END_SCRIPT

/// ChildScript
DECLARE_CHILD_SCRIPT(ChildScript, ParentScript)

enum ChildStates
{
	IDLE,
	MOVING,
	REVERSING,
	COUNT
};

int m_cameraEntityID1 = -1;
int m_cameraEntityID2 = -1;

void TransparencePSOTest()
{
	for (int i = 1; i >= 0; i--)
	{
		int e1 = m_Ecs->CreateEntity();
		m_Ecs->AddComponent<TransformComponent>(e1)->SetWorldPosition({ 0, 0, (float)-i });
		MeshComponent* mesh1 = m_Ecs->AddComponent<MeshComponent>(e1);
		mesh1->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
		//mesh1->SetColor({ 1, 0, 0, 0.25f });
		mesh1->SetMaterial("treeMat");
	}

}

virtual void OnStart()
{
	//Load new Textures (can be used everywhere after loading)
	TextureManager::LoadTexture("woodCrateTex", L"../../res/Textures/WoodCrate01.dds");
	TextureManager::LoadTexture("treeTex", L"../../res/Textures/Tree01S.dds");
	TextureManager::LoadTexture("WaterTex", L"../../res/Textures/water1.dds");
	//Create new materials (can be used everywhere after creation)
	MaterialManager::CreateMaterial(SHADER_TEXTURE_A_NAME, "treeMat", "treeTex");
	MaterialManager::CreateMaterial(SHADER_TEXTURE_NAME, "WaterMat", "WaterTex");

	ECS& ecs = ECS::Get();

	//First Entity (box)
	m_entityID = ecs.CreateEntity();
	TransformComponent* trs = ecs.AddComponent<TransformComponent>(m_entityID);
	MeshComponent* mesh = ecs.AddComponent<MeshComponent>(m_entityID);
	mesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
	mesh->SetMaterial("waterMat");
	ecs.AddComponent<VelocityComponent>(m_entityID);
	ColliderComponent* collider = ecs.AddComponent<ColliderComponent>(m_entityID);
	ColliderSphere* fsCollider = collider->SetType<ColliderSphere>();
	fsCollider->SetRadius(1.0f);

	//Child Entity (pyramid)
	int childEntity = ecs.CreateEntity();
	TransformComponent* childTrs = ecs.AddComponent<TransformComponent>(childEntity);
	MeshComponent* childMesh = ecs.AddComponent<MeshComponent>(childEntity);
	childMesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::PYRAMID));
	childMesh->SetColor({ 1, 0, 1, 1 });
	childTrs->SetLocalPosition({ 0, 1.5f, 0 });
	trs->AddChild(childTrs);
	//Or
	//ecs.GetEntity(m_entityID)->AddChild(childEntity);

	//UI Entity
	int uiEntity = ecs.CreateEntity();
	TransformComponent* uiTrs = ecs.AddComponent<TransformComponent>(uiEntity);
	uiTrs->SetWorldPosition(utils::ScreenToWorld(0, 0));
	SpriteComponent* uiSpr = ecs.AddComponent<SpriteComponent>(uiEntity);
	uiSpr->SetTexture("treeTex");

	//Light
	int entityLight = ecs.CreateEntity();
	TransformComponent* trsLight = ecs.AddComponent<TransformComponent>(entityLight);
	LightComponent* light = ecs.AddComponent<LightComponent>(entityLight);
	PointLight* pLight = new PointLight();
	pLight->m_Strength = { 1, 1, 1 };
	light->SetPointLight(pLight);
	trsLight->SetLocalPosition({ 0, 0, -2 });

	//Emitter (particules)
	EmitterSettings emitterSettings;
	emitterSettings.m_EmitterShape = EMITER_SHAPE::CONE;
	emitterSettings.m_EmitterMaxPart = 10;
	emitterSettings.m_EmitterDensity = 20;
	emitterSettings.m_PartDir = { 0, 0, -1 };
	emitterSettings.m_PartGravity = { 0, -1, 0 };
	emitterSettings.m_PartGeoType = GEO_TYPE::BOX;
	emitterSettings.m_PartStartColor = { 1, 0, 0, 1 };
	emitterSettings.m_PartMaterialName = MATERIAL_DEFAULT_COLOR_NAME;
	ecs.AddComponent<EmitterComponent>(m_entityID)->SetSettings(emitterSettings);

	//State machine
	StateMachineComponent* compSM = ecs.AddComponent<StateMachineComponent>(m_entityID);
	compSM->InitializeStateMachine(ChildStates::COUNT);
	//Transitions & Actions
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

	//Camera front
	m_cameraEntityID1 = ecs.CreateEntity();
	TransformComponent* camArm1 = ecs.AddComponent<TransformComponent>(m_cameraEntityID1);
	CameraComponent* CamComp1 = ecs.AddComponent<CameraComponent>(m_cameraEntityID1);
	CamComp1->SetMainCamera(true);
	camArm1->SetWorldPosition({ 0, 0, -10 });
	//Camera top
	m_cameraEntityID2 = ecs.CreateEntity();
	TransformComponent* camArm2 = ecs.AddComponent<TransformComponent>(m_cameraEntityID2);
	CameraComponent* CamComp2 = ecs.AddComponent<CameraComponent>(m_cameraEntityID2);
	camArm2->SetWorldPosition({ 0, -10, 0 });
	camArm2->SetRotation(0, -Maths::Pi/2.0f, Maths::Pi);
};

virtual void OnUpdate() 
{
	ECS& ecs = ECS::Get();

	if (InputsManager::GetKeyDown(Keyboard::NUMPAD1))
	{
		ecs.GetComponent<CameraComponent>(m_cameraEntityID2)->SetMainCamera(true);
	}
	if (InputsManager::GetKeyDown(Keyboard::NUMPAD0))
	{
		ecs.GetComponent<CameraComponent>(m_cameraEntityID1)->SetMainCamera(true);
	}

	if (InputsManager::GetKey(Keyboard::UP))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(0.0f, 1.0f * ecs.GetDeltaTime(), 0.0f);
	}
	if (InputsManager::GetKey(Keyboard::DOWN))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(0.0f, -1.0f * ecs.GetDeltaTime(), 0.0f);
	}
	if (InputsManager::GetKey(Keyboard::LEFT))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(-1.0f * ecs.GetDeltaTime(), 0.0f, 0.0f);
	}
	if (InputsManager::GetKey(Keyboard::RIGHT))
	{
		ecs.GetComponent<TransformComponent>(m_entityID)->RotateWorld(1.0f * ecs.GetDeltaTime(), 0.0f, 0.0f);
	}

	//Slow motion
	if (InputsManager::GetKeyDown(Keyboard::R))
	{
		if(ecs.GetClockSpeed() > 1.0f)
			ecs.SetClockSpeed(0.2);
		else
			ecs.SetClockSpeed(5.0);
	}
	if (InputsManager::GetKey(Keyboard::LSHIFT) && InputsManager::GetKeyDown(Keyboard::R))
	{
		ecs.SetClockSpeed(1.0);
	}
};
END_SCRIPT

///////////// 
///////////// Scene
///////////// 
class SceneSandbox : public Scene
{
public:

	virtual void OnInit() override
	{
		ECS::Get().CreateScript<ChildScript>();
	}

	virtual void OnUpdate() override
	{
		if (InputsManager::GetKeyDown(Keyboard::NUMPAD_ADD))
			SceneManager::Get()->GoOnScene(1);
		if (InputsManager::GetKeyDown(Keyboard::NUMPAD_SUBTRACT))
			SceneManager::Get()->GoOnScene(0);
	}
};

///////////// 
///////////// Main
/////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// See memory leaks and allocations in to debug output (Visual Studio)
	//DEBUG_MEMORY_RECORD_ALL // --> the program ending will take more time
	DEBUG_ALLOC_CONSOLE

	ECS& ecs = ECS::Create();

	SceneManager::Get()->CreateScene<SceneSandbox>();
	SceneManager::Get()->CreateScene<SceneSandbox>();

	ECSSettings settings = ECSSettings();
	//settings.m_ShowFPSInWindowName = false;

	ecs.Run(hInstance, settings);
}