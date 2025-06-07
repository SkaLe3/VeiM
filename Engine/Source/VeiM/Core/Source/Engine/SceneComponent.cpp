#include "SceneComponent.h"
#include "Engine/Entity.h"
#include "Misc/Globals.h"

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

	void SceneComponent::GetAllParents(std::vector<SceneComponent*>& outParents) const
	{
		outParents.clear();
		SceneComponent* parent = GetParent();
		while (parent != nullptr)
		{
			outParents.push_back(parent);
			parent = parent->GetParent();
		}
	}

	SceneComponent* SceneComponent::GetChildComponent(int32 childIdx) const
	{
		if (childIdx < 0)
		{
			VM_CORE_WARN("SceneComponent::GetChild called with a negative index: {0}", childIdx);
			return nullptr;
		}
		const std::vector<ObjectPtr<SceneComponent>>& children = GetAttachChildren();
		if (childIdx >= children.size())
		{
			VM_CORE_WARN("SceneComponent::GetChild called with an out of range index: {0}", childIdx);
			return nullptr;
		}
		return children[childIdx].Get();

	}

	void SceneComponent::GetChildrenComponents(std::vector<SceneComponent*> outComps, bool bDoChildren /*= true*/) const
	{
		outComps.clear();
		if (bDoChildren)
		{
			AppendAllChildren(outComps);
		}
		else
		{
			const std::vector<ObjectPtr<SceneComponent>> children = GetAttachChildren();
			outComps.reserve(children.size());
			for (ObjectPtr<SceneComponent> child : children)
			{
				if (child)
				{
					outComps.push_back(child.Get());
				}
			}

		}
	}

	bool SceneComponent::AttachToComponent(SceneComponent* newParent, const AttachmentTransformRules& attachRules)
	{
		if (!newParent)
		{
			return false;
		}

		const bool bInParentChildren = std::count(m_Parent->m_AttachChildren.begin(), m_Parent->m_AttachChildren.end(), this) > 0;
		const bool bSameParent = (newParent == m_Parent.Get());
		if (bInParentChildren && bSameParent)
		{
			return true;
		}

		if (newParent == this)
		{
			VM_CORE_WARN("Component cannot be attached to itslef");
			return false;
		}

		Entity* ownerEntity = GetOwner();
		Entity* parentEntity = newParent->GetOwner();
		if (ownerEntity == parentEntity && ownerEntity && ownerEntity->GetRootComponent() == this)
		{
			VM_CORE_WARN("Root component cannot be attached to other components in the same entity");
			return false;
		}
		if (newParent->IsAttachedTo(this))
		{
			VM_CORE_WARN("Detected cycle. Component already attachd to this");
			return false;
		}

		// TODO: Add mobility check here to not attach static to dynamic
		// TODO: Check for prototype matching (both CDO or both not)

		AttachmentTransformRules detachmentRules(attachRules);
		// detach
		if (!bSameParent || !bInParentChildren || IsRegistered())
		{
			DetachFromComponent(detachmentRules);
		}

		m_Parent = newParent;
		newParent->m_AttachChildren.emplace_back(this);
		Transform parentWorldTransform = GetParent()->GetTransform();
		Transform myRelativeTransform = GetTransform().GetRelative(parentWorldTransform);

		switch (attachRules.LocationRule)
		{
		case EAttachmentRule::Relative:
			break;
		case EAttachmentRule::World:
			if (IsAbsoluteLocation())
			{
				m_RelativeLocation = GetTransform().Translation;
			}
			else
			{
				m_RelativeLocation = myRelativeTransform.Translation;
			}
			break;
		}

		switch (attachRules.RotationRule)
		{
		case EAttachmentRule::Relative:
			break;
		case EAttachmentRule::World:
			if (IsAbsoluteRotation())
			{
				m_RelativeRotation = GetRotation();
				m_RelativeRotationQuat = GetRotation();
			}
			else
			{
				m_RelativeRotation = glm::degrees(glm::eulerAngles(myRelativeTransform.Rotation));
				m_RelativeRotationQuat = myRelativeTransform.Rotation;
			}
			break;
		}

		switch (attachRules.ScaleRule)
		{
		case EAttachmentRule::Relative:
			break;
		case EAttachmentRule::World:
			if (IsAbsoluteScale())
			{
				m_RelativeScale = GetTransform().Scale;
			}
			else
			{
				m_RelativeScale = myRelativeTransform.Scale;
			}
			break;
		}

		UpdateWorldTransform();

		return true;

	}

	void SceneComponent::DetachFromComponent(const AttachmentTransformRules detachRules)
	{
		if (!GetParent())
		{
			return;
		}

		Entity* owningEntity = GetOwner();
		auto& children = GetParent()->m_AttachChildren;
		children.erase(std::remove(children.begin(), children.end(), this), children.end());
		m_Parent = nullptr;

		switch (detachRules.LocationRule)
		{
		case EAttachmentRule::Relative:
			break;
		case EAttachmentRule::World:
			m_RelativeLocation = GetTransform().Translation;
			break;
		}

		switch (detachRules.RotationRule)
		{
		case EAttachmentRule::Relative:
			break;
		case EAttachmentRule::World:
			m_RelativeRotation = GetRotation();
			m_RelativeRotationQuat = GetRotation();
			break;
		}

		switch (detachRules.ScaleRule)
		{
		case EAttachmentRule::Relative:
			break;
		case EAttachmentRule::World:
			m_RelativeScale = GetScale();
			break;
		}

		UpdateWorldTransform();

	}

	const std::vector<VeiM::ObjectPtr<VeiM::SceneComponent>>& SceneComponent::GetAttachChildren() const
	{
		return m_AttachChildren;
	}

	void SceneComponent::SetupAttachment(SceneComponent* inParent)
	{
		if (inParent != m_Parent)
		{
			if (m_bRegistered)
			{
				VM_ASSERT(false, "SetupAttachment used after registration. Use AttachToComponent instead. Owner[{}], inParent[{}]", GetOwner() ? GetOwner()->GetName() : "None", inParent->GetName());
				return;
			}
			if (inParent == this)
			{
				VM_ASSERT(false, "Cannot attach component to itself");
				return;
			}
			if (inParent && inParent->IsAttachedTo(this))
			{
				VM_ASSERT(false, "Detected attachment cycle");
				return;
			}
			if (m_Parent && std::count(m_Parent->m_AttachChildren.begin(), m_Parent->m_AttachChildren.end(), this) > 0)
			{
				VM_ASSERT(false, "Component already attached");
				return;
			}

			m_Parent = inParent;

		}
	}

	bool SceneComponent::IsAttachedTo(const SceneComponent* comp) const
	{
		if (!comp)
		{
			return false;
		}
		for (const SceneComponent* nextComp = GetParent(); nextComp != nullptr; nextComp = nextComp->GetParent())
		{
			if (nextComp == comp)
			{
				return true;
			}
		}
		return false;
	}

	SceneComponent::SceneComponent()
		: m_Parent(nullptr)
		, m_RelativeLocation(Math::Utils::ZeroVector)
		, m_RelativeRotation(Math::Utils::ZeroVector)
		, m_RelativeScale(Math::Utils::OnesVector)
		, m_RelativeRotationQuat(Math::Utils::QuatIdentity)
		, m_bWorldTransformUpdated(false)
		, m_bAbsoluteLocation(false)
		, m_bAbsoluteRotation(false)
		, m_bAbsoluteScale(false)
		, m_bVisible(true)
		, m_bHidden(false)
#ifdef VM_WITH_EDITOR
		, m_bEditorVisualize(true)
#endif
	{
	}

	void SceneComponent::SetRelativeLocationAndRotation(const glm::vec3& newLocation, const glm::quat& newRotation)
	{
		m_RelativeLocation = newLocation;
		m_RelativeRotationQuat = newRotation;
		m_RelativeRotation = glm::degrees(glm::eulerAngles(newRotation));
		UpdateWorldTransformWithParent(GetParent(), m_RelativeRotationQuat);
	}

	void SceneComponent::SetRelativeLocation(const glm::vec3& newLocation)
	{
		SetRelativeLocationAndRotation(newLocation, GetRelativeRotation());
	}

	void SceneComponent::SetRelativeRotation(const glm::vec3& newRotation)
	{
		if (!glm::all(glm::epsilonEqual(newRotation, GetRelativeRotation(), VM_EPSILON_SMALL)))
		{
			SetRelativeLocationAndRotation(GetRelativeLocation(), glm::quat(glm::radians(newRotation)));
		}
	}

	void SceneComponent::SetRelativeRotation(const glm::quat& newRotation)
	{
		SetRelativeLocationAndRotation(GetRelativeLocation(), newRotation);
	}

	void SceneComponent::SetRelativeScale(const glm::vec3& newScale)
	{
		if (newScale == GetRelativeScale())
		{
			return;
		}

		m_RelativeScale = newScale;
		UpdateWorldTransform();
	}

	void SceneComponent::SetRelativeTransform(const Transform& newTransform)
	{
		SetRelativeLocationAndRotation(newTransform.Translation, newTransform.Rotation);
		SetRelativeScale(newTransform.Scale);
	}

	void SceneComponent::ResetRelativeTransform()
	{
		SetRelativeLocationAndRotation(Math::Utils::ZeroVector, Math::Utils::ZeroVector);
		SetRelativeScale(Math::Utils::OnesVector);
	}

	void SceneComponent::AddRelativeLocation(const glm::vec3& deltaLocation)
	{
		SetRelativeLocationAndRotation(GetRelativeLocation() + deltaLocation, m_RelativeRotationQuat);
	}

	void SceneComponent::AddRelativeRotation(const glm::vec3& deltaRotation)
	{
		SetRelativeRotation(GetRelativeRotation() + deltaRotation);
	}

	void SceneComponent::AddRelativeRotation(const glm::quat& deltaRotation)
	{
		const glm::quat newRotation = deltaRotation * m_RelativeRotationQuat;
		SetRelativeLocationAndRotation(GetRelativeLocation(), newRotation);
	}

	void SceneComponent::AddLocalLocation(const glm::vec3& deltaLocation)
	{
		const glm::vec3& localOffset = m_RelativeRotationQuat * deltaLocation;
		SetRelativeLocationAndRotation(GetRelativeLocation() * localOffset, m_RelativeRotationQuat);
	}

	void SceneComponent::AddLocalRotation(const glm::vec3& deltaRotation)
	{
		const glm::quat newRotation = m_RelativeRotationQuat * glm::quat(glm::radians(deltaRotation));
		SetRelativeLocationAndRotation(GetRelativeLocation(), newRotation);
	}

	void SceneComponent::AddLocalRotation(const glm::quat& deltaRotation)
	{
		const glm::quat newRotation = m_RelativeRotationQuat * deltaRotation;
		SetRelativeLocationAndRotation(GetRelativeLocation(), newRotation);
	}

	void SceneComponent::AddLocalTransform(const Transform& deltaTransform)
	{
		const Transform relativeTransform(m_RelativeRotationQuat, GetRelativeLocation(), Math::Utils::OnesVector);
		const Transform newRelativeTransform = deltaTransform * relativeTransform;
		SetRelativeTransform(newRelativeTransform);
	}

	void SceneComponent::SetWorldLocation(const glm::vec3& newLocation)
	{
		glm::vec3 newRelativeLocation = newLocation;
		if (GetParent() && !IsAbsoluteLocation())
		{
			Transform parentWorld = GetParent()->GetTransform();
			newRelativeLocation = parentWorld.InverseTransformTranslation(newLocation);
		}
		SetRelativeLocation(newRelativeLocation);
	}

	void SceneComponent::SetWorldRotation(const glm::vec3& newRotation)
	{
		if (!GetParent())
		{
			SetRelativeRotation(newRotation);
		}
		else
		{
			SetWorldRotation(glm::quat(glm::radians(newRotation)));
		}
	}

	void SceneComponent::SetWorldRotation(const glm::quat& newRotation)
	{
		glm::quat newRelativeRotation = GetRelativeRotationFromWorld(newRotation);
		SetRelativeRotation(newRelativeRotation);
	}

	void SceneComponent::SetWorldScale(const glm::vec3& newScale)
	{
		glm::vec3 newRelativeScale = newScale;
		if (GetParent() && !IsAbsoluteScale())
		{
			Transform parentWorld = GetParent()->GetTransform();
			newRelativeScale = newScale * parentWorld.GetSafeScaleReciprocal(parentWorld.Scale);
		}
		SetRelativeScale(newRelativeScale);
	}

	void SceneComponent::SetWorldTransform(const Transform& newTransform)
	{
		if (GetParent())
		{
			const Transform parentWorld = GetParent()->GetTransform();
			Transform relativeTransform = newTransform.GetRelative(parentWorld);

			if (IsAbsoluteLocation())
			{
				relativeTransform.Translation = newTransform.Translation;
			}
			if (IsAbsoluteRotation())
			{
				relativeTransform.Rotation = newTransform.Rotation;
			}
			if (IsAbsoluteScale())
			{
				relativeTransform.Scale = newTransform.Scale;
			}
			SetRelativeTransform(relativeTransform);
		}
		else
		{
			SetRelativeTransform(newTransform);
		}
	}

	void SceneComponent::AddWorldLocation(const glm::vec3& deltaLocation)
	{
		const glm::vec3 newWorldLocation = deltaLocation + GetTransform().Translation;
		SetWorldLocation(newWorldLocation);
	}

	void SceneComponent::AddWorldRotation(const glm::vec3& deltaRotation)
	{
		const glm::quat newWorldRotation = glm::quat(glm::radians(deltaRotation)) * GetTransform().Rotation;
		SetWorldRotation(newWorldRotation);
	}

	void SceneComponent::AddWorldRotation(const glm::quat& deltaRotation)
	{
		const glm::quat newWorldRotation = deltaRotation * GetTransform().Rotation;
		SetWorldRotation(newWorldRotation);
	}

	void SceneComponent::AddWorldTransform(const Transform& deltaTransform)
	{
		const Transform& localTransform = GetTransform();
		const glm::quat newWorldRotation = deltaTransform.Rotation * localTransform.Rotation;
		const glm::vec3 newWorldLocation = deltaTransform.Translation + localTransform.Translation;
		SetWorldTransform(Transform(newWorldRotation, newWorldLocation, Math::Utils::OnesVector));
	}

	void SceneComponent::AddWorldTransformKeepScale(const Transform& deltaTransform)
	{
		const Transform& localTransform = GetTransform();
		const glm::quat newWorldRotation = deltaTransform.Rotation * localTransform.Rotation;
		const glm::vec3 newWorldLocation = deltaTransform.Translation + localTransform.Translation;
		SetWorldTransform(Transform(newWorldRotation, newWorldLocation, localTransform.Scale));
	}

	glm::quat SceneComponent::GetRelativeRotationFromWorld(const glm::quat& newRotation)
	{
		glm::quat newRelativeRotation = newRotation;

		if (GetParent() && IsAbsoluteRotation())
		{
			const Transform parentWorld = GetParent()->GetTransform();
			if (Transform::HasNegativeScale(GetRelativeScale()) || Transform::HasNegativeScale(parentWorld.Scale))
			{
				Transform newTransform = GetTransform();
				newTransform.Rotation = newRotation;
				const glm::quat newRelativeQuat = newTransform.GetRelative(parentWorld).Rotation;
				newRelativeRotation = newRelativeQuat;
			}
			else
			{
				const glm::quat parentWorldQuat = parentWorld.Rotation;
				const glm::quat newRelativeQuat = glm::inverse(parentWorldQuat) * newRotation;
				newRelativeRotation = newRelativeQuat;
			}
		}
		return newRelativeRotation;
	}

	glm::vec3 SceneComponent::GetRelativeRotation() const
	{
		return m_RelativeRotation;
	}

	glm::vec3 SceneComponent::GetRelativeLocation() const
	{
		return m_RelativeLocation;
	}

	glm::vec3 SceneComponent::GetRelativeScale() const
	{
		return m_RelativeScale;
	}

	Transform SceneComponent::GetRelativeTransform() const
	{
		const Transform relativeTransform(m_RelativeRotationQuat, GetRelativeLocation(), GetRelativeScale());
		return relativeTransform;
	}

	const Transform& SceneComponent::GetTransform() const
	{
		return m_Transform;
	}

	glm::vec3 SceneComponent::GetLocation() const
	{
		return m_Transform.Translation;
	}

	glm::vec3 SceneComponent::GetRotation() const
	{
		return glm::degrees(glm::eulerAngles(GetTransform().Rotation));
	}

	glm::quat SceneComponent::GetRotationQuat() const
	{
		return GetTransform().Rotation;
	}

	glm::vec3 SceneComponent::GetScale() const
	{
		return GetTransform().Scale;
	}

	Transform SceneComponent::CalcWorldTransform(const Transform& newRelative, const SceneComponent* parent) const
	{
		parent = parent ? parent : GetParent();
		if (parent)
		{
			const bool bAnyAbsolute = IsAbsoluteLocation() || IsAbsoluteRotation() || IsAbsoluteScale();
			if (!bAnyAbsolute)
			{
				return newRelative * parent->GetTransform();
			}
			return CalcWorldTransformAbsolute(newRelative, parent);
		}
		else
		{
			return newRelative;
		}
	}

	Transform SceneComponent::CalcWorldTransformAbsolute(const Transform& newRelative, const SceneComponent* parent) const
	{
		if (parent == nullptr)
		{
			return newRelative;
		}
		const Transform parentWorld = parent->GetTransform();
		Transform newWorld = newRelative * parentWorld;
		if (IsAbsoluteLocation())
		{
			newWorld.Translation = newRelative.Translation;
		}
		if (IsAbsoluteRotation())
		{
			newWorld.Rotation = newRelative.Rotation;
		}
		if (IsAbsoluteScale())
		{
			newWorld.Scale = newRelative.Scale;
		}
		return newWorld;
	}

	void SceneComponent::UpdateWorldTransformWithParent(SceneComponent* parent, const glm::quat& relativeRotation)
	{
		if (parent && !parent->m_bWorldTransformUpdated)
		{
			parent->UpdateWorldTransform();
			if (m_bWorldTransformUpdated)
			{
				return;
			}
		}
		m_bWorldTransformUpdated = true;
		Transform newTransform;
		const Transform relativeTransform(relativeRotation, GetRelativeLocation(), GetRelativeScale());
		newTransform = CalcWorldTransform(relativeTransform, parent);

		bool bChanged;
		bChanged = !GetTransform().Equals(newTransform, VM_EPSILON_SMALL);

		if (bChanged)
		{
			m_Transform = newTransform;
			if (m_bRegistered)
			{
				MarkRenderTransformDirty();
			}
			if (!m_AttachChildren.empty())
			{
				UpdateChildrenTransforms();
			}
		}

	}

	void SceneComponent::UpdateWorldTransform()
	{
		UpdateWorldTransformWithParent(GetParent(), m_RelativeRotationQuat);
	}

	void SceneComponent::Destroy(bool bPropagate /*= false*/)
	{
		if (!bPropagate)
		{
			Super::Destroy(bPropagate);
			return;
		}

		Entity* owner = GetOwner();
		if (!owner)
		{
			Super::Destroy(bPropagate);
			return;
		}

		SceneComponent* foundChild = nullptr;
		const std::vector<ObjectPtr<SceneComponent>> children = GetAttachChildren();
		if (this == owner->GetRootComponent())
		{
			auto it = std::find_if(children.begin(), children.end(), [owner](ObjectPtr<SceneComponent> child) {return child != nullptr && child->GetOwner() == owner; });
			if (it != children.end())
			{
				foundChild = (*it).Get();
			}
			owner->SetRootComponent(foundChild);
		}
		else
		{
			SceneComponent* parent = GetParent();
			if (parent)
			{
				DetachFromComponent(AttachmentTransformRules::TransformWorld);
				if (!children.empty())
				{
					auto it = std::find_if(children.begin(), children.end(), [owner](ObjectPtr<SceneComponent> child) {return child != nullptr; });
					if (it != children.end())
					{
						foundChild = (*it).Get();
					}
					// else all nullptrs
				}
				if (foundChild)
				{
					foundChild->AttachToComponent(parent, AttachmentTransformRules::TransformWorld);
				}
			}
			else
			{
				VM_CORE_WARN("Deleting a non-root scene component with no parent: {}", GetName());
			}
		}

		std::vector<ObjectPtr<SceneComponent>> childrenCopy = children;
		for (ObjectPtr<SceneComponent> child : childrenCopy)
		{
			if (child)
			{
				child->DetachFromComponent(AttachmentTransformRules::TransformWorld);
				if (child != foundChild)
				{
					child->AttachToComponent(foundChild, AttachmentTransformRules::TransformWorld);
				}
			}
		}
		Super::Destroy(bPropagate);

	}

	void SceneComponent::OnDestroyed(bool bPropagate)
	{
		Super::OnDestroyed(bPropagate);
#ifdef VM_WITH_EDITOR
		// TODO: Destroy sprite component
#endif // VM_WITH_EDITOR

		Entity* owner = GetOwner();

		if (bPropagate)
		{
			bool bHasExternalParent = false;
			SceneComponent* externalParent = nullptr;

			int32 childCount = m_AttachChildren.size();
			std::vector<ObjectPtr<SceneComponent>> children;
			children.reserve(childCount);

			while (childCount > 0)
			{
				SceneComponent* child = m_AttachChildren.back().Get();
				if (child && child->GetOwner() != owner)
				{
					if (child->GetParent())
					{
						if (child->GetParent() == this)
						{
							bool bShouldDetach = true;
							if (!child->HasFlag(Flags::FLAG_GARBAGE) && !child->HasFlag(Flags::FLAG_PENDING_KILL))
							{
								if (!bHasExternalParent)
								{
									externalParent = GetParent();
									while (externalParent)
									{
										if (!externalParent->HasFlag(Flags::FLAG_GARBAGE) && !externalParent->HasFlag(Flags::FLAG_PENDING_KILL) && externalParent->GetOwner() != owner)
										{
											break;
										}
										externalParent = externalParent->GetParent();
									}
									bHasExternalParent = true;
								}

								if (externalParent)
								{
									bShouldDetach = (child->AttachToComponent(externalParent, AttachmentTransformRules::TransformWorld) == false);
								}
							}
							if (bShouldDetach)
							{
								child->DetachFromComponent(AttachmentTransformRules::TransformWorld);
							}
						}
						else
						{
							VM_CORE_ERROR("Component {} has child {}, but it has parent {}", GetName(), child->GetName(), child->GetParent()->GetName());
							m_AttachChildren.pop_back();
						}
					}
					else
					{
						VM_CORE_ERROR("Component {} has child {}, but it has parent nullptr", GetName(), child->GetName());
						m_AttachChildren.pop_back();
					}
				}
				else
				{
					m_AttachChildren.pop_back();
					if (child)
					{
						children.push_back(ObjectPtr(child));
					}
				}
				childCount = m_AttachChildren.size();
			}
			m_AttachChildren = std::move(children);
		}
		else
		{
			int32 childCount = m_AttachChildren.size();
			while (childCount > 0)
			{
				if (SceneComponent* child = m_AttachChildren.back().Get())
				{
					SceneComponent* childParent = child->GetParent();
					if (childParent)
					{
						if (childParent == this)
						{
							bool bShouldDetach = true;
							if (!child->HasFlag(Flags::FLAG_GARBAGE) && !child->HasFlag(Flags::FLAG_PENDING_KILL))
							{
								SceneComponent* newParent = GetParent();
								while (newParent && (newParent->HasFlag(Flags::FLAG_GARBAGE) || newParent->HasFlag(Flags::FLAG_PENDING_KILL)))
								{
									newParent = newParent->GetParent();
								}
								if (newParent)
								{
									bShouldDetach = (child->AttachToComponent(newParent, AttachmentTransformRules::TransformWorld) == false);

								}
							}
							if (bShouldDetach)
							{
								child->DetachFromComponent(AttachmentTransformRules::TransformWorld);
							}
						}
						else
						{
							VM_CORE_ERROR("Component {} has child {}, but it has parent {}", GetName(), child->GetName(), child->GetParent()->GetName());
							m_AttachChildren.pop_back();
						}
					}
					else
					{
						VM_CORE_ERROR("Component {} has child {}, but it has parent nullptr", GetName(), child->GetName());
						m_AttachChildren.pop_back();
					}
				}
				else
				{
					m_AttachChildren.pop_back();
				}
				childCount = m_AttachChildren.size();
			}
		}
		if (GetParent() && (!bPropagate || GetParent()->GetOwner() != owner))
		{
			DetachFromComponent(AttachmentTransformRules::TransformWorld);
		}
	}

#ifdef VM_WITH_EDITOR
	void SceneComponent::MarkReferencedObjects(GarbageCollector& gc)
	{
		Super::MarkReferencedObjects(gc);
		// Mark sprite component
	}

	bool SceneComponent::ShouldRender() const
	{
		Entity* owner = GetOwner();
		const bool bShowInEditor =
#ifdef VM_WITH_EDITOR
			g_IsEditor ? (!owner || !owner->IsHiddenInEditor()) : false;
#else
			false;
#endif
		const bool bShowInGame = IsVisible() && (!owner || !owner->IsHidden());
		return (bShowInGame || bShowInEditor) && m_bVisible == true;
	}

	SceneComponent* SceneComponent::GetRoot() const
	{
		const SceneComponent* root;
		for (root = this; root && root->GetParent(); root = root->GetParent());
		return const_cast<SceneComponent*>(root);
	}

	Entity* SceneComponent::GetEntityOfRoot() const
	{
		const SceneComponent* const rootComp = GetRoot();
		return rootComp ? rootComp->GetOwner() : nullptr;
	}

	glm::vec3 SceneComponent::GetEntityTranslation() const
	{
		const SceneComponent* root;
		for (root = this; root->GetParent(); root = root->GetParent());
		return (root->GetOwner() != nullptr) ? root->GetOwner()->GetLocation() : Math::Utils::ZeroVector;
	}

	Entity* SceneComponent::GetParentEntity() const
	{
		const SceneComponent* const parent = GetParent();
		return parent ? parent->GetOwner() : nullptr;
	}

#endif
	void SceneComponent::UpdateChildrenTransforms()
	{
		if (!m_AttachChildren.empty())
		{
			for (ObjectPtr<SceneComponent> childComp : GetAttachChildren())
			{
				if (!childComp)
					continue;

				//if (!childComp->m_bWorldTransformUpdated)
				//{
				childComp->UpdateWorldTransform();
				//}
			}
		}
	}

	void SceneComponent::SetUseAbsoluteLocation(bool bUse)
	{
		if (bUse != m_bAbsoluteLocation)
		{
			m_bAbsoluteLocation = bUse;
			if (m_bAbsoluteLocation)
			{
				SetRelativeTransform(GetTransform());
			}
			else
			{
				SetWorldTransform(GetTransform());
			}
		}
	}

	void SceneComponent::SetUseAbsoluteRotation(bool bUse)
	{
		if (bUse != m_bAbsoluteRotation)
		{
			m_bAbsoluteRotation = bUse;
			if (m_bAbsoluteRotation)
			{
				SetRelativeTransform(GetTransform());
			}
			else
			{
				SetWorldTransform(GetTransform());
			}
		}
	}

	void SceneComponent::SetUseAbsoluteScale(bool bUse)
	{
		if (bUse != m_bAbsoluteScale)
		{
			m_bAbsoluteScale = bUse;
			if (m_bAbsoluteScale)
			{
				SetRelativeTransform(GetTransform());
			}
			else
			{
				SetWorldTransform(GetTransform());
			}
		}
	}

	bool SceneComponent::IsVisible() const
	{
		if (m_bHidden)
		{
			return false;
		}
		return m_bVisible;
	}

	bool SceneComponent::IsVisibleInEditor() const
	{
		return m_bVisible;
	}

	void SceneComponent::OnVisibilityChanged()
	{
		MarkRenderStateDirty();
	}

	void SceneComponent::OnHiddenChanged()
	{
		MarkRenderStateDirty();
	}

	void SceneComponent::SetVisibility(const bool bInVisibility, bool bPropagate)
	{
		if (bInVisibility != m_bVisible)
		{
			m_bVisible = bInVisibility;
			OnVisibilityChanged();
		}
		const std::vector<ObjectPtr<SceneComponent>>& children = GetAttachChildren();
		if (bPropagate && !children.empty())
		{
			std::list<SceneComponent*> components;
			std::transform(children.begin(), children.end(), std::back_inserter(components),
				[](const ObjectPtr<SceneComponent>& ptr) {
					return ptr.Get();
				});
			while (!components.empty())
			{
				SceneComponent* const comp = components.back();
				components.pop_back();
				if (comp)
				{
					std::transform(comp->GetAttachChildren().begin(), comp->GetAttachChildren().end(), std::back_inserter(components),
						[](const ObjectPtr<SceneComponent>& ptr) {
							return ptr.Get();
						});
					if (bPropagate)
					{
						comp->SetVisibility(bInVisibility, false);
					}
					comp->MarkRenderStateDirty();
				}
			}
		}
	}

	void SceneComponent::SetVisibility(bool bInVisibility)
	{
		SetVisibility(bInVisibility, true);
	}

	void SceneComponent::ToggleVisibility()
	{
		SetVisibility(!m_bVisible, true);
	}

	void SceneComponent::SetHidden(const bool bInHidden, bool bPropagate)
	{
		if (bInHidden != m_bHidden)
		{
			m_bHidden = bInHidden;
			OnHiddenChanged();
		}
		const std::vector<ObjectPtr<SceneComponent>>& children = GetAttachChildren();
		if (bPropagate && !children.empty())
		{
			std::list<SceneComponent*> components;
			std::transform(children.begin(), children.end(), std::back_inserter(components),
				[](const ObjectPtr<SceneComponent>& ptr) {
					return ptr.Get();
				});
			while (!components.empty())
			{
				SceneComponent* const comp = components.back();
				components.pop_back();
				if (comp)
				{
					std::transform(comp->GetAttachChildren().begin(), comp->GetAttachChildren().end(), std::back_inserter(components),
						[](const ObjectPtr<SceneComponent>& ptr) {
							return ptr.Get();
						});
					if (bPropagate)
					{
						comp->SetHidden(bInHidden, false);
					}
					comp->MarkRenderStateDirty();
				}
			}
		}
	}

	void SceneComponent::SetHidden(bool bInHidden)
	{
		SetHidden(bInHidden);
	}

	void SceneComponent::OnRegister()
	{
		if (GetParent())
		{
			// SetupAttachment just assigned m_Parent. Now we need to actually attach component
			if (AttachToComponent(GetParent(), AttachmentTransformRules::TransformRelative) == false)
			{
				m_Parent = nullptr;
			}
		}

		Super::OnRegister();

#ifdef VM_WITH_EDITOR
		// TODO: Create Sprite component to see empty scene component in editor
#endif
	}


	void SceneComponent::AppendAllChildren(std::vector<SceneComponent*> outComps) const
	{
		const std::vector<ObjectPtr<SceneComponent>> children = GetAttachChildren();
		outComps.reserve(outComps.size() + children.size());
		for (ObjectPtr<SceneComponent> child : children)
		{
			if (child)
			{
				outComps.push_back(child.Get());
			}
		}

		for (ObjectPtr<SceneComponent> child : children)
		{
			if (child)
			{
				child->AppendAllChildren(outComps);
			}
		}

	}

}

