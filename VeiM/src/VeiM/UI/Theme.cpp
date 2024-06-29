#include "Theme.h"

namespace VeiM::UI {


	ApplicationTheme::ApplicationTheme(const ApplicationTheme& theme)
	{
		ImVec4 Header = theme.Header;
		ImVec4 HeaderHovered = theme.HeaderHovered;
		ImVec4 HeaderActive = theme.HeaderActive;

		// Buttons
		ImVec4 Button = theme.Button;
		ImVec4 ButtonHovered = theme.ButtonHovered;
		ImVec4 ButtonActive = theme.ButtonActive;

		// Frame BG
		ImVec4 FrameBg = theme.FrameBg;
		ImVec4 FrameBgHovered = theme.FrameBgHovered;
		ImVec4 FrameBgActive = theme.FrameBgActive;

		// Tabs
		ImVec4 Tab = theme.Tab;
		ImVec4 TabHovered = theme.TabHovered;
		ImVec4 TabActive = theme.TabActive;
		ImVec4 TabUnfocused = theme.TabUnfocused;
		ImVec4 TabUnfocusedAvtive = theme.TabUnfocusedAvtive;

		// Title
		ImVec4 TitleBg = theme.TitleBg;
		ImVec4 TitleBgActive = theme.TitleBgActive;
		ImVec4 TitleBgCollapsed = theme.TitleBgCollapsed;

		//Resize Grip
		ImVec4 ResizeGrip = theme.ResizeGrip;
		ImVec4 ResizeGripHovered = theme.ResizeGripHovered;
		ImVec4 ResizeGripActive = theme.ResizeGripActive;

		// Scrollbar
		ImVec4 ScrollBarBg = theme.ScrollBarBg;
		ImVec4 ScrollbarGrab = theme.ScrollbarGrab;
		ImVec4 ScrollbarGrabHovered = theme.ScrollbarGrabHovered;
		ImVec4 ScrollbarGrabActive = theme.ScrollbarGrabActive;

		// Check Mark
		ImVec4 CheckMark = theme.CheckMark;

		// Slider
		ImVec4 SliderGrab = theme.SliderGrab;
		ImVec4 SliderGrabActive = theme.SliderGrabActive;

		// Text
		ImVec4 Text = theme.Text;

		// Checkbox
		ImVec4 CheckBox = theme.CheckBox;

		// Separator
		ImVec4 Separator = theme.Separator;
		ImVec4 SeparatorActive = theme.SeparatorActive;
		ImVec4 SeparatorHovered = theme.SeparatorHovered;

		// WindowBg
		ImVec4 WindowBg = theme.WindowBg;
		ImVec4 ChildBg = theme.ChildBg;
		ImVec4 PopupBg = theme.PopupBg;
		ImVec4 Border = theme.Border;

		// Tables
		ImVec4 TableHeaderBg = theme.TableHeaderBg;
		ImVec4 TableBorderLight = theme.TableBorderLight;

		// Menubar
		ImVec4 MenuBarBg = theme.MenuBarBg;

		/// Style
		float FrameRounding = theme.FrameRounding;
		float FrameBorderSize = theme.FrameBorderSize;
		float IndentSpacing = theme.IndentSpacing;


		uint32_t accent = theme.accent;
		uint32_t highlight = theme.highlight;
		uint32_t niceBlue = theme.niceBlue;
		uint32_t compliment = theme.compliment;
		uint32_t background = theme.background;
		uint32_t backgroundDark = theme.backgroundDark;
		uint32_t titlebar = theme.titlebar;
		uint32_t propertyField = theme.propertyField;
		uint32_t text = theme.text;
		uint32_t textBrighter = theme.textBrighter;
		uint32_t textDarker = theme.textDarker;
		uint32_t textError = theme.textError;
		uint32_t muted = theme.muted;
		uint32_t groupHeader = theme.groupHeader;
		uint32_t selection = theme.selection;
		uint32_t selectionMuted = theme.selectionMuted;
		uint32_t backgroundPopup = theme.backgroundPopup;
		uint32_t validPrefab = theme.validPrefab;
		uint32_t invalidPrefab = theme.invalidPrefab;
		uint32_t missingMesh = theme.missingMesh;
		uint32_t meshNotSet = theme.meshNotSet;
	}
}