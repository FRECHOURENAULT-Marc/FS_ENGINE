#include "pch.h"

namespace FSC
{

	Clock::Clock() : m_SecondsPerCount(0.0), m_DeltaTime(-1.0), m_BaseTime(0), m_PausedTime(0), m_PrevTime(0), m_CurrTime(0), m_IsStopped(false)
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		m_SecondsPerCount = 1.0 / (double)countsPerSec;
		Reset();
	}

	float Clock::TotalTime()const
	{

		if (m_IsStopped)
		{
			return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
		}

		else
		{
			return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
		}
	}

	float Clock::DeltaTime()const
	{
		return (float)m_DeltaTime;
	}

	bool Clock::IsStopped()
	{
		return m_IsStopped;
	}

	void Clock::SetSpeed(double timeSpeed)
	{
		m_TimeSpeed = timeSpeed;
	}

	double Clock::GetSpeed()
	{
		return m_TimeSpeed;
	}

	void Clock::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_BaseTime = currTime;
		m_PrevTime = currTime;
		m_StopTime = 0;
		m_IsStopped = false;
	}

	void Clock::Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (m_IsStopped)
		{
			m_PausedTime += (startTime - m_StopTime);

			m_PrevTime = startTime;
			m_StopTime = 0;
			m_IsStopped = false;
		}
	}

	void Clock::Stop()
	{
		if (!m_IsStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			m_StopTime = currTime;
			m_IsStopped = true;
		}
	}

	void Clock::Tick()
	{
		if (m_IsStopped)
		{
			m_DeltaTime = 0.0;
			return;
		}

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_CurrTime = currTime;

		m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount * m_TimeSpeed;

		m_PrevTime = m_CurrTime;

		if (m_DeltaTime < 0.0)
		{
			m_DeltaTime = 0.0;
		}
	}

}