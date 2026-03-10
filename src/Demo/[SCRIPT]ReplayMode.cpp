#include "pch.h"

void ReplayMode::OnStart()
{
	Reset();
	SetRecording(true);
};

void ReplayMode::OnUpdate()
{
	if(m_isRecording)
		RecordFrame();

	if(m_isPlaying)
		PlayFrame();

	m_PlayingSpeed = 1.0f;
}

void ReplayMode::RecordFrame()
{
	//std::cout << "Recording frame " << m_Frames.size() << std::endl;

	ECS& ecs = ECS::Get();

	std::vector<TransformComponent>& transforms = ecs.GetAllFromType<TransformComponent>();

	FrameData frameData;

	for (auto& trs : transforms)
	{
		if (trs.IsActive() == false)
			continue;

		frameData.m_Tranforms.push_back(trs.GetLocalTransform());
		frameData.m_IDs.push_back(trs.GetID());
	}

	m_Frames.push_back(frameData);
}

void ReplayMode::PlayFrame()
{
	//std::cout << "Playing frame " << m_currentFrame << std::endl;

	ECS& ecs = ECS::Get();

	FrameData& frameData = m_Frames[m_currentFrame];

	for (int i = 0; i < frameData.m_IDs.size(); i++)
	{
		int currentID = frameData.m_IDs[i];

		TransformComponent* trsComp = ecs.GetComponent<TransformComponent>(currentID);
		FS_3DObject* object = ecs.GetComponent<MeshComponent>(currentID)->Get3DObject();

		if(object != nullptr && object->IsActive() == false)
			object->SetActive(true);

		trsComp->GetLocalTransform() = frameData.m_Tranforms[i];
	}

	m_currentFrame += m_PlayingSpeed;
	if (m_currentFrame >= m_Frames.size() || m_currentFrame == 0)
	{
		m_currentFrame = 0;

		ECS& ecs = ECS::Get();

		std::vector<MeshComponent>& meshs = ecs.GetAllFromType<MeshComponent>();

		for (auto& mesh : meshs)
		{
			FS_3DObject* object = mesh.Get3DObject();
			if (object == nullptr)
				continue;

			object->SetActive(false);
		}
	}
}

void ReplayMode::SetRecording(bool isPlaying)
{
	m_isRecording = isPlaying;
	m_isPlaying = false;
}

void ReplayMode::SetPlaying(bool isPlaying)
{
	m_isPlaying = isPlaying;
	m_isRecording = false;

	if (!m_isPlaying)
		return;

	m_currentFrame = 0;

	ECS& ecs = ECS::Get();

	std::vector<MeshComponent>& meshs = ecs.GetAllFromType<MeshComponent>();

	for (auto& mesh : meshs)
	{
		FS_3DObject* object = mesh.Get3DObject();
		if(object == nullptr)
			continue;

		object->SetActive(false);
	}
}

void ReplayMode::Reset()
{
	m_Frames.clear();
	m_currentFrame = 0;
	m_isRecording = false;
	m_isPlaying = false;
}
