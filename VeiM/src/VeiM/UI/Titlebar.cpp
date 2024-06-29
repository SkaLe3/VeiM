#include "Titlebar.h"

#include "VeiM/Application.h"
#include "VeiM/ImGui/ImGuiTheme.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

namespace VeiM::UI
{
#include "VeiM/UI/Embed/VeiM-Icon.embed"
#include "VeiM/UI/Embed/WindowIcons.embed"

	TitleBar::TitleBar(Window* window)
		: m_WindowHandle(window)
	{
		{
			uint32_t w, h;
			void* data = Image::Decode(g_VeiMIcon, sizeof(g_VeiMIcon), w, h);
			m_AppHeaderIcon = std::make_shared<VeiM::Image>(w, h, ImageFormat::RGBA);
			m_AppHeaderIcon->SetData(data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Image::Decode(g_WindowMinimizeIcon, sizeof(g_WindowMinimizeIcon), w, h);
			m_IconMinimize = std::make_shared<VeiM::Image>(w, h, ImageFormat::RGBA);
			m_IconMinimize->SetData(data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Image::Decode(g_WindowMaximizeIcon, sizeof(g_WindowMaximizeIcon), w, h);
			m_IconMaximize = std::make_shared<VeiM::Image>(w, h, ImageFormat::RGBA);
			m_IconMaximize->SetData(data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Image::Decode(g_WindowRestoreIcon, sizeof(g_WindowRestoreIcon), w, h);
			m_IconRestore = std::make_shared<VeiM::Image>(w, h, ImageFormat::RGBA);
			m_IconRestore->SetData(data);
			free(data);
		}
		{
			uint32_t w, h;
			void* data = Image::Decode(g_WindowCloseIcon, sizeof(g_WindowCloseIcon), w, h);
			m_IconClose = std::make_shared<VeiM::Image>(w, h, ImageFormat::RGBA);
			m_IconClose->SetData(data);
			free(data);
		}
		LogoWidth = m_AppHeaderIcon->GetWidth();
		LogoHeight = m_AppHeaderIcon->GetHeight();

	}

	void TitleBar::Draw(float& outTitlebarHeight)
	{
		const bool isMaximized = Application::Get().GetWindow().IsMaximized();
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + VerticalOffset));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + Height };
		auto* bgDrawList = ImGui::GetBackgroundDrawList();
		auto* fgDrawList = ImGui::GetForegroundDrawList();
		bgDrawList->AddRectFilled(titlebarMin, titlebarMax, UI::Colors::Theme::titlebar);


		// DEBUG TITLEBAR BOUNDS
		//fgDrawList->AddRect(titlebarMin, titlebarMax, UI::Colors::Theme::invalidPrefab);

		// Logo
		{
			const ImVec2 logoOffset(LogoOffsetValues.x + windowPadding.x, LogoOffsetValues.y + windowPadding.y + VerticalOffset);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + LogoWidth, logoRectStart.y + LogoHeight };
			fgDrawList->AddImage((void*)m_AppHeaderIcon->GetData(), logoRectStart, logoRectMax);
		}

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		static float moveOffsetX;
		static float moveOffsetY;
		const float w = ImGui::GetContentRegionAvail().x;

		// Title bar drag area
		// On Windows we hook into the GLFW win32 window internals
		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + VerticalOffset)); // Reset cursor pos
		// DEBUG DRAG BOUNDS
		//fgDrawList->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - buttonsAreaWidth, ImGui::GetCursorScreenPos().y + titlebarHeight), Utils::Colors::Theme::invalidPrefab);
		ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - ButtonsAreaWidth, Height));

		m_TitleBarHovered = ImGui::IsItemHovered();

		if (isMaximized)
		{
			float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
			if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
				m_TitleBarHovered = true; // Account for the top-most pixels which don't register
		}

		DrawMenubar();

		{
			// Centered Window title
			ImVec2 currentCursorPos = ImGui::GetCursorPos();
			ImVec2 textSize = ImGui::CalcTextSize(Application::Get().GetSpecifications().Name.c_str());
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, VerticalTitleOffset + windowPadding.y));
			ImGui::Text("%s", Application::Get().GetSpecifications().Name.c_str()); // Draw title
			ImGui::SetCursorPos(currentCursorPos);
		}

		// Window buttons
		const ImU32 buttonColN = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 0.9f);
		const ImU32 buttonColH = UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.2f);
		const ImU32 buttonColP = UI::Colors::Theme::textDarker;


		// Minimize Button

		// makes spacing
		ImGui::Spring();
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconMinimize->GetWidth();
			const int iconHeight = m_IconMinimize->GetHeight();
			const float padY = (ButtonSize.y - (float)iconHeight) / 2.0f;
			if (ImGui::InvisibleButton("Minimize", ImVec2(ButtonSize.x, ButtonSize.y)))
			{
				// TODO: move this stuff to a better place, like Window class
				if (Application::Get().GetWindow().GetNativeWindow())
				{
					//Application::Get().QueueEvent([windowHandle = Application::Get().GetWindow().GetNativeWindow()]() { glfwIconifyWindow(windowHandle); });
				}
			}

			UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), 0.0f, -padY));
		}


		// Maximize Button
		ImGui::Spring(-1.0f, 17.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconMaximize->GetWidth();
			const int iconHeight = m_IconMaximize->GetHeight();

			const bool isMaximized = m_WindowHandle->IsMaximized();

			if (ImGui::InvisibleButton("Maximize", ImVec2(ButtonSize.x, ButtonSize.y)))
			{
// 				Application::Get().QueueEvent([isMaximized, windowHandle = m_WindowHandle->GetNativeWindow()]()
// 											  {
// 												  if (isMaximized)
// 													  glfwRestoreWindow(windowHandle);
// 												  else
// 													  glfwMaximizeWindow(windowHandle);
// 											  });
			}

			UI::DrawButtonImage(isMaximized ? m_IconRestore : m_IconMaximize, buttonColN, buttonColH, buttonColP);
		}

		// Close Button
		ImGui::Spring(-1.0f, 15.0f);
		UI::ShiftCursorY(8.0f);
		{
			const int iconWidth = m_IconClose->GetWidth();
			const int iconHeight = m_IconClose->GetHeight();
			if (ImGui::InvisibleButton("Close", ImVec2(ButtonSize.x, ButtonSize.y)))
				Application::Get().Close();

			UI::DrawButtonImage(m_IconClose, UI::Colors::Theme::text, UI::Colors::ColorWithMultipliedValue(UI::Colors::Theme::text, 1.4f), buttonColP);
		}

		ImGui::Spring(-1.0f, 18.0f);
		ImGui::EndHorizontal();

		outTitlebarHeight = Height;

	}

	void TitleBar::DrawMenubar()
	{
		if (!m_MenubarCallback)
			return;

		ImGui::SuspendLayout();
		{
			ImGui::SetItemAllowOverlap();
			const float logoHorizontalOffset = MenubarOffsetValues.x * 2.0f + MenubarOffsetValues.x * 3 + ImGui::GetCurrentWindow()->WindowPadding.x;
			ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, MenubarOffsetValues.y + VerticalOffset));

			if (Application::Get().GetSpecifications().CustomTitleBar)
			{
				const ImRect menuBarRect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

				ImGui::BeginGroup();
				if (UI::BeginMenubar(menuBarRect))
				{
					m_MenubarCallback();
				}

				UI::EndMenubar();
				ImGui::EndGroup();

			}
			else
			{
				if (ImGui::BeginMenuBar())
				{
					m_MenubarCallback();
					ImGui::EndMenuBar();
				}
			}
		}

		if (ImGui::IsItemHovered())
			m_TitleBarHovered = false;


		ImGui::ResumeLayout();

	}
}

