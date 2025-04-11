#include "EditorCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace VeiM
{

	EditorCamera::EditorCamera() : Camera()
	{
		m_Yaw = 0.0f;
		m_Pitch = 0.0f;
		m_Position = { 0.0f, 0.0f, 0.0f };
	}

	void EditorCamera::Update(float deltaTime)
	{
		Camera::Update(deltaTime);

		glm::quat orientation = GetOrientation();
		m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_View = glm::inverse(m_View);	
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(glm::radians(-m_Pitch), glm::radians(-m_Yaw), 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.f, 0.f, -1.f));
	}

	glm::vec3 EditorCamera::GetRightVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.f, 0.f, 0.f));
	}

	glm::vec3 EditorCamera::GetUpVector() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.f, 1.f, 0.f));
	}

	void EditorCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
	}

	void EditorCamera::InputKey(float deltaTime, const glm::vec3& direction)
	{
		float speed = m_MovementSpeed * deltaTime;
		m_Position = m_Position + direction * speed;
	}

	void EditorCamera::InputMouse(const glm::vec2& delta)
	{
		glm::vec2 movement = delta * m_MouseSensitivity;
		m_Yaw += movement.x;
		m_Pitch += movement.y;

		if (m_Pitch > 90.0f)  m_Pitch = 90.0f;
		if (m_Pitch < -90.0f) m_Pitch = -90.0f;
	}


	void EditorCamera::InputScroll(const glm::vec2& delta)
	{
		m_MovementSpeed = glm::clamp(m_MovementSpeed + delta.y * 1.0f, 1.0f, 25.0f);
	}

	void EditorCamera::InputPan(const glm::vec2& delta)
	{
		float panSpeed = PanSpeed();
		m_Position += -GetRightVector() * delta.x * panSpeed * 0.01f * m_MovementSpeed;
		m_Position += GetUpVector() * delta.y * panSpeed * 0.01f * m_MovementSpeed;
	}

	float EditorCamera::PanSpeed() const
	{
		float x = glm::min(m_ViewportHeight / 1000.f, 2.4f);
		float factor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		return factor;
	}

}

