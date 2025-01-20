#pragma once

#include "VeiM/Core/Image.h"

#include <imgui.h>
#include <string>
#include <functional>

#include <memory>

namespace VeiM
{
	class Window;
}

namespace VeiM::UI
{
	class TitleBar
	{
	public:
		TitleBar(const std::string& title);

		void OnGUI();

		const bool IsHovered() const { return m_Hovered; }
		const float GetHeight() const { return m_Style.Height; }

		void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }
		const std::function<void()>& GetMenubarCallback() const { return m_MenubarCallback; }

	private:
		void RenderMenubar();
		void RenderProjectName();
		void RenderLogo();
		void RenderDragArea(const ImVec2& titlebarMin, const ImVec2& titlebarMax, float w);
		void RenderWindowTitle();
		void RenderButtons();

		void RenderDebugHelpers(const ImVec2& titlebarMin, const ImVec2& titlebarMax, float w);
	private:
		struct Style
		{
			float Height = 54.0f;
			float RestoredHeight = 54.f;
			float MaximizedHeight = 62.f;
			const float VerticalOffset = 0.0f;

			ImVec2 LogoOffset{ 7.0f, 7.0f };
			float LogoSizeMultiplier = 0.8f;

			ImVec2 MenubarOffset = { 72.0f, 2.0f };
			float ButtonsAreaWidth = 132;
			float VerticalTitleOffset = 8.0f;

			ImVec2 ButtonSize = { 42.0f, 42.0f };
			ImVec2 WindowPadding;
		};

	private:
		std::shared_ptr<VeiM::Image> m_AppHeaderIcon;
		std::shared_ptr<VeiM::Image> m_IconClose;
		std::shared_ptr<VeiM::Image> m_IconMinimize;
		std::shared_ptr<VeiM::Image> m_IconMaximize;
		std::shared_ptr<VeiM::Image> m_IconRestore;
	private:
		std::function<void()> m_MenubarCallback;
		Style m_Style;
		std::string m_Title;
		bool m_Hovered = false;
		bool m_EnableDebugMode = false;
		bool m_IsMaximized;
	};


}