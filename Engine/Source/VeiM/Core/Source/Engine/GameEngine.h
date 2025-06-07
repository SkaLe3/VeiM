#pragma once
#include "CoreDefines.h"
#include "Engine/Engine.h"
#include "Engine/GlobalGameState.h"

namespace VeiM
{
	class CORE_API GameEngine : public Engine
	{
		DECLARE_CLASS(GameEngine, Engine)
	public:
		virtual void Tick(float deltaTime) override {}
		virtual void Init() override;

	public:
		ObjectPtr<GlobalGameState> GGS;
		// TODO: Viewports? 
	};
}