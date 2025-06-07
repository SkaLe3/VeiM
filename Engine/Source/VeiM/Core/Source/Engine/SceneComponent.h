#pragma once
#include "Engine/Component.h"

#include "Utils/Math.h"

namespace VeiM
{
	using namespace Math;

	class CORE_API SceneComponent : public Component
	{
		DECLARE_CLASS(SceneComponent, Component);
	public:		
		SceneComponent();
	public:
		void SetRelativeLocationAndRotation(const glm::vec3& newLocation,const glm::quat& newRotation);
		void SetRelativeLocation(const glm::vec3& newLocation);
		void SetRelativeRotation(const glm::vec3& newRotation);
		void SetRelativeRotation(const glm::quat& newRotation);
		void SetRelativeScale(const glm::vec3& newScale);
		void SetRelativeTransform(const Transform& newTransform);
		void ResetRelativeTransform();
		void AddRelativeLocation(const glm::vec3& deltaLocation);
		void AddRelativeRotation(const glm::vec3& deltaRotation);
		void AddRelativeRotation(const glm::quat& deltaRotation);

		void AddLocalLocation(const glm::vec3& deltaLocation);
		void AddLocalRotation(const glm::vec3& deltaRotation);
		void AddLocalRotation(const glm::quat& deltaRotation);
		void AddLocalTransform(const Transform& deltaTransform);

		void SetWorldLocation(const glm::vec3& newLocation);
		void SetWorldRotation(const glm::vec3& newRotation);
		void SetWorldRotation(const glm::quat& newRotation);
		void SetWorldScale(const glm::vec3& newScale);
		void SetWorldTransform(const Transform& newTransform);
		void AddWorldLocation(const glm::vec3& deltaLocation);
		void AddWorldRotation(const glm::vec3& deltaRotation);
		void AddWorldRotation(const glm::quat& deltaRotation);
		void AddWorldTransform(const Transform& deltaTransform);
		void AddWorldTransformKeepScale(const Transform& deltaTransform);

		glm::vec3 GetForwardVector();
		glm::vec3 GetUpVector();
		glm::vec3 GetRightVector();

		glm::quat GetRelativeRotationFromWorld(const glm::quat& newRotation);
		glm::vec3 GetRelativeRotation() const;
		glm::vec3 GetRelativeLocation() const;
		glm::vec3 GetRelativeScale() const;
		Transform GetRelativeTransform() const;
		const Transform& GetTransform() const;

		glm::vec3 GetLocation() const;
		glm::vec3 GetRotation() const;
		glm::quat GetRotationQuat() const;
		glm::vec3 GetScale() const;

		SceneComponent* GetParent() const;
		void GetAllParents(std::vector<SceneComponent*>& outParents) const;
		SceneComponent* GetChildComponent(int32 childIdx) const;
		void GetChildrenComponents(std::vector<SceneComponent*> outComps, bool bDoChildren = true) const;
		const std::vector<ObjectPtr<SceneComponent>>& GetAttachChildren() const;

		void SetupAttachment(SceneComponent* inParent);
		bool AttachToComponent(SceneComponent* newParent, const AttachmentTransformRules& attachRules);
		void DetachFromComponent(const AttachmentTransformRules detachRules);

		Transform CalcWorldTransform(const Transform& newRelative, const SceneComponent* parent = nullptr) const;
		Transform CalcWorldTransformAbsolute(const Transform& newRelative, const SceneComponent* parent) const;
		
		void UpdateChildrenTransforms();
		bool IsAbsoluteLocation() const { return m_bAbsoluteLocation; }
		bool IsAbsoluteRotation() const { return m_bAbsoluteRotation; }
		bool IsAbsoluteScale() const { return m_bAbsoluteScale; }
		void SetUseAbsoluteLocation(bool bUse);
		void SetUseAbsoluteRotation(bool bUse);
		void SetUseAbsoluteScale(bool bUse);

		virtual bool IsVisible() const;
		virtual bool IsVisibleInEditor() const;
	protected:
		virtual void OnVisibilityChanged();
		virtual void OnHiddenChanged();

	private:
		void SetVisibility(const bool bInVisibility, bool bPropagate);
		void SetHidden(const bool bInHidden, bool bPropagate);
		void AppendAllChildren(std::vector<SceneComponent*> outComps) const;
		void UpdateWorldTransformWithParent(SceneComponent* parent, const glm::quat& relativeRotation);
	public:
		void SetVisibility(bool bInVisibility);
		void ToggleVisibility();
		void SetHidden(bool bInHidden);

		/* Component Interface */
		virtual void OnRegister() override;
		virtual void UpdateWorldTransform() override final;
		virtual void Destroy(bool bPropagate = false) override;
		virtual void OnDestroyed(bool bPropagate) override;

		/* Object Interface */
#ifdef VM_WITH_EDITOR
		virtual void MarkReferencedObjects(GarbageCollector& gc) override;
#endif 
		/* */
		bool ShouldRender() const;
		SceneComponent* GetRoot() const;
		Entity* GetEntityOfRoot() const;
		glm::vec3 GetEntityTranslation() const;
		Entity* GetParentEntity() const;
		bool IsAttachedTo(const SceneComponent* comp) const;
	private:
		ObjectPtr<SceneComponent> m_Parent;
		std::vector<ObjectPtr<SceneComponent>> m_AttachChildren;

		glm::vec3 m_RelativeLocation;
		glm::vec3 m_RelativeRotation;
		glm::vec3 m_RelativeScale;
		glm::quat m_RelativeRotationQuat;
		Transform m_Transform; // Add reflection to transform type
	private:
		uint8 m_bWorldTransformUpdated : 1;
		uint8 m_bAbsoluteLocation : 1;
		uint8 m_bAbsoluteRotation : 1;
		uint8 m_bAbsoluteScale : 1;
		uint8 m_bVisible : 1;
		uint8 m_bHidden : 1;
#ifdef VM_WITH_EDITOR
	public:
		uint8 m_bEditorVisualize : 1;
		// Add SpriteComponent creation functions + pointer to component
	private:
#endif
		// Add mobility? (for optimizations)
	};
}