#pragma once
#include "CoreDefines.h"

#include "RenderInterface/RIObjects.h"

namespace VeiM
{


	class CORE_API RenderInterface
	{
	public:
		virtual ~RenderInterface() {}

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	public:
		virtual SharedPtr<ViewportRI> CreateViewport(uint32 width, uint32 height, bool bFullscreen, EPixelFormat format) = 0;
	};

	RenderInterface* CreatePlatformRenderInterface();

	extern CORE_API RenderInterface* g_RenderInterface;




	namespace RI
	{
		FORCEINLINE SharedPtr<ViewportRI> CreateViewport(uint32 width, uint32 height, bool bFullscreen, EPixelFormat format)
		{
			return g_RenderInterface->CreateViewport(width, height, bFullscreen, format);
		}
	}






}