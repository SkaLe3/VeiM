#pragma once
#include "CoreDefines.h"
#include "Engine/CoreObject.h"
#include "Engine/Reflection.h"

namespace VeiM
{
	class Player;

	class CORE_API GlobalGameState : public Object
	{
		DECLARE_CLASS(GlobalGameState, Object)
	public:
#ifdef VM_WITH_EDITOR
		void StartEditorPlay();
		void InitForEditorPlay();
#endif
		void InitForGame();

		Player* CreatePlayer();
		void AddPlayer(Player* newPlayer);
		const std::vector<ObjectPtr<Player>>& GetPlayers() const;

		virtual World* GetWorld() const override;
		class Engine* GetEngine() const;

		// Temp
		virtual void MarkReferencedObjects(GarbageCollector& gc) override;
	private:
		ObjectPtr<World> m_RelatedWorld;
		std::vector<ObjectPtr<Player>> m_Players;
	};
}