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
	TransformComponent* camTrs1 = ecs.AddComponent<TransformComponent>(m_cameraEntityID1);
	CameraComponent* camComp1 = ecs.AddComponent<CameraComponent>(m_cameraEntityID1);
	camComp1->SetMainCamera(true);
	camTrs1->SetWorldPosition({ 0, 0, -10 });
	//Camera top
	m_cameraEntityID2 = ecs.CreateEntity();
	TransformComponent* camTrs2 = ecs.AddComponent<TransformComponent>(m_cameraEntityID2);
	CameraComponent* camComp2 = ecs.AddComponent<CameraComponent>(m_cameraEntityID2);
	camTrs2->SetWorldPosition({ 0, -10, 0 });
	camTrs2->SetRotation(0, -Maths::Pi/2.0f, Maths::Pi);
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
		if (InputsManager::GetKeyDown(Keyboard::NUMPAD3))
			SceneManager::Get()->GoOnScene(2);
	}
};
class SceneDemo : public Scene
{
	VelocityComponent* movingCubeVel = nullptr;

	TransformComponent* camTrs = nullptr;
	TransformComponent* cubeTrs = nullptr;
	float m_OrbitAngle = 0.0f;
	float m_OrbitSpeed = Maths::Pi / 8.0f; // radians / sec
	float m_OrbitRadius = 4.0f;
	float m_OrbitHeight = 2.0f;

public:

	virtual void OnInit() override
	{
		ECS& ecs = ECS::Get();

		// Floor
		int floor = ecs.CreateEntity();
		TransformComponent* floorTrs = ecs.AddComponent<TransformComponent>(floor);
		floorTrs->SetScale(50, 1, 50);
		floorTrs->SetWorldPosition({ 0, -floorTrs->GetScale().y / 2.0f, 0 });
		MeshComponent* floorMesh = ecs.AddComponent<MeshComponent>(floor);
		floorMesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
		floorMesh->SetColor({0, 0, 0, 1});

		// Cube
		int cube = ecs.CreateEntity();
		cubeTrs = ecs.AddComponent<TransformComponent>(cube);
		cubeTrs->SetWorldPosition({ 0, cubeTrs->GetScale().y / 2.0f, 0 });
		MeshComponent* cubeMesh = ecs.AddComponent<MeshComponent>(cube);
		cubeMesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
		cubeMesh->SetColor({0.5f, 0.5f, 0.5f, 1.0f});

		// Moving cube
		int movingCube = ecs.CreateEntity();
		TransformComponent* movingCubeTrs = ecs.AddComponent<TransformComponent>(movingCube);
		movingCubeTrs->SetScale(0.25f);
		movingCubeTrs->SetWorldPosition({ 0, movingCubeTrs->GetScale().y / 2.0f + 1.0f, 0 });
		MeshComponent* movingCubeMesh = ecs.AddComponent<MeshComponent>(movingCube);
		movingCubeMesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::BOX));
		movingCubeMesh->SetColor({ 1, 1, 1, 0.5f });
		movingCubeMesh->SetMaterial(MATERIAL_DEFAULT_COLOR_A_NAME);
		movingCubeVel = ecs.AddComponent<VelocityComponent>(movingCube);
		movingCubeVel->SetVelocity(1);

		// Sphere
		int sphere = ecs.CreateEntity();
		TransformComponent* sphereTrs = ecs.AddComponent<TransformComponent>(sphere);
		sphereTrs->SetScale(0.25f);
		sphereTrs->SetWorldPosition({ 0, sphereTrs->GetScale().y / 2.0f, 1});
		MeshComponent* sphereMesh = ecs.AddComponent<MeshComponent>(sphere);
		sphereMesh->SetGeometry(GeometryManager::GetGeometry(GEO_TYPE::SPHERE));
		sphereMesh->SetColor({1, 0, 0, 1});

		// Custom Sphere
		int customSphere = ecs.CreateEntity();
		TransformComponent* customSphereTrs = ecs.AddComponent<TransformComponent>(customSphere);
		customSphereTrs->SetScale(0.5f);
		customSphereTrs->SetWorldPosition({ 1.0f, customSphereTrs->GetScale().y / 2.0f, 0.5f});
		MeshComponent* customSphereMesh = ecs.AddComponent<MeshComponent>(customSphere);
		// Create custom geometry
		CommandList* cmdList = Device::GetCommand();
		ThrowIfFailed(cmdList->m_CommandList->Reset(cmdList->m_DirectCmdListAlloc.Get(), nullptr));
		MeshGeometry* customSphereGeometry = GeometryManager::BuildGeometry(GeometryGenerator::CreateSphere(0.5f, 64, 32));
		cmdList->ExecuteCommands();
		cmdList->FlushCommandQueue();
		customSphereMesh->SetGeometry(customSphereGeometry);
		customSphereMesh->SetColor({1, 0, 0, 1});

		// Custom Cylinder
		int customCylinder = ecs.CreateEntity();
		TransformComponent* customCylinderTrs = ecs.AddComponent<TransformComponent>(customCylinder);
		customCylinderTrs->SetScale(1.0f, 0.5f, 1.0f);
		customCylinderTrs->SetWorldPosition({ -0.5f, customCylinderTrs->GetScale().y / 2.0f, -1.0f });
		MeshComponent* customCylinderMesh = ecs.AddComponent<MeshComponent>(customCylinder);
		// Create custom geometry
		ThrowIfFailed(cmdList->m_CommandList->Reset(cmdList->m_DirectCmdListAlloc.Get(), nullptr));
		MeshGeometry* customCylinderGeometry = GeometryManager::BuildGeometry(GeometryGenerator::CreateCylinder(0.2f, 0.3f, 1.0f, 64, 32));
		cmdList->ExecuteCommands();
		cmdList->FlushCommandQueue();
		customCylinderMesh->SetGeometry(customCylinderGeometry);
		customCylinderMesh->SetColor({ 0, 0, 1, 1 });
		// Emitter (particules)
		int emitter = ecs.CreateEntity();
		TransformComponent* emitterTrs = ecs.AddComponent<TransformComponent>(emitter);
		customCylinderTrs->AddChild(emitterTrs);
		emitterTrs->SetLocalPosition({ 0.0f, 0.25f, 0.0f });
		EmitterSettings emitterSettings;
		emitterSettings.m_EmitterShape = EMITER_SHAPE::CONE;
		emitterSettings.m_EmitterMaxPart = 500;
		emitterSettings.m_EmitterDensity = 20;
		emitterSettings.m_PartDir = { 0, 1, 0 };
		emitterSettings.m_PartGravity = { 0, -1, 0 };
		emitterSettings.m_PartGeoType = GEO_TYPE::SPHERE;
		emitterSettings.m_PartStartColor = { 0, 1, 0, 1 };
		emitterSettings.m_PartMaterialName = MATERIAL_DEFAULT_COLOR_NAME;
		ecs.AddComponent<EmitterComponent>(emitter)->SetSettings(emitterSettings);

		// Light
		int light = ecs.CreateEntity();
		TransformComponent* lightTrs = ecs.AddComponent<TransformComponent>(light);
		LightComponent* lightComp = ecs.AddComponent<LightComponent>(light);
		PointLight* lightP = new PointLight();
		lightP->m_Strength = { 1, 1, 1 };
		lightComp->SetPointLight(lightP);
		lightTrs->SetWorldPosition({ 0, 2, 0 });

		// Camera top
		int camera = ecs.CreateEntity();
		camTrs = ecs.AddComponent<TransformComponent>(camera);
		camTrs->SetWorldPosition({ 0, 4, -3 });
		camTrs->SetRotation(0, Maths::Pi / 4.0f, 0);
		CameraComponent* camComp = ecs.AddComponent<CameraComponent>(camera);
		camComp->SetMainCamera(true);

		// First orbit parameters
		{
			XMFLOAT3 center = cubeTrs->GetWorldPosition();
			XMFLOAT3 camPos = camTrs->GetWorldPosition();
			float dx = camPos.x - center.x;
			float dz = camPos.z - center.z;
			m_OrbitRadius = std::sqrt(dx*dx + dz*dz);
			m_OrbitHeight = camPos.y - center.y;
			m_OrbitAngle = std::atan2(dx, dz);
		}
	}

	float m_time = 0.0f;
	virtual void OnUpdate() override
	{
		if (InputsManager::GetKeyDown(Keyboard::NUMPAD_ADD))
			SceneManager::Get()->GoOnScene(1);
		if (InputsManager::GetKeyDown(Keyboard::NUMPAD_SUBTRACT))
			SceneManager::Get()->GoOnScene(0);

		float dt = ECS::GetDeltaTime();

		/// Move the cube
		m_time += dt;
		movingCubeVel->SetVelocity(cos(m_time));

		/// Orbit Update
		m_OrbitAngle += m_OrbitSpeed * dt;
		XMFLOAT3 center = cubeTrs->GetWorldPosition();

		// Camera position update
		float x = center.x + std::sin(m_OrbitAngle) * m_OrbitRadius;
		float z = center.z + std::cos(m_OrbitAngle) * m_OrbitRadius;
		float y = center.y + m_OrbitHeight;
		camTrs->SetWorldPosition({ x, y, z });

		// Rotation update to look at the center
		// Look to center
		XMFLOAT3 camPos = camTrs->GetWorldPosition();
		float dirX = center.x - camPos.x;
		float dirY = center.y - camPos.y;
		float dirZ = center.z - camPos.z;

		float horizDist = std::sqrt(dirX * dirX + dirZ * dirZ);
		float yaw = std::atan2(dirX, dirZ);
		float pitch = std::atan2(-dirY, horizDist);

		camTrs->SetRotation(yaw, pitch, 0.0f);
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
	SceneManager::Get()->CreateScene<SceneDemo>();

	ECSSettings settings = ECSSettings();
	settings.m_WinHeight = 1080;
	settings.m_WinWidth = 1920;
	//settings.m_ShowFPSInWindowName = false;

	ecs.Run(hInstance, settings);
}