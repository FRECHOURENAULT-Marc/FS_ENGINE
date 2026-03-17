#include "pch.h"

XMFLOAT3 CatmullRom(const XMFLOAT3& p0,const XMFLOAT3& p1,const XMFLOAT3& p2,const XMFLOAT3& p3,float t)
{
    XMVECTOR P0 = XMLoadFloat3(&p0);
    XMVECTOR P1 = XMLoadFloat3(&p1);
    XMVECTOR P2 = XMLoadFloat3(&p2);
    XMVECTOR P3 = XMLoadFloat3(&p3);

    XMVECTOR result = XMVectorCatmullRom(P0, P1, P2, P3, t);

    XMFLOAT3 out;
    XMStoreFloat3(&out, result);
    return out;
}

void GeneratingRail::OnStart()
{
    std::srand((unsigned int)std::time(nullptr));

    std::vector<XMFLOAT3> PosGeneration;
    PosGeneration.push_back({ 0,0,0 });

    for (int i = 0; i < 10; ++i)
    {
        // GEN POS (centre)
        float x = ((float)std::rand() / RAND_MAX) * 40.0f - 20.0f;
        float y = 0;
        float z = i * 10;

        PosGeneration.push_back({ x, y, z });

        // PARAMETRES DU CERCLE
        float radius = ((float)std::rand() / RAND_MAX) * 25.0f - 5.0f;
        float angle = ((float)std::rand() / RAND_MAX) * 2.0f * 3.1415926f;

        float circleX = x + std::cos(angle) * radius;
        float circleY = y + std::sin(angle) * radius;

        //TARGET GEN
        ECS& ecs = ECS::Get();
        Target* targ = new Target;

        targ->Create({ circleX, abs(circleY) ,z });

        m_targets.push_back(targ);
    }

    // ===== SAMPLE DE LA COURBE =====
    const int samplePerSegment = 20;

    for (size_t i = 1; i < PosGeneration.size() - 2; ++i)
    {
        for (int j = 0; j <= samplePerSegment; ++j)
        {
            float t = static_cast<float>(j) / samplePerSegment;

            m_curvePoints.push_back(
                CatmullRom( PosGeneration[i - 1], PosGeneration[i], PosGeneration[i + 1], PosGeneration[i + 2], t));
        }
    }

    // ===== CREATION DU RAIL =====
    GeometryData* RAIL = new GeometryData;

    float halfWidth = 1.0f;
    float halfHeight = 1.0f;

    // ===== VERTICES =====

    for (auto Point : m_curvePoints)
    { 
        Vertex v0 = { {-halfWidth + Point.x, -halfHeight + Point.y, Point.z}, {0,0}, {0,0,-1}, {1,0,0} }; //GAUCHE BAS

        Vertex v1 = { {-halfWidth + Point.x,  halfHeight + Point.y, Point.z}, {0,0}, {0,0,-1}, {1,0,0} }; //GAUCHE HAUT

        Vertex v2 = { { halfWidth + Point.x, -halfHeight + Point.y, Point.z}, {0,0}, {0,0,-1}, {1,0,0} }; //DROITE BAS

        Vertex v3 = { { halfWidth + Point.x,  halfHeight + Point.y, Point.z}, {0,0}, {0,0,-1}, {1,0,0} }; //DROITE HAUT

        RAIL->vertices.push_back(v0);
        RAIL->vertices.push_back(v1);
        RAIL->vertices.push_back(v2);
        RAIL->vertices.push_back(v3);
    }

    for (size_t i = 0; i < m_curvePoints.size() - 1; i++)
    {
        int base = static_cast<int>(i) * 4;
        int nextBase = base + 4;

        // ===== FACE GAUCHE =====
        RAIL->indices.push_back(base + 0); 
        RAIL->indices.push_back(nextBase + 0); 
        RAIL->indices.push_back(nextBase + 1);

        RAIL->indices.push_back(base + 0);  
        RAIL->indices.push_back(nextBase + 1);
        RAIL->indices.push_back(base + 1);

        // ===== FACE DROITE =====
        RAIL->indices.push_back(base + 2); 
        RAIL->indices.push_back(nextBase + 3);
        RAIL->indices.push_back(nextBase + 2);

        RAIL->indices.push_back(base + 2); 
        RAIL->indices.push_back(base + 3);   
        RAIL->indices.push_back(nextBase + 3); 

        // ===== FACE BAS =====
        RAIL->indices.push_back(base + 0); 
        RAIL->indices.push_back(nextBase + 2);
        RAIL->indices.push_back(nextBase + 0); 

        RAIL->indices.push_back(base + 0); 
        RAIL->indices.push_back(base + 2);  
        RAIL->indices.push_back(nextBase + 2); 

        // ===== FACE HAUT =====
        RAIL->indices.push_back(base + 1); 
        RAIL->indices.push_back(nextBase + 1); 
        RAIL->indices.push_back(nextBase + 3); 

        RAIL->indices.push_back(base + 1);  
        RAIL->indices.push_back(nextBase + 3);
        RAIL->indices.push_back(base + 3);
    }

    // ===== BUILD GEOMETRY =====

    FS_Command* cmdList = FS_Device::CommandList();

    ThrowIfFailed(cmdList->mCommandList->Reset(cmdList->mDirectCmdListAlloc.Get(), nullptr));

    m_newGeo = GeometryManager::Get()->BuildGeometry(RAIL);

    cmdList->ExecuteCommands();

    cmdList->FlushCommandQueue();

    // ===== ECS =====
    m_ecs = &ECS::Get();
    int entityID = m_ecs->CreateEntity();

    TransformComponent* trs = m_ecs->AddComponent<TransformComponent>(entityID);
    MeshComponent* mesh = m_ecs->AddComponent<MeshComponent>(entityID);

    mesh->SetGeometry(m_newGeo);

    mesh->SetMaterial(MATERIAL_DEFAULT_COLOR_NAME);

    mesh->SetColor({ 0.3f, 0.3f, 0.3f, 1.0f });
}

void GeneratingRail::OnUpdate()
{
    //return;

    int armID = m_player->GetEntityID();
	float dt = m_ecs->GetDeltaTime();
	float speed = m_player->GetSpeed();

    auto armTrs = m_ecs->GetComponent<TransformComponent>(armID);
    XMFLOAT3 pos = armTrs->GetWorldPosition();

    if (!m_curvePoints.empty())
    {
        XMFLOAT3 target = m_curvePoints[m_currentPointIndex];

        XMVECTOR currentPos = XMLoadFloat3(&pos);
        XMVECTOR targetPos = XMLoadFloat3(&target);

        XMVECTOR direction = XMVectorSubtract(targetPos, currentPos);
        float distance = XMVectorGetX(XMVector3Length(direction));

        if (distance < 0.1f)
        {
            m_currentPointIndex++;

            if (m_currentPointIndex >= m_curvePoints.size())
            {
                m_currentPointIndex = m_curvePoints.size() - 1;
				//m_gm->StartReplay();
            }
                
        }
        else
        {
            direction = XMVector3Normalize(direction);

            XMFLOAT3 dirFloat;
            XMStoreFloat3(&dirFloat, direction);

            float angle = atan2f(dirFloat.x, dirFloat.z);

            //armTrs->Rotate(0, angle, 0 ); // TO LOOK 

            currentPos += direction * speed * dt;

            XMStoreFloat3(&pos, currentPos);
            armTrs->SetWorldPosition(pos);
        }
    }

    armTrs->SetWorldPosition(pos);
}