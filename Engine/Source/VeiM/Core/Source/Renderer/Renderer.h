#pragma once
#include "CoreDefines.h"
#include "Engine/World.h"

namespace VeiM
{
	class IScene;

	class Renderer
	{
	public:
		static Renderer* Get();

		void Startup();
		void Shutdown();

		IScene* CreateScene(World* world);

	private:
		std::unordered_set<IScene*> m_AllScenes;
	};
}