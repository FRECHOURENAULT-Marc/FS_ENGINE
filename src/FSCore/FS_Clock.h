#pragma once

class FS_Clock
{
public:
	FS_Clock();

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
	double mSecondsPerCount;
	double mDeltaTime;

	double mTimeSpeed = 1.0f;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};
