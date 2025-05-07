#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"
#include "Misc/Guid.h"

#include "Engine/Reflection.h"
#include "Engine/CoreObject.h"
#include "Engine/Component.h"
#include "Engine/SceneComponent.h"
#include "Engine/InputComponent.h"
#include "Engine/EngineTypes.h"


#include <glm/glm.hpp>

// Entity.h
namespace VeiM
{

	class World;
	class WorldSettings;
	class GlobalGameState; // GameInstance


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

		// TODO: Implement next block
#if 0
		/* Input */
		virtual void EnableInput(class PlayerController* playerController);
		virtual void DisableInput(class PlayerController* playerController);
		virtual void CreateInputComponent(ClassDescriptor* inputComponentClass); //Not type safe. Here InputComponent
		/* ----- */

		/* General */
		const Transform& GetTransform() const { return RootComponent ? RootComponent->GetTransform() : Transform::Identity; }
		void SetTransform(const Transform& newTransform);
#endif
		const glm::vec3 GetForwardVector() const;
		const glm::vec3 GetUpVector() const;
		const glm::vec3 GetRightVector() const;
#if 0

		virtual void GetBounds() const; // Not implemented yet, ignore
		void SetLocation(const glm::vec3& newLocation);
		void SetRotation(const glm::quat& newRotation);
		void SetScale(const glm::vec3 newScale);
		/* ------- */
#endif	

		virtual void SetHiddenInGame(bool bHidden);
		// void SetEnableCollistion(bool bEnabled); + getter

		Component* AddComponent(ClassDescriptor* componentClass); // Not type safe
		bool AttachToComponent(SceneComponent* parent); //TODO: Add rules for attachment
		bool AttachToEntity(Entity* parent);
		void DetachFromEntity();
		void DetachAllSceneComponents(SceneComponent* parent);
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
		void OnSpawnInitialize();
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
		// TODO: Add Getter by Tag

		const std::unordered_set<ObjectPtr<Component>>& GetComponents() const { return m_OwnedComponents; }
		void GetComponents(std::vector<Component*>& outComponents) const;
		template <typename T>
		void GetComponents(std::vector<T*>& outComponents) const;
		const std::vector<ObjectPtr<Component>> GetRuntimeComponents() const { return m_RuntimeComponents; }


		void MarkComponentsRenderStateDirty();
		//void UpdateComponentsTransforms() ? - Yes, to update positions and stuff for render and physics
		void MarkComponentsAsPendingKill();
		inline bool IsDying() const { return m_bDying; }
		void SetDying(bool bDying) { m_bDying = bDying; }

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
		virtual void MarkReferencedObjects(GarbageCollector& gc) override;
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
		int32 InputPriority;
		std::vector<StringID> Tags;

		/* Add Delegates*/

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