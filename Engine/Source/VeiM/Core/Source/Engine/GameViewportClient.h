#pragma once
#include "CoreDefines.h"
#include "Engine/Reflection.h"
#include "Engine/CoreObject.h"
#include "Engine/ViewportClient.h"
#include "Engine/EngineTypes.h"


namespace VeiM
{
	class World;
	class GlobalGameState;


	class CORE_API GameViewportClient : public Object, public ViewportClient
	{
		DECLARE_CLASS(GameViewportClient, Object)
	public:
		GameViewportClient();

		void Init(GlobalGameState* ggs);


	public:
		bool bIsEditorPlay;
	protected:
		ObjectPtr<World> m_World;
		ObjectPtr<GlobalGameState> m_GlobalGameState;

	private:
		EMouseCapture m_CaptureMouseCond;
	};
}