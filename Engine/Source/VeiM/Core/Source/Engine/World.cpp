#include "World.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/Entity.h"
#include "Engine/Level.h"
#include "Engine/Controller.h"
#include "Engine/GlobalGameState.h"
#include "Engine/ServiceEntity.h"
#include "Engine/TickManager.h"
#include "Engine/TimerManager.h"

namespace VeiM
{
	IMPLEMENT_CLASS(World)

	World* g_World;


	World::World() :
		bInitialized(false),
		bTickable(true),
		bWasInitialized(false)
		// Audio device handle
	{
		m_TimerManager = new TimerManager;
	}

	ServiceEntity* World::GetServiceByClass(ClassDescriptor* serviceClass) const
	{
		ServiceEntity* foundService = nullptr;
		for (ObjectPtr<ServiceEntity> service : LevelServices)
		{
			if (service && service->IsA(serviceClass))
			{
				foundService = service.Get();
				break;
			}
		}
		return foundService;
	}

	Level* World::GetCurrentLevel() const
	{
		return CurrentLevel.Get();
	}

	WorldSettings* World::GetSettings()
	{
		return CurrentLevel->GetWorldSettings();
	}

	std::vector<WeakObjectPtr<Controller>>& World::GetControllers()
	{
		return m_Controllers;
	}

	Controller* World::GetController() const
	{
		return m_Controllers[0].Get();
	}

	World* World::GetWorld() const
	{
		return const_cast<World*>(this);
	}

	void World::SetGlobalGameState(GlobalGameState* newGGS)
	{
		m_GlobalGameState = newGGS;
	}

	GlobalGameState* World::GetGlobalGameState() const
	{
		return m_GlobalGameState.Get();
	}

	void World::Tick(float deltaTime)
	{
		bInTick = true;
		bool bIsPaused = IsPaused();
		m_DeltaTimeUnscaled += deltaTime;
		float unscaledDeltaTime = deltaTime;

		deltaTime *= GetSettings()->GetTimeScale();
		const float gameDeltaTime = GetSettings()->ClampDeltaTime(deltaTime);
		deltaTime = gameDeltaTime;
		m_DeltaTime = deltaTime;
		m_DeltaTimeUnscaled = unscaledDeltaTime;

		if (!bIsPaused)
		{
			// Run different tick groups. Only one for now
			TickGroup = ETickGroup::Default;
			TickManager::Get().StartFrame(this, deltaTime, CurrentLevel.Get());
			RunTickGroup(TickGroup);
		}
		else
		{
			// TODO: Run Pause Ticks
		}

		// Run Timer Manager Tick
		// TODO: Update cameras - dont know what exactly to do for it
		if (!bIsPaused)
		{
			TickManager::Get().EndFrame();
		}
	}

	void World::RunTickGroup(ETickGroup group)
	{
		// Supports only one tick group yet
		TickManager::Get().RunTickGroup(group);
		TickGroup = ETickGroup((int32)TickGroup + 1);
	}

	bool World::DestroyEntity(Entity* entity)
	{
		if (entity->IsDying())
			return true;
		entity->ProcessEndPlay();

		// Detaching everything that is attached to this entity
		std::vector<Entity*> attachedEntities;
		entity->GetAttachedEntities(attachedEntities);
		if (!attachedEntities.empty())
		{
			std::vector<SceneComponent*> sceneComponents;
			entity->GetComponents(sceneComponents);

			for (Entity* attachedEntity : attachedEntities)
			{
				if (attachedEntity)
				{
					for (SceneComponent* sceneComponent : sceneComponents)
					{
						attachedEntity->DetachAllSceneComponents(sceneComponent); // TODO: Add detachment rules
					}
				}
			}
		}

		// Detaching this entity from another component
		SceneComponent* rootComponent = entity->GetRootComponent();
		if (rootComponent && rootComponent->GetParent() != nullptr)
		{
			Entity* parentEntity = rootComponent->GetParent()->GetOwner();
			entity->DetachFromEntity(); // TODO: add detachment rules
		}

		RemoveEntity(entity);
		entity->UnregisterAllComponents();
		entity->MarkPendingKill();
		entity->MarkComponentsAsPendingKill();
		entity->RegisterTickFunctions(false, true);
		entity->SetDying(true);
		return true;
	}

	bool World::RemoveEntity(Entity* entity)
	{
		auto it = std::find(CurrentLevel->Entities.begin(), CurrentLevel->Entities.end(), ObjectPtr(entity));
		if (it != CurrentLevel->Entities.end())
		{
			const int32 entityIndex = it - CurrentLevel->Entities.begin();
			CurrentLevel->Entities[entityIndex] = nullptr;
			return true;
		}
		return false;
	}

	Entity* World::SpawnEntity(ClassDescriptor* spawnClass)
	{
		if (!spawnClass->IsChildOf(Entity::StaticClass()))
		{
			VM_CORE_ERROR("Got non Entity subclass in SpawnEntity");
			return nullptr;
		}
		if (bIsDying)
		{
			VM_CORE_ERROR("SpawnEntity called on world that is dying");
			return nullptr;
		}

		StringID entityName = StringID(EStringID::None);
		Entity* entity = NewObject<Entity>(spawnClass, CurrentLevel.Get(), entityName);

#ifdef VM_WITH_EDITOR
		Guid entityGuid = Guid::NewGuid();
		entity->SetGuid(entityGuid);
#endif

		CurrentLevel->AddEntity(entity);
		entity->OnSpawnInitialize();

		return entity;
	}

	void World::InitializeEntities()
	{
		UpdateAllComponents();
		if (!AreEntitiesInitialized())
		{
			bEntitiesInitialized = true;
			CurrentLevel->InitializeEntities();
		}
	}

	void World::BeginPlay()
	{
		for (ObjectPtr<ServiceEntity> service : LevelServices)
		{
			service->StartPlay();	
		}
		GetSettings()->NotifyBeginPlay();
		// PhysicsScene beginplay
	}

	bool World::EndPlay()
	{
		if (!bIsDying)
			StartDying();
		if (!HasBegunPlay())
			return false;

		for (ObjectPtr<Entity> entity : CurrentLevel->Entities)
		{
			if (!entity->IsPendingKill())
			{
				entity->ProcessEndPlay();
			}
		}

		m_bHasBegunPlay = false;
		return true;
	}

	void World::SetBegunPlay(bool bBegun)
	{
		m_bHasBegunPlay = bBegun;
	}

	void World::InitializeNewWorld()
	{
		CurrentLevel = NewObject<Level>(this, StringID("CurrentLevel"));
		CurrentLevel->OwningWorld = this;
		SharedPtr<WorldSettings> worldSettings = MakeShared<WorldSettings>();
		CurrentLevel->SetWorldSettings(worldSettings);

		Initialize();
	}

	void World::Initialize()
	{
		if (bInitialized) return;

		CreatePhysicsScene();
		// Renderer::Get().CreateScene(this); // Scene constructor will set itself to world's field
		CurrentLevel->OwningWorld = this;
		// Set Gravity for physics scene
		// Create collision handler
		bInitialized = true;
		bWasInitialized = true;
	}

	void World::Destroy()
	{
		Cleanup();
		UnmarkAsRoot();
	}

	void World::MarkAllPendingKill()
	{
		auto markPendingKill = [](Object* obj) -> bool
			{
				obj->MarkPendingKill();
				return true;
			};
		GetCreatorSystem().ForEachObjectWithCreator(this, markPendingKill, true);
		MarkPendingKill();
		m_bMarkedAllPendingKill = true;
	}

	void World::ClearAllComponents()
	{
		CurrentLevel->ClearAllComponents();
	}

	void World::UpdateAllComponents()
	{
		CurrentLevel->UpdateAllComponents();
	}

	bool World::HasBegunPlay() const
	{
		return m_bHasBegunPlay && CurrentLevel && !CurrentLevel->Entities.empty();
	}

	bool World::HasMarkedAllPendingKill() const
	{
		return m_bMarkedAllPendingKill;
	}

	bool World::AreEntitiesInitialized() const
	{
		return bEntitiesInitialized && CurrentLevel && !CurrentLevel->Entities.empty();
	}

	bool World::IsPaused()
	{
		return GetSettings()->GetPaused();
	}

	float World::GetDeltaTime() const
	{
		return m_DeltaTime;
	}

	bool World::IsGameWorld() const
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::EditorPlay;
	}

	bool World::IsEditorWorld() const
	{
		return WorldType == EWorldType::Editor || WorldType == EWorldType::EditorPlay;
	}

	bool World::IsToolWorld() const
	{
		return WorldType == EWorldType::EditorTool;
	}

	void World::AddController(Controller* controller)
	{
		m_Controllers.push_back(WeakObjectPtr(controller));
	}

	void World::RemoveController(Controller* controller)
	{
		WeakObjectPtr<Controller> controllerPtr = WeakObjectPtr(controller);
		m_Controllers.erase(std::remove(
			m_Controllers.begin(), m_Controllers.end(), controllerPtr), m_Controllers.end());
	}

	void World::Cleanup()
	{
		// Called after EndPlay

		// Call Cleanup on Renderer Renderer::Get().OnWorldCleanup(this, bIsExit)
		ClearAllComponents();
		CurrentLevel->Cleanup();
		bInitialized = false;
	}

	void World::CleanupEntities()
	{
		std::vector<ObjectPtr<Entity>>& entities = CurrentLevel->Entities;
		auto newEnd = std::remove(entities.begin(), entities.end(), nullptr);
		entities.erase(newEnd, entities.end());
	}

	void World::StartDying()
	{
		VM_CORE_WARN("Called Start Dyinig for world"); // TODO: Remove
		bIsDying = true;
	}

	void World::StartDestroy()
	{
		Super::StartDestroy();
		if (bInitialized)
		{
			Cleanup();
		}
		if (m_PScene)
		{
			//m_PScene->StartDestroy();
		}
		// Set audio device empty
	}

	void World::FinishDestroy()
	{
		if (bWasInitialized)
		{
			// ReleasePhysicsScene();
			if (RScene)
			{
				//RScene->Release()
				RScene = nullptr;
			}
		}
		if (g_World == this)
		{
			g_World = nullptr;
		}
		if (m_TimerManager)
		{
			delete m_TimerManager;
		}
		Super::FinishDestroy();
	}

	void World::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(World, ObjectProperty, CurrentLevel);
	}

	World* World::CreateWorld(const EWorldType worldType, StringID name, bool bAddToRoot)
	{
		const String worldName = name.ToEStringID() == EStringID::None ? "Unnamed" : name.ToString();
		World* newWorld = NewObject<World>(nullptr, StringID(worldName.data()));
		if (bAddToRoot)
		{
			newWorld->MarkAsRoot();
		}
		newWorld->WorldType = worldType;
		newWorld->InitializeNewWorld();

		return newWorld;
	}

}

