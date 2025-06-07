#include "EditorViewportClient.h"

namespace VeiM
{

	void EditorViewportClient::SaveCameraInfo()
	{
		LastEditorViewLocation = GetViewLocation();
		LastEditorViewRotation = GetViewRotation();
	}

	const glm::vec3& EditorViewportClient::GetViewLocation() const
	{
		return GetViewTransform().GetLocation();
	}

	const glm::vec3& EditorViewportClient::GetViewRotation() const
	{
		return GetViewTransform().GetRotation();
	}

	EditorCameraTransform& EditorViewportClient::GetViewTransform()
	{
		return IsPerspective() ? ViewTransformPerspective : ViewTransformOrthographic;
	}

	const EditorCameraTransform& EditorViewportClient::GetViewTransform() const
	{
		return IsPerspective() ? ViewTransformPerspective : ViewTransformOrthographic;
	}

	bool EditorViewportClient::IsPerspective() const
	{
		return Projection == Camera::ProjectionType::Perspective;
	}

	Camera::ProjectionType EditorViewportClient::GetProjectionType() const
	{
		return Projection;
	}

}

