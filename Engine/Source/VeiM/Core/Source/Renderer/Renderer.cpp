#include "Renderer.h"
#include "Renderer/RenderScene.h"

namespace VeiM
{
	static Renderer* s_RendererInstance;

	Renderer* Renderer::Get()
	{	
		return s_RendererInstance;
	}

	void Renderer::Startup()
	{
		s_RendererInstance = new Renderer();
	}

	void Renderer::Shutdown()
	{
		delete s_RendererInstance;
	}

	IScene* Renderer::CreateScene(World* world)
	{
		RenderScene* newScene = new RenderScene(world);
		m_AllScenes.insert(newScene);
		return newScene;
	}

}

