#pragma once
#include <imgui.h>
#include <cstdint>
#include <iostream>
namespace VeiM::UI {


	struct ApplicationTheme
	{
		ApplicationTheme() = default;
		ApplicationTheme(const ApplicationTheme& theme);
		~ApplicationTheme() = default;
		// Headers
		ImVec4 Header = ImVec4();
		ImVec4 HeaderHovered = ImVec4();
		ImVec4 HeaderActive = ImVec4();

		// Buttons
		ImVec4 Button = ImVec4();
		ImVec4 ButtonHovered = ImVec4();
		ImVec4 ButtonActive = ImVec4();

		// Frame BG
		ImVec4 FrameBg = ImVec4();
		ImVec4 FrameBgHovered = ImVec4();
		ImVec4 FrameBgActive = ImVec4();

		// Tabs
		ImVec4 Tab = ImVec4();
		ImVec4 TabHovered = ImVec4();
		ImVec4 TabActive = ImVec4();
		ImVec4 TabUnfocused = ImVec4();
		ImVec4 TabUnfocusedAvtive = ImVec4();

		// Title
		ImVec4 TitleBg = ImVec4();
		ImVec4 TitleBgActive = ImVec4();
		ImVec4 TitleBgCollapsed = ImVec4();

		//Resize Grip
		ImVec4 ResizeGrip = ImVec4();
		ImVec4 ResizeGripHovered = ImVec4();
		ImVec4 ResizeGripActive = ImVec4();

		// Scrollbar
		ImVec4 ScrollBarBg = ImVec4();
		ImVec4 ScrollbarGrab = ImVec4();
		ImVec4 ScrollbarGrabHovered = ImVec4();
		ImVec4 ScrollbarGrabActive = ImVec4();

		// Check Mark
		ImVec4 CheckMark = ImVec4();

		// Slider
		ImVec4 SliderGrab = ImVec4();
		ImVec4 SliderGrabActive = ImVec4();

		// Text
		ImVec4 Text = ImVec4();

		// Checkbox
		ImVec4 CheckBox = ImVec4();

		// Separator
		ImVec4 Separator = ImVec4();
		ImVec4 SeparatorActive = ImVec4();
		ImVec4 SeparatorHovered = ImVec4();

		// WindowBg
		ImVec4 WindowBg = ImVec4();
		ImVec4 ChildBg = ImVec4();
		ImVec4 PopupBg = ImVec4();
		ImVec4 Border = ImVec4();

		// Tables
		ImVec4 TableHeaderBg = ImVec4();
		ImVec4 TableBorderLight = ImVec4();

		// Menubar
		ImVec4 MenuBarBg = ImVec4();

		/// Style
		float FrameRounding = 0.0f;
		float FrameBorderSize = 0.0f;
		float IndentSpacing = 0.0f;


		uint32_t accent = IM_COL32(236, 158, 36, 255);
		uint32_t highlight = IM_COL32(39, 185, 242, 255);
		uint32_t niceBlue = IM_COL32(83, 232, 254, 255);
		uint32_t compliment = IM_COL32(78, 151, 166, 255);
		uint32_t background = IM_COL32(36, 36, 36, 255);
		uint32_t backgroundDark = IM_COL32(26, 26, 26, 255);
		uint32_t titlebar = IM_COL32(21, 21, 21, 255);
		uint32_t propertyField = IM_COL32(15, 15, 15, 255);
		uint32_t text = IM_COL32(192, 192, 192, 255);
		uint32_t textBrighter = IM_COL32(210, 210, 210, 255);
		uint32_t textDarker = IM_COL32(128, 128, 128, 255);
		uint32_t textError = IM_COL32(230, 51, 51, 255);
		uint32_t muted = IM_COL32(77, 77, 77, 255);
		uint32_t groupHeader = IM_COL32(47, 47, 47, 255);
		uint32_t selection = IM_COL32(237, 192, 119, 255);
		uint32_t selectionMuted = IM_COL32(237, 201, 142, 23);
		uint32_t backgroundPopup = IM_COL32(50, 50, 50, 255);
		uint32_t validPrefab = IM_COL32(82, 179, 222, 255);
		uint32_t invalidPrefab = IM_COL32(222, 43, 43, 255);
		uint32_t missingMesh = IM_COL32(230, 102, 76, 255);
		uint32_t meshNotSet = IM_COL32(250, 101, 23, 255);
	};
}