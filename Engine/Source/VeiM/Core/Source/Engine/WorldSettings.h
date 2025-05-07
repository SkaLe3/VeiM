#pragma once
#include "CoreDefines.h"
#include <glm/glm.hpp>

// WorldSettings.h
namespace VeiM
{
	class WorldSettings
	{
	public:
		float GetTimeScale() const
		{
			return m_TimeScale;
		}

		float ClampDeltaTime(float deltaTime) const
		{
			float const timeScale = GetTimeScale();
			float const minFrameTime = m_MinFrameTimeUnscaled * timeScale;
			float const maxFrameTime = m_MaxFrameTimeUnscaled * timeScale;
			return glm::clamp(deltaTime, minFrameTime, maxFrameTime);
		}
	
		bool GetPaused() { return m_bPaused; }
		void SetPasued(bool bPause) { m_bPaused = bPause; }

		void NotifyBeginPlay();

	private:
		float m_TimeScale;
		float m_MinFrameTimeUnscaled;
		float m_MaxFrameTimeUnscaled;
		bool m_bPaused;
	};



}