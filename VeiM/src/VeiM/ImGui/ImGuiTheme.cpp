#include "ImGuiTheme.h"
#include "VeiM/UI/Theme.h"
#include "VeiM/Application.h"

namespace VeiM::UI {

	void SetDefaultTheme()
	{
		VeiM::UI::ApplicationTheme* defaultTheme = new VeiM::UI::ApplicationTheme();

		// Headers
		defaultTheme->Header = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
		defaultTheme->HeaderHovered = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
		defaultTheme->HeaderActive = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);

		// Buttons
		defaultTheme->Button = ImColor(56, 56, 56, 200);
		defaultTheme->ButtonHovered = ImColor(70, 70, 70, 255);
		defaultTheme->ButtonActive = ImColor(56, 56, 56, 150);

		// Frame BG
		defaultTheme->FrameBg = ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);
		defaultTheme->FrameBgHovered = ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);
		defaultTheme->FrameBgActive = ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);

		// Tabs
		defaultTheme->Tab = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		defaultTheme->TabHovered = ImColor(255, 225, 135, 30);
		defaultTheme->TabActive = ImColor(255, 225, 135, 60);
		defaultTheme->TabUnfocused = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		defaultTheme->TabUnfocusedAvtive = defaultTheme->TabHovered;

		// Title
		defaultTheme->TitleBg = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		defaultTheme->TitleBgActive = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		defaultTheme->TitleBgCollapsed = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Resize Grip
		defaultTheme->ResizeGrip = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
		defaultTheme->ResizeGripHovered = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
		defaultTheme->ResizeGripActive = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);

		// Scrollbar
		defaultTheme->ScrollBarBg = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		defaultTheme->ScrollbarGrab = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		defaultTheme->ScrollbarGrabHovered = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		defaultTheme->ScrollbarGrabActive = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

		// Check Mark
		defaultTheme->CheckMark = ImColor(200, 200, 200, 255);

		// Slider
		defaultTheme->SliderGrab = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		defaultTheme->SliderGrabActive = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);

		// Text
		defaultTheme->Text = ImGui::ColorConvertU32ToFloat4(Colors::Theme::text);

		// Checkbox
		defaultTheme->CheckMark = ImGui::ColorConvertU32ToFloat4(Colors::Theme::text);

		// Separator
		defaultTheme->Separator = ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);
		defaultTheme->SeparatorActive = ImGui::ColorConvertU32ToFloat4(Colors::Theme::highlight);
		defaultTheme->SeparatorHovered = ImColor(39, 185, 242, 150);

		// Window Background
		defaultTheme->WindowBg = ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		defaultTheme->ChildBg = ImGui::ColorConvertU32ToFloat4(Colors::Theme::background);
		defaultTheme->PopupBg = ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundPopup);
		defaultTheme->Border = ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);

		// Tables
		defaultTheme->TableHeaderBg = ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
		defaultTheme->TableBorderLight = ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);

		// Menubar
		defaultTheme->MenuBarBg = ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f };

		//========================================================
		/// Style
		defaultTheme->FrameRounding = 2.5f;
		defaultTheme->FrameBorderSize = 1.0f;
		defaultTheme->IndentSpacing = 11.0f;
	}

}