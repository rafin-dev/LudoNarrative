#pragma once

namespace Ludo {

	class TimeStep
	{
	public:
		TimeStep(float time)
			: m_Time(time)
		{
		}

		operator float() const { return m_Time; }

		float GetSeconds() { return m_Time; }
		float GetMiliseconds() { return m_Time * 1000.0f; }

	private:
		float m_Time;
	};

}