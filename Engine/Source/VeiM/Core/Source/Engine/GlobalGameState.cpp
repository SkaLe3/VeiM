#include "GlobalGameState.h"
#include "Engine/World.h"
#include "Engine/CoreObjectStatics.h"
#include "Engine/Engine.h"
#include "Engine/Player.h"
#include "Engine/Controller.h"
#include "Engine/Classes/Editor/EditorEngine.h"

namespace VeiM
{
	IMPLEMENT_CLASS(GlobalGameState)

	void GlobalGameState::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_VECTOR_PROPERTY(GlobalGameState, ObjectProperty, m_Players);
	}
#ifdef VM_WITH_EDITOR
	void GlobalGameState::StartEditorPlay()
	{
		World* playWorld = GetWorld();
		playWorld->BeginPlay();
	}

	void GlobalGameState::InitForEditorPlay()
	{
		EditorEngine* editorEngine = CastObject<EditorEngine>(GetEngine());
		World* editorWorld = editorEngine->GetEditorWorld();
		World* newWorld = editorEngine->CreatePlayWorldDuplication(editorWorld);

		newWorld->SetGlobalGameState(this);
		m_RelatedWorld = newWorld;
		GarbageCollector::Get().CollectGarbage(true);
	}
#endif

	void GlobalGameState::InitForGame()
	{
		m_RelatedWorld = World::CreateWorld(EWorldType::Game, StringID("GameWorld"), true);
		m_RelatedWorld->SetGlobalGameState(this);
	}

	Player* GlobalGameState::CreatePlayer()
	{
		Player* player = NewObject<Player>(GetEngine(), StringID("Player"));
		AddPlayer(player);
		Controller* const newController = CastObject<Controller>(GetWorld()->SpawnEntity(Controller::StaticClass(), Math::Transform::Identity));
		newController->SetPlayer(player);
#ifdef VM_WITH_EDITOR
		newController->SetLabel("PlayerController");
#endif
		return player;
	}

	void GlobalGameState::AddPlayer(Player* newPlayer)
	{
		m_Players.push_back(ObjectPtr(newPlayer));
	}

	const std::vector<ObjectPtr<Player>>& GlobalGameState::GetPlayers() const
	{
		return m_Players;
	}

	World* GlobalGameState::GetWorld() const
	{
		return m_RelatedWorld ? m_RelatedWorld.Get() : nullptr;
	}

	class Engine* GlobalGameState::GetEngine() const
	{
		return CastObject<Engine>(GetCreator());
	}

	void GlobalGameState::MarkReferencedObjects(GarbageCollector& gc)
	{
		Super::MarkReferencedObjects(gc);
	}

}


