#pragma once

#include "VeiM/Image.h"

#include <imgui/imgui.h>
#include <string>
#include <functional>

#include <glm/glm.hpp>
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
		TitleBar(Window* window);
		~TitleBar()
		{
			m_AppHeaderIcon.reset();
			m_IconClose.reset();
			m_IconMinimize.reset();
			m_IconMaximize.reset();
			m_IconRestore.reset();
		}

		void Draw(float& outTitlebarHeight);
		void DrawMenubar();
		const bool IsHovered() const { return m_TitleBarHovered; }
		bool& GetHovered() { return m_TitleBarHovered; }


		void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }
		const std::function<void()>& GetMenubarCallback() const { return m_MenubarCallback; }
	public:
		//Titlebar
		float Height = 30.0f; // vertical size
		float VerticalOffset = -5.0f; // Icons, menubar, logo
		// Logo
		int LogoWidth = 0;
		int LogoHeight = 0;
		glm::vec2 LogoOffsetValues{ 16.0f, 5.0f };

		// Other
		glm::vec2 MenubarOffsetValues = { 16.0f, 6.0f };
		float ButtonsAreaWidth = 94;
		float VerticalTitleOffset = 2.0f;

		glm::vec2 ButtonSize = { 14.0f, 14.0f };
		// Add color values for button, title
	public:
		std::shared_ptr<VeiM::Image> m_AppHeaderIcon;
		std::shared_ptr<VeiM::Image> m_IconClose;
		std::shared_ptr<VeiM::Image> m_IconMinimize;
		std::shared_ptr<VeiM::Image> m_IconMaximize;
		std::shared_ptr<VeiM::Image> m_IconRestore;
	private:
		Window* m_WindowHandle;
		bool m_TitleBarHovered = false;
		std::function<void()> m_MenubarCallback;
		friend class Menubar;
	};


}