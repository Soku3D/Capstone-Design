#include "pch.h"
#include "Timer.h"

namespace soku {
	Timer::Timer()
		:m_currTime(0),
		m_prevTime(0),
		m_deltaTime(0),
		m_secondsPerCount(0.0)
	{
		__int64 countPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countPerSec);
		m_secondsPerCount = 1.0 / countPerSec;
	}
	void Timer::Start()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_currTime = currTime;
		m_prevTime = currTime;
	}
	void Timer::Tick()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_currTime = currTime;
		m_deltaTime = m_currTime - m_prevTime;

		m_prevTime = currTime;
	}
}