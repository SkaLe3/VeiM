#include "EditorEngine.h"
#include "Engine/World.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/GlobalGameState.h"
#include "Engine/GarbageCollector.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Player.h"
#include "Application/Rendering/NativeViewport.h"

#include "Application/Application.h"

namespace VeiM
{
	IMPLEMENT_CLASS(EditorEngine);

	CORE_API EditorEngine* g_Editor = nullptr;

	void EditorEngine::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(EditorEngine, ObjectProperty, PlayWorld);
		REGISTER_PROPERTY(EditorEngine, ObjectProperty, EditorWorld);
	}

	EditorEngine::EditorEngine()
		: bIsInitialized(false),
		bRequestedPlaySession(false),
		bRequestedEndPlaySession(false)
	{

	}

	void EditorEngine::RequestPlaySession(std::weak_ptr<NativeViewport> viewport)
	{
		m_RequestedViewport = viewport;
		bRequestedPlaySession = true;
	}

	void EditorEngine::RequestEndPlaySession()
	{
		bRequestedEndPlaySession = true;
	}

	void EditorEngine::StartPlaySession(std::weak_ptr<NativeViewport> viewport)
	{
		bRequestedPlaySession = false;
		// TODO: Add loading class from maps settings (where default map is set) to be able to instanciate custom user GGS
		GlobalGameState* globalGameState = NewObject<GlobalGameState>(this, StringID("EditorPlayGGS"));
		globalGameState->MarkAsRoot();
		globalGameState->InitForEditorPlay();
		PlayWorld = globalGameState->GetWorld();
		g_World = PlayWorld.Get();
		// TODO: Handle viewports here
			// Create new viewport
			// Init
			// Set GameViewport to this new viewport
		Player* newPlayer = nullptr;
		SharedPtr<NativeViewport> nativeViewport = nullptr;
		GameViewport = NewObject<GameViewportClient>(this, StringID("GameViewportClient"));
		GameViewport->Init(globalGameState);
		GameViewport->bIsEditorPlay = true;
		// Set GameViewport to World, possibly
		// Bind Viewport close event to call request end play session
		globalGameState->CreatePlayer();
		viewport.lock()->StartEditorPlaySession(GameViewport);



		globalGameState->UnmarkAsRoot();
		globalGameState->StartEditorPlay();
		// TODO: Register viewport
	}


	void EditorEngine::EndPlaySession()
	{
		bRequestedEndPlaySession = false;
		World* currentWorld = PlayWorld.Get();

		PlayWorld->CleanupEntities();
		PlayWorld->StartDying();
		PlayWorld->EndPlay();
		// TODO: Shutdown owning game session
		PlayWorld->Cleanup();
		PlayWorld->UnmarkAsRoot();
		PlayWorld = nullptr;
		g_World = EditorWorld.Get();
		// TODO: Recreate render scene
		currentWorld->MarkAllPendingKill();
		// TODO: Delete all objects within associated global game state
		GarbageCollector::Get().CollectGarbage(true);
	}




	void EditorEngine::Tick(float deltaTime)
	{
#if 0
		// TODO: Go through all worlds and check if viewport closed
		if (PlayWorld && PlayWorld->WorldType == EWorldType::EditorPlay && Viewport == nullptr)
		{
			EndPlayMap();
		}
#endif
		// TODO: Tick timer manager

		if (bRequestedPlaySession)
		{
			StartPlaySession(m_RequestedViewport);
		}

		if (PlayWorld)
			PlayWorld->Tick(deltaTime);
		if (EditorWorld && HasRunGCThisFrame())
		{
			EditorWorld->CleanupEntities();
		}

		const bool bEndPlaySession = PlayWorld && bRequestedEndPlaySession;
		if (bEndPlaySession)
		{
			EndPlaySession();
		}

		// Tick Editor viewport to update camera movement
	}

	void EditorEngine::Init()
	{
		g_Editor = this;
		Engine::Init();
		// TODO: Create timer manager

		NewMap();
	}

	World* EditorEngine::NewMap()
	{
		if (PlayWorld)
		{
			EndPlaySession();
		}

		DestroyWorld(EditorWorld.Get());
		World* newWorld = World::CreateWorld(EWorldType::Editor, StringID("EditorWorld"), true);
		EditorWorld = newWorld;
		g_World = newWorld;
		newWorld->UpdateAllComponents();
		return newWorld;
	}

	void EditorEngine::DestroyWorld(World* inWorld)
	{
		if (inWorld == nullptr)
		{
			return;
		}
		inWorld->Destroy();
		GarbageCollector::Get().CollectGarbage(true);
	}

	void EditorEngine::CloseEditor()
	{
		if (PlayWorld)
		{
			EndPlaySession();
		}
		Application::Get().Close();
	}

	World* EditorEngine::CreatePlayWorldDuplication(World* inWorld)
	{
		World* newWorld = World::GetDuplicateForEditorPlay(inWorld);
		newWorld->WorldType = EWorldType::EditorPlay;
		VM_CORE_TRACE("Created Play world by duplicating Editor world");
		return newWorld;
	}

	World* EditorEngine::GetEditorWorld() const
	{
		return EditorWorld.Get();
	}

	World* EditorEngine::GetPlayWorld() const
	{
		return PlayWorld.Get();
	}


}

