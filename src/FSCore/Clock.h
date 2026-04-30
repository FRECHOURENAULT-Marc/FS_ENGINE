#pragma once
namespace FSC
{

	class Clock
	{
	public:
		Clock();

		float TotalTime()const;
		float DeltaTime()const;

		bool IsStopped();

		void SetSpeed(double timeSpeed);
		double GetSpeed();

		void Reset();
		void Start();
		void Stop();
		void Tick();

	private:
		double m_SecondsPerCount;
		double m_DeltaTime;

		double m_TimeSpeed = 1.0f;

		__int64 m_BaseTime;
		__int64 m_PausedTime;
		__int64 m_StopTime;
		__int64 m_PrevTime;
		__int64 m_CurrTime;

		bool m_IsStopped;
	};

}
