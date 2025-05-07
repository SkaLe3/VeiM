#include "SceneComponent.h"

namespace VeiM
{
	IMPLEMENT_CLASS(SceneComponent);


	void SceneComponent::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		// TODO: Reigster
	}

	glm::vec3 SceneComponent::GetForwardVector()
	{
		return glm::vec3(1.0);
	}

	glm::vec3 SceneComponent::GetUpVector()
	{
		return glm::vec3(1.0);
	}

	glm::vec3 SceneComponent::GetRightVector()
	{
		return glm::vec3(1.0);
	}

	SceneComponent* SceneComponent::GetParent() const
	{
		return m_Parent.Get();
	}

	bool SceneComponent::AttachToComponent(SceneComponent* newParent)
	{
		return true;
		// TODO: Implement
	}

	void SceneComponent::DetachFromComponent()
	{
		// TODO: Implement
	}

}

