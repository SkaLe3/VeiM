#pragma once

#include "VeiM/Core/Window.h"
#include "VeiM/Core/Layer.h"

#include "VeiM/UI/Theme.h"

namespace VeiM
{
	class GUIContext : public Layer
	{
	public:
		GUIContext();

		// Layer interface override
	public:
		void OnAttach() override;
		void OnDetach() override;

	public:
		void EnableDocking(Window&);
		void RenderDockspace();
		void BeginFrame();
		void EndFrame();

	public:
		UI::Theme& GetTheme() { return m_Theme; }
		inline void SetTheme(const UI::Theme& theme) { m_Theme = theme; }
		void UpdateTheme();

	private:
		UI::Theme m_Theme;
	};
}