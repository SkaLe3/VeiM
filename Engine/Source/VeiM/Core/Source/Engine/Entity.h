#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"
#include "Misc/Guid.h"

#include "Engine/Reflection.h"
#include "Engine/CoreObject.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/Component.h"
#include "Engine/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "Types/Delegate.h"

#include <glm/glm.hpp>

// Entity.h
namespace VeiM
{
	DECLARE_EVENT_1p(EntityEndPlayDelegate, Entity*);


	class World;
	class WorldSettings;
	class GlobalGameState; // GameInstance
	class InputComponent;


	class CORE_API Entity : public Object
	{
		DECLARE_CLASS(Entity, Object)
	public:
		Entity();

	private:
		void InitializeDefaults();

	public:
		virtual void TickInWorld(float deltaTime);
		virtual void Tick(float deltaTime) {}

		String GetLabel() const;

		/* Input */
		virtual void EnableInput(class Controller* playerController);
		virtual void DisableInput(class Controller* playerController);
		virtual void CreateInputComponent(ClassDescriptor* inputComponentClass);
		/* ----- */

		/* General */
		FORCEINLINE const Transform& GetTransform() const { return RootComponent ? RootComponent->GetTransform() : Transform::Identity; }
		void SetTransform(const Transform& newTransform);
		const glm::vec3 GetForwardVector() const;
		const glm::vec3 GetUpVector() const;
		const glm::vec3 GetRightVector() const;
		SceneComponent* GetDefaultAttachComponent() { return RootComponent.Get(); }

		bool SetLocation(const glm::vec3& newLocation);
		bool SetRotation(const glm::vec3& newRotation);
		bool SetRotation(const glm::quat& newRotation);
		bool SetScale(const glm::vec3 newScale);
		bool SetLocationAndRotation(const glm::vec3& newLocation, const glm::vec3& newRotation);
		bool SetLocationAndRotation(const glm::vec3& newLocation, const glm::quat& newRotation);

		void AddWorldLocation(const glm::vec3& deltaLocation);
		void AddWorldRotation(const glm::vec3& deltaRotation);
		void AddWorldRotation(const glm::quat& deltaRotation);
		void AddWorldTransform(const Transform& deltaTransform);
		void AddWorldTransformKeepScale(const Transform& deltaTransform);

		void AddLocalLocation(const glm::vec3& deltaLocation);
		void AddLocalRotation(const glm::vec3& deltaRotation);
		void AddLocalRotation(const glm::quat& deltaRotation);
		void AddLocalTransform(const Transform& deltaTransform);

		void SetRelativeLocation(const glm::vec3& newLocation);
		void SetRelativeRotation(const glm::vec3& newRotation);
		void SetRelativeRotation(const glm::quat& newRotation);
		void SetRelativeScale(const glm::vec3& newScale);
		void SetRelativeTransform(const Transform& newTransform);

		glm::vec3 GetScale() const;
		glm::vec3 GetLocation() const;
		
		glm::vec3 GetRelativeScale() const;

		float GetDistanceTo(const Entity* otherEntity) const;
		float GetSquaredDistanceTo(const Entity* otherEntity) const;
		float GetDotProductTo(const Entity* otherEntity) const;
		/* ------- */

		bool IsHidden() const;
		virtual void SetHiddenInGame(bool bHidden);
		

		Component* AddComponent(ClassDescriptor* componentClass,bool bManualAttach, const Transform& relativeTransform, StringID name = StringID(EStringID::None)); // Not type safe
		bool AttachToComponent(SceneComponent* parent, const AttachmentTransformRules& rules);
		bool AttachToEntity(Entity* parent, const AttachmentTransformRules& rules);
		void DetachFromEntity(const AttachmentTransformRules& rules);
		void DetachAllSceneComponents(SceneComponent* parent, const AttachmentTransformRules& rules);
		void GetAttachedEntities(std::vector<Entity*>& outEntities);

		SceneComponent* GetRootComponent() const { return RootComponent.Get(); }
		bool SetRootComponent(SceneComponent* newRootComponent);

		void AddOwnedComponent(Component* component);
		void RemoveOwnedComponent(Component* component);
		bool OwnsComponent(Component* component);

		void AddRuntimeComponent(Component* component);  //TODO: Remove if not needed
		void RemoveRuntimeComponent(Component* component);  //TODO: Remove if not needed


		bool HasTag(StringID tag) const;

		float GetTimeScale() const;

		bool IsInitialized() const { return m_bInitialized; }
		bool HasBegunPlay() const { return m_bHasBegunPlay; }
		bool AllComponentsRegistered() const { return m_bAllComponentsRegistered; }


		bool Destroy();

		// Consider OnDestroy delegate
		// Consider OnEndPlay delegate

		void RegisterAllComponents();
		void UnregisterAllComponents();
		void ProcessComponentRegistration(Component* component);
		void OnSpawnInitialize(const Transform& transform, bool bOverrideRootScale);
		void PostConstruction();

		virtual void RegisterTickFunction(bool bRegister);
		void RegisterTickFunctions(bool bRegister, bool bComponents);
		virtual void SetTickEnable(bool bEnabled);
		virtual bool IsTickEnabled() const { return DefaultEntityTick.IsEnabled(); }
		void SetTickPeriod(float period) { DefaultEntityTick.TickPeriod = period; }
		float GetTickPeriod() const { return DefaultEntityTick.TickPeriod; }

		Entity* GetOwner() const;
		template<typename T>
		T* GetOwner() const { return CastObject<T>(GetOwner()); }
		Level* GetLevel() const;
		WorldSettings* GetWorldSettings() const;
		virtual World* GetWorld() const override final;
		GlobalGameState* GetGlobalGameState() const;
		template<typename T>
		T* GetGlobalGameState() const { return CastObject<T>(GetGlobalGameState()); }
		// Add TimerManager getter - not yet
		virtual Component* GetComponentByClass(ClassDescriptor* componentClass) const; // Not type safe
		template<typename T>
		T* GetComponentByClass() const { return CastObject<T>(GetComponentByClass(T::StaticClass())); }
		virtual Component* GetComponentByName(StringID name) const;
		// TODO: Add Getter by Tag
		const std::unordered_set<ObjectPtr<Component>>& GetComponents() const { return m_OwnedComponents; }
		void GetComponents(std::vector<Component*>& outComponents) const;
		template <typename T>
		void GetComponents(std::vector<T*>& outComponents) const;
		const std::vector<ObjectPtr<Component>> GetRuntimeComponents() const { return m_RuntimeComponents; }


		void UpdateComponentsVisibility();
		//void UpdateComponentsTransforms() ? - Yes, to update positions and stuff for render and physics
		void MarkComponentsAsPendingKill();
		inline bool IsDying() const { return m_bDying; }
		void SetDying(bool bDying) { m_bDying = bDying; }

		virtual void PreInitializeComponents();
		void InitializeComponents();
		void UninitializeComponents();

		void ProcessBeginPlay();
		void ProcessEndPlay();
		/* Object */
		virtual void StartDestroy() override;
#if 0
		void Serialize(serializer);

#endif

	protected:
		virtual void BeginPlay();
		virtual void EndPlay();

	public:
		EntityEndPlayDelegate OnEndPlay;
		virtual void MarkReferencedObjects(GarbageCollector& gc) override;
		virtual void Duplicate(Object* sourceObj, Object* destintationObj) override;
	private:
		std::unordered_set<ObjectPtr<Component>> m_OwnedComponents;
		std::vector<ObjectPtr<Component>> m_RuntimeComponents;   // TODO: Remove if not needed

	public:
		ObjectPtr<InputComponent> EntityInputComponent; //TODO: Register | created at runtime when possesed
	protected:
		ObjectPtr<SceneComponent> RootComponent; // TODO: Register

	public:
		EntityTickFunction DefaultEntityTick;
		float TimeScale; // Multiplied by global scale in world settings
		int32 ControllerIndex;
		int32 InputPriority;
		std::vector<StringID> Tags;

		/* Add Delegates*/

		uint8 bBlockInput : 1;
	private:
		uint8 m_bHidden : 1; // Hide in runtime
		uint8 m_bInitialized : 1;
		uint8 m_bTicksRegistered : 1;
		uint8 m_bAllComponentsRegistered : 1;
		uint8 m_bEnableCollision : 1;
		uint8 m_bDying : 1;
		uint8 m_bHasBegunPlay : 1;
		uint8 m_bConstructing : 1; // Delete if not used

#ifdef VM_WITH_EDITOR
	public:
		virtual bool IsHiddenInEditor() const;

		void SetLabel(const String& newLabel);
		Guid GetGuid() const { return m_Guid; }
		void SetGuid(Guid guid) { m_Guid = guid; }
		uint8 bHiddenInEditor : 1;
	protected:
		Guid m_Guid;
	private:
		String m_Label;
#endif

	};

	template <typename T>
	void VeiM::Entity::GetComponents(std::vector<T*>& outComponents) const
	{
		for (const ObjectPtr<Component> component : m_OwnedComponents)
		{
			outComponents.push_back(CastObject<T>(component.Get()));
		}
	}

}