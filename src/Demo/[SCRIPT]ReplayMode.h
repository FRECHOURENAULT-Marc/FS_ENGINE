#pragma once

struct FrameData
{
	std::vector<Transform> m_Tranforms;
	std::vector<int> m_IDs;
};

DECLARE_SCRIPT(ReplayMode)

std::vector<FrameData> m_Frames;

int m_currentFrame = 0;
bool m_isRecording = false;
bool m_isPlaying = false;

float m_PlayingSpeed = 1.0f;

virtual void OnStart();
virtual void OnUpdate();

void Forcing() { OnUpdate(); };
void SetRecording(bool isRecording);
void SetPlaying(bool isPlaying);
bool IsPlaying() { return m_isPlaying; }
bool IsRecording() { return m_isRecording; }
void Reset();
void RecordFrame();
void PlayFrame();
void SetPlayingSpeed(float speed) { m_PlayingSpeed = speed; }

END_SCRIPT


