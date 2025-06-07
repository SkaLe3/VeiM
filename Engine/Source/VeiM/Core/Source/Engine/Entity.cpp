#include "Entity.h"
#include "Engine/EngineTypes.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/GlobalGameState.h"
#include "Engine/WorldSettings.h"
#include "Engine/InputComponent.h"
#include "Engine/Controller.h"
#include "Engine/GameStatics.h"
#include "Utils/Math.h"

#include <unordered_set>

namespace VeiM
{
	IMPLEMENT_CLASS(Entity);

	static_assert(std::is_base_of_v<Object, InputComponent>,
		"InputComponent must derive from Object");

	void Entity::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(Entity, FloatProperty, TimeScale);
		REGISTER_VECTOR_PROPERTY(Entity, VectorProperty, Tags);
		REGISTER_PROPERTY(Entity, ObjectProperty, EntityInputComponent);
#ifdef VM_WITH_EDITOR
		REGISTER_PROPERTY(Entity, StringProperty, m_Label);
#endif
	}

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

		ControllerIndex = -1;
		TimeScale = 1.0f;
		bBlockInput = false;
		m_bAllComponentsRegistered = false;
		m_bHasBegunPlay = false;
		m_bTicksRegistered = false;
		m_bHidden = false;
		m_bInitialized = false;
		m_bEnableCollision = false;
		m_bDying = false;
#ifdef VM_WITH_EDITOR
		m_Label = "UnnamedEntity";
		m_Guid = Guid::NewGuid();
#endif
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

	void Entity::EnableInput(class Controller* playerController)
	{
		if (!playerController)
		{
			return;
		}
		if (!EntityInputComponent)
		{
			EntityInputComponent = NewObject<InputComponent>(this, StringID((GetName() + "InputComponent").data()));
			EntityInputComponent->RegisterComponent();
			EntityInputComponent->bBlockInput = bBlockInput;
			EntityInputComponent->Priority = InputPriority;
		}
		else
		{
			playerController->PopInputComponent(EntityInputComponent);
		}
		playerController->PushInputComponent(EntityInputComponent);
	}

	void Entity::DisableInput(class Controller* playerController)
	{
		if (!EntityInputComponent)
		{
			return;
		}
		if (playerController)
		{
			playerController->PopInputComponent(EntityInputComponent);
		}
		else if (World* world = GetWorld())
		{
			std::vector<WeakObjectPtr<Controller>>& controllers = GetWorld()->GetControllers();
			for (WeakObjectPtr<Controller> controller : controllers)
			{
				if (controller)
				{
					controller->PopInputComponent(EntityInputComponent);
				}
			}
		}
	}

	void Entity::CreateInputComponent(ClassDescriptor* inputComponentClass)
	{
		if (inputComponentClass && !EntityInputComponent)
		{
			EntityInputComponent = NewObject<InputComponent>(inputComponentClass, this, StringID((GetName() + "InputComponent").data()));
			EntityInputComponent->RegisterComponent();
			EntityInputComponent->bBlockInput = bBlockInput;
			EntityInputComponent->Priority = InputPriority;

			// TODO: Bind delegates
		}
	}

	void Entity::SetTransform(const Transform& newTransform)
	{
		if (RootComponent)
		{
			RootComponent->SetWorldTransform(newTransform);
		}
	}

	const glm::vec3 Entity::GetForwardVector() const
	{
		return RootComponent != nullptr ? RootComponent->GetForwardVector() : Math::Utils::ForwardVector;
	}

	const glm::vec3 Entity::GetUpVector() const
	{
		return RootComponent != nullptr ? RootComponent->GetUpVector() : Math::Utils::UpVector;
	}

	const glm::vec3 Entity::GetRightVector() const
	{
		return RootComponent != nullptr ? RootComponent->GetRightVector() : Math::Utils::RightVector;
	}

	bool Entity::SetLocation(const glm::vec3& newLocation)
	{
		if (RootComponent)
		{
			const glm::vec3 delta = newLocation - GetLocation();
			RootComponent->SetWorldLocation(newLocation);
			return true;
		}
		return false;
	}

	bool Entity::SetRotation(const glm::vec3& newRotation)
	{
		if (RootComponent)
		{
			RootComponent->SetWorldRotation(glm::quat(glm::radians(newRotation)));
			return true;
		}
		return false;
	}

	bool Entity::SetRotation(const glm::quat& newRotation)
	{
		if (RootComponent)
		{
			RootComponent->SetWorldRotation(newRotation);
			return true;
		}
		return false;
	}

	bool Entity::SetScale(const glm::vec3 newScale)
	{
		if (RootComponent)
		{
			RootComponent->SetWorldScale(newScale);
			return true;
		}
		return false;
	}

	bool Entity::SetLocationAndRotation(const glm::vec3& newLocation, const glm::vec3& newRotation)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeLocationAndRotation(newLocation, glm::quat(glm::radians(newRotation)));
			return true;
		}
		return false;
	}

	bool Entity::SetLocationAndRotation(const glm::vec3& newLocation, const glm::quat& newRotation)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeLocationAndRotation(newLocation, newRotation);
			return true;
		}
		return false;
	}

	glm::vec3 Entity::GetScale() const
	{
		if (RootComponent)
		{
			return RootComponent->GetScale();
		}
		return Math::Utils::OnesVector;
	}

	glm::vec3 Entity::GetLocation() const
	{
		return RootComponent->GetLocation();
	}

	glm::vec3 Entity::GetRelativeScale() const
	{
		if (RootComponent)
		{
			return RootComponent->GetRelativeScale();
		}
		return Math::Utils::OnesVector;
	}

	void Entity::AddWorldLocation(const glm::vec3& deltaLocation)
	{
		if (RootComponent)
		{
			RootComponent->AddWorldLocation(deltaLocation);
		}
	}

	void Entity::AddWorldRotation(const glm::vec3& deltaRotation)
	{
		if (RootComponent)
		{
			RootComponent->AddWorldRotation(deltaRotation);
		}
	}

	void Entity::AddWorldRotation(const glm::quat& deltaRotation)
	{
		if (RootComponent)
		{
			RootComponent->AddWorldRotation(deltaRotation);
		}
	}

	void Entity::AddWorldTransform(const Transform& deltaTransform)
	{
		if (RootComponent)
		{
			RootComponent->AddWorldTransform(deltaTransform);
		}
	}

	void Entity::AddWorldTransformKeepScale(const Transform& deltaTransform)
	{
		if (RootComponent)
		{
			RootComponent->AddWorldTransformKeepScale(deltaTransform);
		}
	}

	void Entity::AddLocalLocation(const glm::vec3& deltaLocation)
	{
		if (RootComponent)
		{
			RootComponent->AddLocalLocation(deltaLocation);
		}
	}

	void Entity::AddLocalRotation(const glm::vec3& deltaRotation)
	{
		if (RootComponent)
		{
			RootComponent->AddLocalRotation(deltaRotation);
		}
	}

	void Entity::AddLocalRotation(const glm::quat& deltaRotation)
	{
		if (RootComponent)
		{
			RootComponent->AddLocalRotation(deltaRotation);
		}
	}

	void Entity::AddLocalTransform(const Transform& deltaTransform)
	{
		if (RootComponent)
		{
			RootComponent->AddLocalTransform(deltaTransform);
		}
	}

	void Entity::SetRelativeLocation(const glm::vec3& newLocation)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeLocation(newLocation);
		}
	}

	void Entity::SetRelativeRotation(const glm::vec3& newRotation)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeRotation(newRotation);
		}
	}

	void Entity::SetRelativeRotation(const glm::quat& newRotation)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeRotation(newRotation);
		}
	}

	void Entity::SetRelativeScale(const glm::vec3& newScale)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeScale(newScale);
		}
	}

	void Entity::SetRelativeTransform(const Transform& newTransform)
	{
		if (RootComponent)
		{
			RootComponent->SetRelativeTransform(newTransform);
		}
	}

	float Entity::GetDistanceTo(const Entity* otherEntity) const
	{
		return otherEntity ? glm::distance(GetLocation(), otherEntity->GetLocation()) : 0;
	}

	float Entity::GetSquaredDistanceTo(const Entity* otherEntity) const
	{
		return otherEntity ? glm::distance2(GetLocation(), otherEntity->GetLocation()) : 0;
	}

	float Entity::GetDotProductTo(const Entity* otherEntity) const
	{
		if (otherEntity)
		{
			glm::vec3 dir = GetForwardVector();
			glm::vec3 offset = otherEntity->GetLocation() - GetLocation();
			offset = Math::Utils::SafeNormalize(offset);
			return glm::dot(dir, offset);
		}
	}

	bool Entity::IsHidden() const
	{
		return m_bHidden;
	}

	void Entity::SetHiddenInGame(bool bHidden)
	{
		if (m_bHidden != bHidden)
		{
			m_bHidden = bHidden;
			UpdateComponentsVisibility();
		}
	}

	Component* Entity::AddComponent(ClassDescriptor* componentClass, bool bManualAttach, const Transform& relativeTransform, StringID name)
	{
		if (!componentClass) return nullptr;

		const World* world = GetWorld();
		if (!world) return nullptr;
		if (world->bIsDying) return nullptr;

		Component* newComponent = NewObject<Component>(componentClass, this, name);
		newComponent->m_OwnerCached = this;
		if (!newComponent) return nullptr;

		newComponent->bCreatedNatively = false;

		AddOwnedComponent(newComponent);
		newComponent->OnCreated();

		bool bIsSceneComponent = false;
		SceneComponent* newSceneComponent = CastObject<SceneComponent>(newComponent);
		if (newSceneComponent)
		{
			if (!bManualAttach)
			{
				if (!RootComponent)
				{
					RootComponent = newSceneComponent;
				}
				else
				{
					newSceneComponent->SetupAttachment(RootComponent.Get());
				}
			}
			newSceneComponent->SetRelativeTransform(relativeTransform);
			bIsSceneComponent = true;
		}
		newComponent->RegisterComponent();

		return newComponent;
	}

	bool Entity::AttachToComponent(SceneComponent* parent, const AttachmentTransformRules& rules)
	{
		if (RootComponent && parent)
		{
			return RootComponent->AttachToComponent(parent, rules);
		}
		return false;
	}



	bool Entity::AttachToEntity(Entity* parent, const AttachmentTransformRules& rules)
	{
		if (RootComponent && parent)
		{
			SceneComponent* parentRoot = parent->GetDefaultAttachComponent();
			if (parentRoot)
			{
				return RootComponent->AttachToComponent(parentRoot, rules);
			}
		}
		return false;
	}

	void Entity::DetachFromEntity(const AttachmentTransformRules& rules)
	{
		if (RootComponent)
		{
			RootComponent->DetachFromComponent(rules);
		}
	}

	void Entity::DetachAllSceneComponents(SceneComponent* parent, const AttachmentTransformRules& rules)
	{
		if (parent)
		{
			std::vector<SceneComponent*> components;
			GetComponents(components);

			for (SceneComponent* component : components)
			{
				if (component->GetParent() == parent)
				{
					component->DetachFromComponent(rules);
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

	Component* Entity::GetComponentByName(StringID name) const
	{
		Component* foundComponent = nullptr;
		for (ObjectPtr<Component> component : m_OwnedComponents)
		{
			if (component->m_Name == name)
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
		int32 registeredComponentsCount = 0;
		if (RootComponent != nullptr && !RootComponent->IsRegistered())
		{
			RootComponent->RegisterInWorld(world);
			registeredComponentsCount++;
		}

		std::vector<Component*> components;
		GetComponents(components);

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

	void Entity::OnSpawnInitialize(const Transform& transform, bool bOverrideRootScale)
	{
		World* world = GetWorld();

		SceneComponent* const rootComp = GetRootComponent();
		if (rootComp != nullptr)
		{
			Transform rootTransform(rootComp->GetRelativeRotation(), rootComp->GetRelativeLocation(), rootComp->GetRelativeScale());
			if (bOverrideRootScale)
			{
				rootTransform = transform;
			}
			else
			{
				rootTransform = rootTransform * transform;
			}
			rootComp->SetWorldTransform(rootTransform);
		}

		PropagateOnComponentsCreated(this);
		if (world)
		{
			RegisterAllComponents();
		}
		PostConstruction();
	}

	void Entity::PostConstruction()
	{
		World* world = GetWorld();
		const bool bEntitiesInitialized = world && world->AreEntitiesInitialized();
		if (!bEntitiesInitialized) return;

		PreInitializeComponents();
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

	void Entity::UpdateComponentsVisibility()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			if (component && component->IsRegistered())
			{
				component->OnEntityVisualsChanged();
			}
		}
	}

	void Entity::MarkComponentsAsPendingKill()
	{
		std::vector<Component*> components;
		GetComponents(components);

		for (Component* component : components)
		{
			component->OnDestroyed(true);
			component->MarkPendingKill();
		}
	}

	void Entity::PreInitializeComponents()
	{
		if (ControllerIndex >= 0)
		{
			Controller* controller = GameStatics::GetController(ControllerIndex);
			if (controller)
			{
				EnableInput(controller);
			}
			else
			{
				GetWorld()->CurrentLevel->AddEntityToInputList(this, ControllerIndex);
			}
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
				level->Entities.erase(it);
			}
		}
		Super::StartDestroy();
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

		OnEndPlay.Broadcast(this);

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

	void Entity::Duplicate(Object* sourceObj, Object* destintationObj)
	{
		Entity* sourceEntity = static_cast<Entity*>(sourceObj);
		Entity* destEntity = static_cast<Entity*>(destintationObj);

		if (!sourceEntity || !destEntity)
			return;

		VM_CORE_TRACE("[Object] Duplicating Entity: '{0}'", sourceEntity->GetName());

		for (auto& compPtr : sourceEntity->GetComponents())
		{
			if (compPtr && compPtr->bCreatedNatively)
			{
				Component* sourceComponent = compPtr.Get();
				Component* destComponent = destEntity->GetComponentByName(StringID(compPtr->GetName().data()));
				compPtr->CopyProperties(sourceComponent, destComponent);
				compPtr->Duplicate(sourceComponent, destComponent);

				if (sourceComponent == sourceEntity->GetRootComponent())
				{
					destEntity->SetRootComponent(static_cast<SceneComponent*>(destComponent));
				}

				if (sourceComponent == sourceEntity->EntityInputComponent.Get())
				{
					destEntity->EntityInputComponent = static_cast<InputComponent*>(destComponent);
				}

			}
		}

		for (auto& compPtr : sourceEntity->GetRuntimeComponents())
		{
			if (compPtr)
			{
				Component* component = compPtr.Get();

				Component* duplicatedComponent = static_cast<Component*>(component->DuplicateObject(
					component,
					destEntity,
					component->m_Name,
					component->GetClass()
				));
				duplicatedComponent->bCreatedNatively = false;

				destEntity->AddOwnedComponent(duplicatedComponent);

				if (component == sourceEntity->GetRootComponent())
				{
					destEntity->SetRootComponent(static_cast<SceneComponent*>(duplicatedComponent));
				}

				if (component == sourceEntity->EntityInputComponent.Get())
				{
					destEntity->EntityInputComponent = static_cast<InputComponent*>(duplicatedComponent);
				}
			}
		}
	}

#ifdef VM_WITH_EDITOR

	bool Entity::IsHiddenInEditor() const
	{
		return bHiddenInEditor;
	}

	void Entity::SetLabel(const String& newLabel)
	{
		m_Label = newLabel;
	}
#endif
}

