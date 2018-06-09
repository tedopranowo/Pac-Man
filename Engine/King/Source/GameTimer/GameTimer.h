// GameTimer.h

#pragma once

class GameTimer
{
private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_prevTime;
	__int64 m_currTime;

public:
	GameTimer();

	const __int64 GetTime() const { return m_currTime; }
	//const double GetTotalTime() const { }
	const double GetDeltaTime() const { return m_deltaTime; }

	void Tick();
	void Reset();

};