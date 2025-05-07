#pragma once
#include "Engine/Component.h"

#include "Utils/Math.h"

namespace VeiM
{
	class CORE_API SceneComponent : public Component
	{
		DECLARE_CLASS(SceneComponent, Component);
	public:

		glm::vec3 GetForwardVector();
		glm::vec3 GetUpVector();
		glm::vec3 GetRightVector();

		SceneComponent* GetParent() const;
		bool AttachToComponent(SceneComponent* newParent);
		void DetachFromComponent();

	private:

		ObjectPtr<SceneComponent> m_Parent;

	};
}