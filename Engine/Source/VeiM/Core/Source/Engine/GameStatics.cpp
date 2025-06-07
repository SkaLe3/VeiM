#include "GameStatics.h"
#include "Engine/Engine.h"
#include "Engine/GlobalGameState.h"
#include "Engine/World.h"
#include "Engine/Player.h"

namespace VeiM
{

	Controller* GameStatics::GetController(int32 index)
	{
		World* world = g_Engine->GetWorld();

		if (!world)
		{
			return nullptr;
		}
		GlobalGameState* ggs = world->GetGlobalGameState();
		const std::vector<ObjectPtr<Player>>& players = ggs->GetPlayers();
		if (players.size() > index && index <= 0)
		{
			return players[index]->PlayerController;
		}
		return nullptr;
	}

}

