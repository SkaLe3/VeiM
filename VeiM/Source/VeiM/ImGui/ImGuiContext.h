#pragma once

#include "VeiM/Core/Window.h"

namespace VeiM
{
	class ImGuiContext
	{
	public:
		void Init();
		void EnableDocking(Window&);
		void RenderDockspace();
		void BeginFrame();
		void EndFrame();
	};
}