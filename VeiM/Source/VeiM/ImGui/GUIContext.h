#pragma once

#include "VeiM/Core/Window.h"
#include "VeiM/Core/Layer.h"

#include "VeiM/UI/Theme.h"

namespace VeiM
{
	struct GUIDebug
	{
		float FlashColorTime = 0.0f;
		ImVec4* FlashColor4 = nullptr;
		ImVec4 FlashColor4Backup = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImU32* FlashColorU32 = nullptr;
		ImU32 FlashColorU32Backup = IM_COL32(255, 255, 255, 255);
	};


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

		GUIDebug& GetDebug() { return m_Debug; }

	private:
		UI::Theme m_Theme;
		GUIDebug m_Debug;

	};
}