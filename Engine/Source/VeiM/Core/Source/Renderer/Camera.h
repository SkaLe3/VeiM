#pragma once
#include "CoreDefines.h"

#include <glm/glm.hpp>

namespace VeiM
{
	class Camera
	{
	public:
		enum class ProjectionType : uint32
		{
			Perspective,
			Orthographic,
			Frustum
		};

		enum class FOVAxis : uint32
		{
			Horizontal,
			Vertical
		};

	public:
		Camera();
		
		virtual void Update(float deltaTime);

		void SetPerspective(float fov, float aspect, float nearZ, float farZ);
		void SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

		void SetProjectionType(ProjectionType projectionType);
		void SetFovAxis(FOVAxis fovAxis);
		void SetFov(float fov);
		void SetNearClip(float nearZ);
		void SetFarClip(float farZ);
		void SetOrthoHeight(float size);

		void SetViewport(float width, float height);

		ProjectionType GetProjectionType() const;
		FOVAxis GetFovAxis() const;
		float GetFov() const;
		float GetNearClip() const;
		float GetFarClip() const;
		float GetAspect() const;
		float GetOrthoHeight() const;

		const glm::mat4& GetProjectionMatrix() { return m_Projection; }

	protected:
		glm::mat4 m_Projection;

		float m_FOV;
		float m_AspectRatio;
		float m_NearClip;
		float m_FarClip;
		float m_Size;

		float m_ViewportWidth;
		float m_ViewportHeight;

		bool m_bIsProjectionDirty;

		ProjectionType m_Mode = ProjectionType::Perspective;
		FOVAxis m_FOVAxis = FOVAxis::Vertical;
	};
}