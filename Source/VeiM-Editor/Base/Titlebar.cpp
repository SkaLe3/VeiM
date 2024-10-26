#include "Titlebar.h"

#include "VeiM/Core/Application.h"
#include "VeiM/UI/UI.h"
#include "VeiM/UI/Theme.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

namespace VeiM::UI
{
#include "Utils/Embed/VeiM-Icon.embed"
#include "Utils/Embed/WindowIcons.embed"

	TitleBar::TitleBar(const std::string& title) : m_Title(title)
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

	}

	void TitleBar::OnGUI()
	{
		m_IsMaximized = Application::Get().GetWindow().GetWindowMode() == EWindowMode::WindowedFullscreen;

		if (!Application::Get().GetWindow().HasCustomTitlebar())
		{
			RenderMenubar();
			return;
		}

		m_Style.Height = m_IsMaximized ? m_Style.MaximizedHeight : m_Style.RestoredHeight;
		m_Style.WindowPadding = m_IsMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(0.0f, 0.0f);
		const float w = ImGui::GetContentRegionAvail().x;

		ImGui::SetCursorPos(ImVec2(m_Style.WindowPadding.x, m_Style.WindowPadding.y + m_Style.VerticalOffset));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - m_Style.WindowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + m_Style.Height };
		auto* bgDrawList = ImGui::GetBackgroundDrawList();
		auto* fgDrawList = ImGui::GetForegroundDrawList();
		bgDrawList->AddRectFilled(titlebarMin, titlebarMax, UI::Theme::Get().EditorColors.InvalidPrefab);

		RenderLogo();

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - m_Style.WindowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		RenderDragArea(titlebarMin, titlebarMax, w);
		RenderMenubar();
		RenderWindowTitle();
		RenderProjectName();
		RenderButtons();

		ImGui::EndHorizontal();
	}

	void TitleBar::RenderMenubar()
	{
		if (!m_MenubarCallback)
			return;

		const bool isMaximized = Application::Get().GetWindow().GetWindowMode() == EWindowMode::WindowedFullscreen;

		ImGui::SuspendLayout();
		{
			ImGui::SetItemAllowOverlap();
			const float logoHorizontalOffset = m_Style.MenubarOffset.x + ImGui::GetCurrentWindow()->WindowPadding.x;
			ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, isMaximized ? m_Style.MenubarOffset.y + 8 : m_Style.MenubarOffset.y));

			if (Application::Get().GetWindow().HasCustomTitlebar())
			{
				ImVec2 menubarStart = ImGui::GetCursorPos();
				menubarStart.y -= 2;
				const ImRect menuBarRect = { menubarStart, { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() + (isMaximized ? 8 : 0) } };

				ImGui::BeginGroup();
	
				if (UI::BeginMenubar(menuBarRect))
				{

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 10));
					m_MenubarCallback();
					ImGui::PopStyleVar();
				}
				UI::EndMenubar();
				ImGui::Dummy(ImVec2(0, 21)); // Fix for menubar size
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
			if (ImGui::IsItemHovered())
				m_Hovered = false;
		}	
		ImGui::ResumeLayout();
	}

	void TitleBar::RenderProjectName()
	{
		ImVec2 currentCursorPos = ImGui::GetCursorPos();
		float rectOffsetY = -5.f; // Do not draw upper rounded corners

		ImVec2 framePadding = { 20.f, 5.f };
		ImVec2 textSize = { ImGui::CalcTextSize("Project Name").x, 10 };
		ImVec2 itemSize = { textSize.x + framePadding.x * 2.f, 16 + framePadding.y * 2.f - rectOffsetY };

		ImGui::SetCursorPos({ ImGui::GetWindowWidth() - m_Style.ButtonsAreaWidth - itemSize.x - m_Style.ButtonSize.x / 2.0f, m_IsMaximized ? 8 : 0.f });

		UI::ShiftCursorY(rectOffsetY);

		ImDrawList* list = ImGui::GetWindowDrawList();
		list->AddRectFilled(ImGui::GetCursorScreenPos(), { ImGui::GetCursorScreenPos().x + itemSize.x , ImGui::GetCursorScreenPos().y + itemSize.y }, IM_COL32(15, 15, 15, 255), 5.f);

		UI::ShiftCursor(framePadding.x, framePadding.y - rectOffsetY + 1);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
		ImGui::Text("Project Name");
		ImGui::PopStyleColor();
		ImGui::SetCursorPos(currentCursorPos);
	}

	void TitleBar::RenderLogo()
	{
		auto* fgDrawList = ImGui::GetForegroundDrawList();
		float logoWidth = m_AppHeaderIcon->GetWidth();
		float logoHeight = m_AppHeaderIcon->GetHeight();
		const ImVec2 logoOffset(m_Style.LogoOffset.x + m_Style.WindowPadding.x + (m_IsMaximized ? -3 : 0), m_Style.LogoOffset.y + m_Style.WindowPadding.y + m_Style.VerticalOffset + (m_IsMaximized ? 1 : 0));
		const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
		const ImVec2 logoRectMax = { logoRectStart.x + logoWidth * m_Style.LogoSizeMultiplier, logoRectStart.y + logoHeight * m_Style.LogoSizeMultiplier };
		fgDrawList->AddImage((void*)m_AppHeaderIcon->GetData(), logoRectStart, logoRectMax);
	}

	void TitleBar::RenderDebugHelpers(const ImVec2& titlebarMin, const ImVec2& titlebarMax, float w)
	{
		if (m_EnableDebugMode)
		{
			auto* fgDrawList = ImGui::GetForegroundDrawList();
			// DEBUG TITLEBAR BOUNDS
			fgDrawList->AddRect(titlebarMin, titlebarMax, UI::Theme::Get().EditorColors.InvalidPrefab);
			// DEBUG DRAG BOUNDS
			fgDrawList->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - m_Style.ButtonsAreaWidth, ImGui::GetCursorScreenPos().y + m_Style.Height), UI::Theme::Get().EditorColors.InvalidPrefab);
		}
	}

	void TitleBar::RenderWindowTitle()
	{
		ImVec2 currentCursorPos = ImGui::GetCursorPos();
		ImVec2 textSize = ImGui::CalcTextSize(m_Title.c_str());
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, m_Style.VerticalTitleOffset + m_Style.WindowPadding.y));
		ImGui::Text("%s", m_Title.c_str()); // Draw title
		ImGui::SetCursorPos(currentCursorPos);
	}

	void TitleBar::RenderButtons()
	{
		// Window buttons
		const ImU32 buttonColN = UI::Theme::Get().EditorColors.Text;
		const ImU32 buttonColH = UI::Theme::Get().EditorColors.TextBrighter;
		const ImU32 buttonColP = UI::Theme::Get().EditorColors.TextDarker;

		auto& window = Application::Get().GetWindow();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		// Minimize Button
		ImGui::Spring(); 
		UI::ShiftCursorY(11.0f);
		{
			const int iconWidth = m_IconMinimize->GetWidth();
			const int iconHeight = m_IconMinimize->GetHeight();
			const float padY = (m_Style.ButtonSize.y - (float)iconHeight) / 2.0f;
			const float padX = (m_Style.ButtonSize.x - (float)iconWidth) / 2.0f;
			if (ImGui::InvisibleButton("Minimize", ImVec2(m_Style.ButtonSize.x, m_Style.ButtonSize.y)))
			{
				window.SetWindowMode(EWindowMode::Minimized);
			}

			UI::DrawButtonImage(m_IconMinimize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), -padX, -padY));
		}

		// Maximize Button
		ImGui::Spring();
		UI::ShiftCursorY(11.0f);
		{
			const int iconWidth = m_IconMaximize->GetWidth();
			const int iconHeight = m_IconMaximize->GetHeight();
			const float padY = (m_Style.ButtonSize.y - (float)iconHeight) / 2.0f;
			const float padX = (m_Style.ButtonSize.x - (float)iconWidth) / 2.0f;

			if (ImGui::InvisibleButton("Maximize", ImVec2(m_Style.ButtonSize.x, m_Style.ButtonSize.y)))
			{
				if (window.GetWindowMode() == EWindowMode::WindowedFullscreen)
					window.SetWindowMode(EWindowMode::Windowed);
				else
					window.SetWindowMode(EWindowMode::WindowedFullscreen);
			}

			UI::DrawButtonImage(m_IsMaximized ? m_IconRestore : m_IconMaximize, buttonColN, buttonColH, buttonColP, UI::RectExpanded(UI::GetItemRect(), -padX, -padY));
		}

		// Close Button
		ImGui::Spring();
		UI::ShiftCursorY(11.0f);
		{
			const int iconWidth = m_IconClose->GetWidth();
			const int iconHeight = m_IconClose->GetHeight();
			const float padY = (m_Style.ButtonSize.y - (float)iconHeight) / 2.0f;
			const float padX = (m_Style.ButtonSize.x - (float)iconWidth) / 2.0f;
			if (ImGui::InvisibleButton("Close", ImVec2(m_Style.ButtonSize.x + 6 , m_Style.ButtonSize.y)))
				Application::Get().Close();

			UI::DrawButtonImage(m_IconClose, UI::Theme::Get().EditorColors.Text, UI::Colors::ColorWithMultipliedValue(UI::Theme::Get().EditorColors.Text, 1.4f), buttonColP,
								ImRect({ImGui::GetItemRectMin().x + padX, ImGui::GetItemRectMin().y + padY}, {ImGui::GetItemRectMax().x - padX - 6, ImGui::GetItemRectMax().y - padY}));
		}
		ImGui::Spring(-1, 18);
		ImGui::PopStyleVar();
	}

	void TitleBar::RenderDragArea(const ImVec2& titlebarMin, const ImVec2& titlebarMax, float w)
	{
		// Title bar drag area
		ImGui::SetCursorPos(ImVec2(m_Style.WindowPadding.x, m_Style.WindowPadding.y + m_Style.VerticalOffset)); // Reset cursor pos
		RenderDebugHelpers(titlebarMin, titlebarMax, w);
		UI::InvisibleItem("##titleBarDragZone", ImVec2(w - m_Style.ButtonsAreaWidth - m_Style.WindowPadding.x , m_Style.Height));

		m_Hovered = ImGui::IsItemHovered();

		if (m_IsMaximized)
		{
			float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
			if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
				m_Hovered = true; // Account for the top-most pixels which don't register
		}
	}

}

