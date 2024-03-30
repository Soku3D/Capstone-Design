#pragma once

namespace soku {
	class Timer {
	public:
		Timer();
		void Start();
		double m_secondsPerCount;
		void Tick();
		double GetDeltaTime() const { return (m_secondsPerCount * m_deltaTime); }
	private:
		__int64 m_currTime;
		__int64 m_deltaTime;
		__int64 m_prevTime;

	};
}
