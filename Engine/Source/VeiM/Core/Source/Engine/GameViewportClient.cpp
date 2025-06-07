#include "GameViewportClient.h"
#include "Engine/World.h"
#include "Engine/GlobalGameState.h"


namespace VeiM
{
	IMPLEMENT_CLASS(GameViewportClient);

	void GameViewportClient::RegisterProperties(ClassDescriptor* classDesc)
	{
		Super::RegisterProperties(classDesc);
		REGISTER_PROPERTY(GameViewportClient, ObjectProperty, m_World);
		REGISTER_PROPERTY(GameViewportClient, ObjectProperty, m_GlobalGameState);
	}

	GameViewportClient::GameViewportClient()
		: m_CaptureMouseCond(EMouseCapture::Always)

	{

	}

	void GameViewportClient::Init(GlobalGameState* ggs)
	{
		m_GlobalGameState = ggs;
	}

}

