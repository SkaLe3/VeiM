#pragma once
#include "CoreDefines.h"
#include "Engine/Scene.h"

namespace VeiM
{
	class World;

	class RenderScene : public IScene
	{
	public:
		RenderScene(World* world);

	public:
		World* OwningWorld;
	private:
	};
}