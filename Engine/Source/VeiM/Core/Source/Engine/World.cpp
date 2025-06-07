#include "World.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/Entity.h"
#include "Engine/Level.h"
#include "Engine/Controller.h"
#include "Engine/GlobalGameState.h"
#include "Engine/ServiceEntity.h"
#include "Engine/TickManager.h"
#include "Engine/TimerManager.h"
#include "Engine/Engine.h"

namespace VeiM
{
	IMPLEMENT_CLASS(World);

	CORE_API World* g_World = nullptr;


	void World::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(World, ObjectProperty, CurrentLevel);
		REGISTER_PROPERTY(World, ObjectProperty, m_GlobalGameState);
	}

	World::World() :
		bInitialized(false),
		bTickable(true),
		bWasInitialized(false),
		bEntitiesInitialized(false),
		bInTick(false),
		bIsDying(false),
		m_bHasBegunPlay(false),
		m_bMarkedAllPendingKill(false) 
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

	Entity* World::GetEntityByName(StringID name)
	{
		std::vector<ObjectPtr<Entity>>& entities = CurrentLevel->Entities;
		for (auto& entityPtr : entities)
		{
			if (entityPtr && entityPtr->GetNameID() == name)
			{
				return entityPtr.Get();
			}
		}
		return nullptr;
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

		if (g_Engine->HasRunGCThisFrame())
		{
			CleanupEntities();
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
						attachedEntity->DetachAllSceneComponents(sceneComponent, AttachmentTransformRules::TransformWorld);
					}
				}
			}
		}

		// Detaching this entity from another component
		SceneComponent* rootComponent = entity->GetRootComponent();
		if (rootComponent && rootComponent->GetParent() != nullptr)
		{
			entity->DetachFromEntity(AttachmentTransformRules::TransformWorld);
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

	Entity* World::SpawnEntity(ClassDescriptor* spawnClass, const Transform& transform, bool bOverrideRootScale, StringID entityName)
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

		Entity* entity = NewObject<Entity>(spawnClass, CurrentLevel.Get(), entityName);

#ifdef VM_WITH_EDITOR
		Guid entityGuid = Guid::NewGuid();
		entity->SetGuid(entityGuid);
#endif

		CurrentLevel->AddEntity(entity);
		entity->OnSpawnInitialize(transform, bOverrideRootScale);

		return entity;
	}

	VeiM::Entity* World::SpawnEntity(ClassDescriptor* spawnClass, const glm::vec3& location, const glm::vec3& rotation, bool bOverrideRootScale, StringID entityName /*= StringID(EStringID::None)*/)
	{
		Transform transform;
		transform.Translation = location;
		transform.Rotation = glm::quat(glm::radians(rotation));
		return SpawnEntity(spawnClass, transform, bOverrideRootScale, entityName);
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

	static String GetWorldTypeString(EWorldType inWorldType)
	{
		switch (inWorldType)
		{
		case VeiM::EWorldType::Game:
			return "Game";
			break;
		case VeiM::EWorldType::Editor:
			return "Editor";
			break;
		case VeiM::EWorldType::EditorPlay:
			return "EditorPlay";
			break;
		case VeiM::EWorldType::EditorTool:
			return "EditorTool";
			break;
		default:
			return "Unknown";
			break;
		}
	}

	void World::InitializeNewWorld()
	{

		StringID levelName = StringID((GetWorldTypeString(WorldType) + "Level").data());
		CurrentLevel = NewObject<Level>(this, levelName);
		CurrentLevel->OwningWorld = this;
		SharedPtr<WorldSettings> worldSettings = MakeShared<WorldSettings>();
		CurrentLevel->SetWorldSettings(worldSettings);

		Initialize();
	}

	void World::Initialize()
	{
		if (bInitialized) return;

		CreatePhysicsScene();

		CurrentLevel->OwningWorld = this;
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

	void World::Duplicate(Object* sourceObj, Object* destintationObj)
	{
		Super::Duplicate(sourceObj, destintationObj);

		World* sourceWorld = static_cast<World*>(sourceObj);
		World* destinationWorld = static_cast<World*>(destintationObj);

		if (!sourceWorld || !destinationWorld)
			return;

		VM_CORE_TRACE("[Object] Duplicating World: '{0}'", sourceWorld->GetName());

		if (sourceWorld->CurrentLevel)
		{
			Level* duplicatedLevel = static_cast<Level*>(sourceWorld->CurrentLevel->DuplicateObject(
				sourceWorld->CurrentLevel.Get(),
				destinationWorld,
				sourceWorld->CurrentLevel->m_Name,
				sourceWorld->CurrentLevel->GetClass()
			));
			destinationWorld->CurrentLevel = duplicatedLevel;
			duplicatedLevel->OwningWorld = destinationWorld;
		}
		if (sourceWorld->GetSettings())
		{
			SharedPtr<WorldSettings> duplicatedSettings = MakeShared<WorldSettings>(*sourceWorld->GetSettings());
			destinationWorld->GetCurrentLevel()->SetWorldSettings(duplicatedSettings);
		}

		// TODO: RenderScene and Physics scene duplication

	}



	World* World::CreateWorld(const EWorldType worldType, StringID name, bool bAddToRoot)
	{
		const String worldName = name == EStringID::None ? "Unnamed" : name.ToString();
		World* newWorld = NewObject<World>(nullptr, StringID(worldName.data()));
		if (bAddToRoot)
		{
			newWorld->MarkAsRoot();
		}
		newWorld->WorldType = worldType;
		newWorld->InitializeNewWorld();

		return newWorld;
	}

	World* World::GetDuplicateForEditorPlay(World* editorWorld)
	{
#ifdef VM_WITH_EDITOR
		if (!editorWorld)
		{
			VM_CORE_ERROR("[World] Cannot duplicate null editor world for Editor Play World");
			return nullptr;
		}

		Object* playWorldObj = editorWorld->DuplicateObject(editorWorld, editorWorld->GetCreator(), StringID("Editor_Play_World"), editorWorld->GetClass());
		World* playWorld = static_cast<World*>(playWorldObj);

		playWorld->WorldType = EWorldType::EditorPlay;
		playWorld->MarkAsRoot();
		playWorld->Initialize();
		playWorld->InitializeEntities();

		editorWorld->ResolveObjectReferences(editorWorld, playWorld);
		return playWorld;
#else
		return nullptr;
#endif
	}

}

