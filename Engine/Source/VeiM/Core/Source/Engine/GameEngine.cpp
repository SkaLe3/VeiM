#include "GameEngine.h"
#include "Engine/CoreObjectStatics.h"

namespace VeiM
{
	IMPLEMENT_CLASS(GameEngine);

	Engine* g_Engine = nullptr;

	void GameEngine::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		// TODO: Reigster
	}

	void GameEngine::Init()
	{
		Engine::Init();	
		// Load GGS class from config
		GGS = NewObject<GlobalGameState>(this, StringID("GlobalGameState"));
		GGS->InitForGame();
		// TODO: Create new viewport client
		//		Init
		//		Assign to GameViewport
		//		Create sceneviewport
		// TODO: Create Players as many as settings say
		GGS->CreatePlayer();
	}

}

