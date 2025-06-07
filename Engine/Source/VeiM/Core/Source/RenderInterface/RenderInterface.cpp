#include "RenderInterface.h"
#include "RenderInterfaceBase.h"

#include "OpenGLPlatform/OpenGLInterface.h"

namespace VeiM
{

	RenderInterface* CreatePlatformRenderInterface()
	{
		return OpenGLPlatform::CreateRI();
	}

	RenderInterface* g_RenderInterface;

	void InitRenderer()
	{
		if (g_RenderInterface)
		{
			return;
		}
		g_RenderInterface = CreatePlatformRenderInterface();
		if (g_RenderInterface)
		{
			g_RenderInterface->Init();
		}
	}

	void ShutdownRenderer()
	{
		if (g_RenderInterface)
		{
			g_RenderInterface->Shutdown();
			delete g_RenderInterface;
			g_RenderInterface = nullptr;
		}
	}

}

