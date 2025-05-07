#include "Entity.h"
#include "Engine/EngineTypes.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/GlobalGameState.h"
#include "Engine/WorldSettings.h"
#include "Utils/Math.h"

#include <unordered_set>

namespace VeiM
{
	IMPLEMENT_CLASS(Entity);

	static_assert(std::is_base_of_v<Object, InputComponent>,
		"InputComponent must derive from Object");

	Entity::Entity()
	{
		InitializeDefaults();
	}

	void Entity::InitializeDefaults()
	{
		DefaultEntityTick.TickGroup = ETickGroup::Default;
		DefaultEntityTick.bTickable = false;
		DefaultEntityTick.bTickOnStart = true;
		DefaultEntityTick.SetEnable(false);

		TimeScale = 1.0f;
		m_bAllComponentsRegistered = false;
		m_bHasBegunPlay = false;
		m_bTicksRegistered = false;
		m_bHidden = false;
		m_bInitialized = false;
		m_bEnableCollision = false;
		m_bDying = false;

	}

	void EntityTickFunction::RunTick(float deltaTime)
	{
		if (TargetObject)
		{
			// Add Timer for profiling
			TargetObject->TickInWorld(deltaTime * TargetObject->TimeScale);
		}
	}



	void Entity::TickInWorld(float deltaTime)
	{
		if (GetWorld())
		{
			Tick(deltaTime);
		}
	}

	String Entity::GetLabel() const
	{
#ifdef VM_WITH_EDITOR
		if (!m_Label.empty())
		{
			return m_Label;
		}
#endif
		return GetName();
	}

	const glm::vec3 Entity::GetForwardVector() const
	{
		return RootComponent != nullptr ? RootComponent->GetForwardVector() : MathUtils::ForwardVector;
	}

	const glm::vec3 Entity::GetUpVector() const
	{
		return RootComponent != nullptr ? RootComponent->GetUpVector() : MathUtils::UpVector;
	}

	const glm::vec3 Entity::GetRightVector() const
	{
		return RootComponent != nullptr ? RootComponent->GetRightVector() : MathUtils::RightVector;
	}

	void Entity::SetHiddenInGame(bool bHidden)
	{
		if (m_bHidden != bHidden)
		{
			m_bHidden = bHidden;
			MarkComponentsRenderStateDirty();
		}
	}

	Component* Entity::AddComponent(ClassDescriptor* componentClass)
	{
		if (!componentClass) return nullptr;

		const World* world = GetWorld();
		if (!world) return nullptr;
		if (world->bIsDying) return nullptr;

		Component* newComponent = NewObject<Component>(componentClass, this, StringID(EStringID::None));
		if (!newComponent) return nullptr;

		AddOwnedComponent(newComponent);
		newComponent->OnCreated();

		bool bIsSceneComponent = false;
		if (SceneComponent* newSceneComponent = CastObject<SceneComponent>(newComponent))
		{
			// Set Transform for component with transform from parameters
			bIsSceneComponent = true;
		}

		newComponent->RegisterComponent();
		return newComponent;
	}

	bool Entity::AttachToComponent(SceneComponent* parent)
	{
		if (RootComponent && parent)
		{
			return RootComponent->AttachToComponent(parent);
		}
		return false;
	}



	bool Entity::AttachToEntity(Entity* parent)
	{
		if (RootComponent && parent)
		{
			SceneComponent* parentRoot = parent->GetRootComponent(); // Provide GetDefaultAttachComponent() virtual function with default GetRootComponent, but give ability to override it and use another component as attach parent
			if (parentRoot)
			{
				return RootComponent->AttachToComponent(parentRoot);
			}
		}
		return false;
	}

	void Entity::DetachFromEntity()
	{
		if (RootComponent)
		{
			RootComponent->DetachFromComponent();
		}
	}

	void Entity::DetachAllSceneComponents(SceneComponent* parent)
	{
		if (parent)
		{
			std::vector<SceneComponent*> components;
			GetComponents(components);

			for (SceneComponent* component : components)
			{
				if (component->GetParent() == parent)
				{
					component->DetachFromComponent();
				}
			}
		}
	}

	void Entity::GetAttachedEntities(std::vector<Entity*>& outEntities)
	{
		// TODO: Implement
	}

	bool Entity::SetRootComponent(SceneComponent* newRootComponent)
	{
		if (newRootComponent == RootComponent.Get()) return false;
		if (newRootComponent->GetOwner() != this) return false;

		RootComponent = newRootComponent;
		return true;
	}

	void Entity::AddOwnedComponent(Component* component)
	{
		ObjectPtr<Component> componentPtr = ObjectPtr(component);
		bool bWasOwned = m_OwnedComponents.contains(componentPtr);
		m_OwnedComponents.insert(componentPtr);

		if (bWasOwned) return;

		if (!component->bCreatedNatively)
		{
			m_RuntimeComponents.push_back(componentPtr);
		}

	}

	void Entity::RemoveOwnedComponent(Component* component)
	{
		ObjectPtr<Component> componentPtr = ObjectPtr(component);
		bool bIsOwned = m_OwnedComponents.erase(componentPtr);
		if (bIsOwned)
		{
			m_RuntimeComponents.erase(std::remove(
				m_RuntimeComponents.begin(), m_RuntimeComponents.end(), componentPtr), m_RuntimeComponents.end());
		}
	}

	bool Entity::OwnsComponent(Component* component)
	{
		return m_OwnedComponents.contains(ObjectPtr(component));
	}

	void Entity::AddRuntimeComponent(Component* component)
	{
		component->bCreatedNatively = false;
		m_RuntimeComponents.push_back(ObjectPtr(component));
	}

	void Entity::RemoveRuntimeComponent(Component* component)
	{
		ObjectPtr<Component> componentPtr = ObjectPtr(component);
		m_RuntimeComponents.erase(std::remove(
			m_RuntimeComponents.begin(), m_RuntimeComponents.end(), componentPtr), m_RuntimeComponents.end());
	}

	Component* Entity::GetComponentByClass(ClassDescriptor* componentClass) const
	{
		Component* foundComponent = nullptr;
		for (ObjectPtr<Component> component : m_OwnedComponents)
		{
			if (component && component->IsA(componentClass))
			{
				foundComponent = component.Get();
				break;
			}
		}
		return foundComponent;
	}

	void Entity::GetComponents(std::vector<Component*>& outComponents) const
	{
		for (const ObjectPtr<Component>& component : m_OwnedComponents)
		{
			outComponents.push_back(component.Get());
		}
	}

	bool Entity::HasTag(StringID tag) const
	{
		return (tag != EStringID::None) && (std::find(Tags.begin(), Tags.end(), tag) != Tags.end());
	}

	float Entity::GetTimeScale() const
	{
		return TimeScale * GetWorld()->GetSettings()->GetTimeScale(); // GetWorldSettings
	}

	bool Entity::Destroy()
	{
		if (!m_bDying)
		{
			World* world = GetWorld();
			if (world)
			{
				world->DestroyEntity(this);
			}
		}
		return m_bDying;
	}

	static SceneComponent* GetUnregisteredParentComponent(Component* component)
	{
		SceneComponent* parent = nullptr;
		SceneComponent* thisComponent = CastObject<SceneComponent>(component);

		while (thisComponent && thisComponent->GetParent()
			&& thisComponent->GetParent()->GetOwner() == component->GetOwner()
			&& !thisComponent->GetParent()->IsRegistered())
		{
			thisComponent = thisComponent->GetParent();
			parent = thisComponent;
		}
		return parent;
	}

	void Entity::RegisterAllComponents()
	{
		World* world = GetWorld();
		if (RootComponent != nullptr && !RootComponent->IsRegistered())
		{
			RootComponent->RegisterInWorld(world);
		}

		std::vector<Component*> components;
		GetComponents(components);
		int32 registeredComponentsCount = 0;

		while (!m_bAllComponentsRegistered)
		{
			for (Component* component : components)
			{
				if (!component->IsRegistered())
				{
					SceneComponent* unregisteredParent = GetUnregisteredParentComponent(component);
					if (unregisteredParent)
					{
						component = unregisteredParent;
					}
					component->RegisterInWorld(world);
					registeredComponentsCount++;
				}
			}
			if (registeredComponentsCount == components.size())
			{
				m_bAllComponentsRegistered = true;
			}
		}
	}

	void Entity::UnregisterAllComponents()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component->IsRegistered())
			{
				component->UnregisterComponent();
			}
		}

		if (m_bAllComponentsRegistered)
		{
			m_bAllComponentsRegistered = false;
		}
	}

	void Entity::ProcessComponentRegistration(Component* component)
	{
		const bool bOwnerBegunPlay = HasBegunPlay();
		if (!component->IsInitialized() && IsInitialized())
		{
			component->Initialize();
		}

		if (bOwnerBegunPlay)
		{
			component->RegisterTickFunctions(true);
			if (!component->HasBegunPlay())
			{
				component->BeginPlay();
			}
		}
	}

	static void PropagateOnComponentsCreated(Entity* newEntity)
	{
		std::vector<Component*> components;
		newEntity->GetComponents(components);

		for (Component* component : components)
		{
			if (!component->IsCreated())
			{
				component->OnCreated();
			}
		}
	}

	void Entity::OnSpawnInitialize() // TODO: Add Transform
	{
		World* world = GetWorld();
		PropagateOnComponentsCreated(this);
		if (world)
		{
			RegisterAllComponents();
		}
		// Handle Transfrom in this function
		PostConstruction();
	}

	void Entity::PostConstruction()
	{
		World* world = GetWorld();
		const bool bEntitiesInitialized = world && world->AreEntitiesInitialized();
		if (!bEntitiesInitialized) return;

		InitializeComponents();
		// Handle collisions

		if (world && world->HasBegunPlay() && !HasBegunPlay())
		{
			BeginPlay();
		}

	}

	void Entity::RegisterTickFunction(bool bRegister)
	{
		if (bRegister)
		{
			if (DefaultEntityTick.bTickable)
			{
				DefaultEntityTick.TargetObject = this;
				DefaultEntityTick.SetEnable(DefaultEntityTick.bTickOnStart || DefaultEntityTick.IsEnabled());
				DefaultEntityTick.Register(GetLevel());
			}
		}
		else
		{
			if (DefaultEntityTick.IsRegistered())
			{
				DefaultEntityTick.Unregister();
			}
		}
	}

	void Entity::RegisterTickFunctions(bool bRegister, bool bComponents)
	{
		if (!IsPrototype())
		{
			if (m_bTicksRegistered != bRegister)
			{
				RegisterTickFunction(bRegister);
				m_bTicksRegistered = true;
			}
			if (bComponents)
			{
				std::vector<Component*> componenets;
				GetComponents(componenets);
				for (Component* component : componenets)
				{
					if (component)
					{
						component->RegisterTickFunctions(bRegister);
					}
				}
			}
		}
	}

	void Entity::SetTickEnable(bool bEnabled)
	{
		if (DefaultEntityTick.bTickable && !IsPrototype())
		{
			DefaultEntityTick.SetEnable(bEnabled);
		}
	}


	Level* Entity::GetLevel() const
	{
		return GetCreatorAs<Level>();
	}

	WorldSettings* Entity::GetWorldSettings() const
	{
		World* world = GetWorld();
		return world ? world->GetSettings() : nullptr;
	}

	World* Entity::GetWorld() const
	{
		Level* level = GetLevel();
		return level ? level->OwningWorld.Get() : nullptr;
	}

	GlobalGameState* Entity::GetGlobalGameState() const
	{
		return GetWorld()->GetGlobalGameState();
	}

	void Entity::MarkComponentsRenderStateDirty()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component && component->IsRegistered())
			{
				component->MarkRenderStateDirty();
			}
		}
	}

	void Entity::MarkComponentsAsPendingKill()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			component->OnDetroyed(true);
			component->MarkPendingKill();
		}
	}

	void Entity::InitializeComponents()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component->IsRegistered())
			{
				if (!component->IsActive())
				{
					component->SetActive(true);
				}
				if (!component->IsInitialized())
				{
					component->Initialize();
				}
			}
		}
	}

	void Entity::UninitializeComponents()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component->IsInitialized())
			{
				component->Uninitialize();
			}
		}
	}

	void Entity::ProcessBeginPlay()
	{
		World* world = (!HasBegunPlay() ? GetWorld() : nullptr);
		if (world)
		{
			BeginPlay();
		}
	}

	void Entity::ProcessEndPlay()
	{
		if (m_bInitialized)
		{
			if (HasBegunPlay())
			{
				EndPlay();
			}
		}
		UninitializeComponents();
	}

	void Entity::StartDestroy()
	{
		UnregisterAllComponents();
		Level* level = GetLevel();
		if (level)
		{
			auto it = std::find(level->Entities.begin(), level->Entities.end(), this);
			if (it != level->Entities.end())
			{
				level->Entities.erase(it); // TODO: check if i should erase or just make null
			}
		}
		Super::StartDestroy();
	}

	void Entity::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		// TODO: Reigster
	}

	void Entity::BeginPlay()
	{
		VM_CORE_ASSERT(!m_bHasBegunPlay, "BeginPlay called on Entity with begun play true");
		RegisterTickFunctions(true, false);
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component->IsRegistered() && !component->HasBegunPlay())
			{
				component->RegisterTickFunctions(true);
				component->BeginPlay();
			}
		}
		m_bHasBegunPlay = true;
	}

	void Entity::EndPlay()
	{
		if (!m_bHasBegunPlay) return;
		m_bHasBegunPlay = false;

		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component->HasBegunPlay())
			{
				component->EndPlay();
			}
		}

	}

	void Entity::MarkReferencedObjects(GarbageCollector& gc)
	{
		Super::MarkReferencedObjects(gc);
		gc.MarkReachableSet(m_OwnedComponents);
	}

#ifdef VM_WITH_EDITOR
	void Entity::SetLabel(const String& newLabel)
	{
		m_Label = newLabel;
	}
#endif
}

