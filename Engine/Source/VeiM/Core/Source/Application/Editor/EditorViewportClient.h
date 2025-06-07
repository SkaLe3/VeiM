#pragma once
#include "CoreDefines.h"
#include "Engine/ViewportClient.h"

#include "Application/Editor/EditorCameraTransform.h"
#include "Renderer/Camera.h"

#include <glm/glm.hpp>

namespace VeiM
{
	class EditorViewportClient : public ViewportClient
	{
	public:
		void SaveCameraInfo();
		const glm::vec3& GetViewLocation() const;
		const glm::vec3& GetViewRotation() const;
		EditorCameraTransform& GetViewTransform();
		const EditorCameraTransform& GetViewTransform() const;

		bool IsPerspective() const;
		Camera::ProjectionType GetProjectionType() const;
	public:
		EditorCameraTransform ViewTransformPerspective;
		EditorCameraTransform ViewTransformOrthographic;
		Camera::ProjectionType Projection;
		glm::vec3 LastEditorViewLocation;
		glm::vec3 LastEditorViewRotation;
	};
}