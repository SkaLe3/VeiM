#include "EditorCameraTransform.h"
#include "Utils/Math.h"

namespace VeiM
{

	EditorCameraTransform::EditorCameraTransform()
		: m_ViewLocation(Math::Utils::ZeroVector)
		, m_ViewRotation(Math::Utils::ZeroVector)
		, m_OrthoSize(10.f)
	{

	}

	void EditorCameraTransform::SetLocation(const glm::vec3& location)
	{
		m_ViewLocation = location;
	}

	void EditorCameraTransform::SetRotation(const glm::vec3& rotation)
	{
		m_ViewRotation = rotation;
	}

	void EditorCameraTransform::SetOrthoSize(float size)
	{
		m_OrthoSize = size;
	}

}

