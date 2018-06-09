// GameTimer.cpp

#include "GameTimer.h"

#include <Windows.h>

//--------------------------------------------------------------------------------------
//  This is the GameTimer for the engine.  I grabbed most of this code from 3D Game
//	Programming with DirectX12 by Frank Luna.  This is a very basic timer that is used to
//	get the time that has alloted since the last frame.  Call GetDeltaTime() to get the
//	time that's elapsed this frame.
//		m_secondsPerCount: is the amount of time in seconds per performance count
//		m_deltaTime: the time that has elapsed since the last frame
//		m_prevTime: the pervious performance counter time
//		m_currTime: the current performance counter time
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
//	Construct the game timer, set all the values to default values.  The only work we do
//	here is get the frequency of the performance counter.  We store this value and use it
//	for the rest of the game.  The frequency is consistent across all processors.
//--------------------------------------------------------------------------------------
GameTimer::GameTimer()
	: m_secondsPerCount(0.0)
	, m_deltaTime(-1.0)
	, m_currTime(0)
	, m_prevTime(0)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

//--------------------------------------------------------------------------------------
//	This ticks the game timer.  We get the current value of the performance counter and
//	determine the time that has elapsed this frame by getting the difference from the
//	last frames performance counter result.
//--------------------------------------------------------------------------------------
void GameTimer::Tick()
{
	// Get the time this frame
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// Time difference between this frame and the previous
	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

	// Perpare for next frame
	m_prevTime = m_currTime;

	if (m_deltaTime < 0)
	{
		m_deltaTime = 0.0;
	}
}

//--------------------------------------------------------------------------------------
//	Reset the timer
//--------------------------------------------------------------------------------------
void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_prevTime = currTime;
}
