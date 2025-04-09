#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

namespace VeiM
{


	Camera::Camera()
	{
		SetPerspective(45.f, 1.778f, 0.05f, 4000.f);
	}

	void Camera::SetPerspective(float fov, float aspect, float nearZ, float farZ)
	{
		if (fov == m_FOV && aspect == m_AspectRatio && nearZ == m_NearClip && farZ == m_FarClip && m_Mode == ProjectionType::Perspective)
		{
			return;
		}

		m_Mode = ProjectionType::Perspective;
		m_NearClip = nearZ;
		m_FarClip = farZ;
		m_FOV = fov;
		m_AspectRatio = aspect;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		m_bIsProjectionDirty = false;
	}

	void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
		m_Mode = ProjectionType::Orthographic;
		m_NearClip = nearZ;
		m_FarClip = farZ;
		m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);
		m_bIsProjectionDirty = false;
	}

	void Camera::SetProjectionType(ProjectionType projectionType)
	{
		m_Mode = projectionType;
		m_bIsProjectionDirty = true;
	}

	void Camera::SetFovAxis(FOVAxis fovAxis)
	{
		m_FOVAxis = fovAxis;
		m_bIsProjectionDirty = true;
	}

	void Camera::SetFov(float fov)
	{
		m_FOV = fov;
		m_bIsProjectionDirty = true;
	}

	void Camera::SetNearClip(float nearZ)
	{
		m_NearClip = nearZ;
		m_bIsProjectionDirty = true;
	}

	void Camera::SetFarClip(float farZ)
	{
		m_FarClip = farZ;
		m_bIsProjectionDirty = true;
	}

	void Camera::SetOrthoHeight(float size)
	{
		m_Size = size;
		m_bIsProjectionDirty = true;
	}

	void Camera::SetViewport(float width, float height)
	{
		if (width == m_ViewportHeight && height == m_ViewportHeight)
			return;
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_bIsProjectionDirty = true;
	}

	Camera::ProjectionType Camera::GetProjectionType() const
	{
		return m_Mode;
	}

	Camera::FOVAxis Camera::GetFovAxis() const
	{
		return m_FOVAxis;
	}

	float Camera::GetFov() const
	{
		return m_FOV;
	}

	float Camera::GetNearClip() const
	{
		return m_NearClip;
	}

	float Camera::GetFarClip() const
	{
		return m_FarClip;
	}

	float Camera::GetAspect() const
	{
		return m_AspectRatio;
	}

	float Camera::GetOrthoHeight() const
	{
		return m_Size;
	}

	void Camera::Update(float deltaTime)
	{
		if (m_bIsProjectionDirty)
		{
			switch (m_Mode)
			{
			case Camera::ProjectionType::Perspective:
			{
				float fovRadians = 0.0f;
				fovRadians = glm::radians(m_FOV);
				if (m_FOVAxis == FOVAxis::Horizontal)
				{
					fovRadians = 2.0f * glm::atan(glm::tan(fovRadians / 2.0f) / m_AspectRatio);
				}

				m_Projection = glm::perspective(fovRadians, m_AspectRatio, m_NearClip, m_FarClip);
				break;
			}
			case Camera::ProjectionType::Orthographic:
			{
				float orthoHeight = m_Size;
				float orthoWidth = m_Size;

				if (m_FOVAxis == FOVAxis::Vertical)
				{
					orthoHeight = m_Size;
					orthoWidth = orthoHeight * m_AspectRatio;
				}
				else // Horizontal
				{
					orthoWidth = m_Size;
					orthoHeight = orthoWidth / m_AspectRatio;
				}

				float left = -orthoWidth / 2.0f;
				float right = orthoWidth / 2.0f;
				float bottom = -orthoHeight / 2.0f;
				float top = orthoHeight / 2.0f;

				m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);
				break;
			}
			case Camera::ProjectionType::Frustum:
				break;
			default:
				break;
			}
			m_bIsProjectionDirty = false;
		}
	}

}