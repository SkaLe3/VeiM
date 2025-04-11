#pragma once
#include "CoreDefines.h"
#include "Renderer/Camera.h"

namespace VeiM
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera();

		virtual void Update(float deltaTime) override;

		const glm::mat4& GetViewMatrix() const { return m_View; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_View; }

		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;
		glm::vec3 GetForwardVector() const;
		glm::vec3 GetRightVector() const;
		glm::vec3 GetUpVector() const;

		float& MovementSpeed() { return m_MovementSpeed; }
		float& Sensitivity() { return m_MouseSensitivity; }

		void SetPosition(const glm::vec3& position);

		void InputKey(float deltaTime, const glm::vec3& direction);
		void InputMouse(const glm::vec2& delta);
		void InputScroll(const glm::vec2& delta);
		void InputPan(const glm::vec2& delta);

		float PanSpeed() const;

	private:
		glm::mat4 m_View;
		glm::vec3 m_Position;

		float m_Pitch;
		float m_Yaw;
		float m_MovementSpeed = 4.f;
		float m_MouseSensitivity = 0.1f;
	};
}