#pragma once
#include "CoreDefines.h"

#include <glm/glm.hpp>

namespace VeiM
{
	struct CORE_API EditorCameraTransform
	{
	public:
		EditorCameraTransform();

		void SetLocation(const glm::vec3& location);
		void SetRotation(const glm::vec3& rotation);
		void SetOrthoSize(float size);
		
		FORCEINLINE const glm::vec3& GetLocation() const { return m_ViewLocation; }
		FORCEINLINE const glm::vec3& GetRotation() const { return m_ViewRotation; }
		FORCEINLINE const float GetOrthoSize() const { return m_OrthoSize; }

	private:
		glm::vec3 m_ViewLocation;
		glm::vec3 m_ViewRotation;
		float m_OrthoSize;
		// Add lookAt
	};
}