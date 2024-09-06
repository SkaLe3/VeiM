#include "Theme.h"

#include "UI.h"
#include "VeiM/Core/Application.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace YAML
{
	template<>
	struct convert<ImVec2>
	{
		static Node encode(const ImVec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, ImVec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
	template<>
	struct convert<ImVec4>
	{
		static Node encode(const ImVec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, ImVec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace VeiM::UI
{
	Theme Theme::m_SavedTheme;
	std::unordered_map<String, YAML::Node> Theme::m_Themes;
	String Theme::m_DefaultThemeName = "DefaultDark";
	String Theme::m_SelectedThemeName = "DefaultDark";
	int32 Theme::m_ThemeIdx = 0;
	std::vector<String> Theme::m_ThemeNames;
	bool Theme::m_NameCollision;
	char Theme::m_NewThemeName[32];
	ImGuiTextFilter Theme::m_ColorFilter;
	std::filesystem::path Theme::m_ConfPath = "VeiM-Editor/Config/Themes.ini";

	YAML::Emitter& operator<<(YAML::Emitter& out, const ImVec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const ImVec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	Theme& Theme::Get()
	{
		return Application::Get().GetGUIContext()->GetTheme();
	}

	void Theme::Editor()
	{
		Theme& theme = Get();

		UI::TextCentered("General");

		ThemeSelector("Themes##Selector");


		// TODO: FontSelector
		ImGui::InputText("New Theme Name", &m_NewThemeName[0], 32);
		if (m_NameCollision)
		{
			ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Get().EditorColors.Negative), "Theme with such name already exists");
		}
		if (m_NewThemeName[0] == '\0')
		{
			ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Get().EditorColors.Negative), "Theme  name is empty");
		}
		ImGui::ShowFontSelector("Fonts##Selector");

		if (ImGui::Button("Save"))
		{
			SaveNewTheme(theme);
			m_SavedTheme = theme;
		}
		ImGui::SameLine();

		static bool showConfirmationDialog = false;
		if (ImGui::Button("Update"))
		{
			if (m_ThemeNames[m_ThemeIdx] != m_DefaultThemeName)
			{
				showConfirmationDialog = true;
				ImGui::OpenPopup("Update Confirmation");
			}
			else
			{
				VM_CORE_WARN("[Editor] An attempt to update the default theme was refused");
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			SetTheme(m_ThemeNames[m_ThemeIdx]);
		}
		UpdateConfirmPopup(showConfirmationDialog);

		ImGui::Separator();
		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Style"))
			{
				ImGui::Spacing();
				UI::TextCentered("Main");
				if (UI::BeginParameterTable("Main"))
				{
					UI::ParameterSlider2("WindowPadding", (float*)&theme.Style.WindowPadding, 0.0f, 20.0f);
					UI::ParameterSlider2("FramePadding", (float*)&theme.Style.FramePadding, 0.0f, 20.0f);
					UI::ParameterSlider2("ItemSpacing", (float*)&theme.Style.ItemSpacing, 0.0f, 20.0f);
					UI::ParameterSlider2("ItemInnerSpacing", (float*)&theme.Style.ItemInnerSpacing, 0.0f, 20.0f);
					UI::ParameterSlider2("TouchExtraPadding", (float*)&theme.Style.TouchExtraPadding, 0.0f, 10.0f);
					UI::ParameterSlider("IndentSpacing", &theme.Style.IndentSpacing, 0.0f, 30.0f);
					UI::ParameterSlider("ScrollbarSize", &theme.Style.ScrollbarSize, 1.0f, 20.0f);
					UI::ParameterSlider("GrabMinSize", &theme.Style.GrabMinSize, 1.0f, 20.0f);
				}
				ImGui::EndTable();
				ImGui::Spacing();
				UI::TextCentered("Borders");
				if (UI::BeginParameterTable("Borders"))
				{
					UI::ParameterSlider("WindowBorderSize", &theme.Style.WindowBorderSize, 0.0f, 1.0f);
					UI::ParameterSlider("ChildBorderSize", &theme.Style.ChildBorderSize, 0.0f, 1.0f);
					UI::ParameterSlider("PopupBorderSize", &theme.Style.PopupBorderSize, 0.0f, 1.0f);
					UI::ParameterSlider("FrameBorderSize", &theme.Style.FrameBorderSize, 0.0f, 1.0f);
					UI::ParameterSlider("TabBorderSize", &theme.Style.TabBorderSize, 0.0f, 1.0f);
					UI::ParameterSlider("TabBarBorderSize", &theme.Style.TabBarBorderSize, 0.0f, 2.0f);
				}
				ImGui::EndTable();
				ImGui::Spacing();
				UI::TextCentered("Rounding");
				if (UI::BeginParameterTable("Rounding"))
				{
					UI::ParameterSlider("WindowRounding", &theme.Style.WindowRounding, 0.0f, 12.0f);
					UI::ParameterSlider("ChildRounding", &theme.Style.ChildRounding, 0.0f, 12.0f);
					UI::ParameterSlider("FrameRounding", &theme.Style.FrameRounding, 0.0f, 12.0f);
					UI::ParameterSlider("PopupRounding", &theme.Style.PopupRounding, 0.0f, 12.0f);
					UI::ParameterSlider("ScrollbarRounding", &theme.Style.ScrollbarRounding, 0.0f, 12.0f);
					UI::ParameterSlider("GrabRounding", &theme.Style.GrabRounding, 0.0f, 12.0f);
					UI::ParameterSlider("TabRounding", &theme.Style.TabRounding, 0.0f, 12.0f);
				}
				ImGui::EndTable();
				ImGui::Spacing();
				UI::TextCentered("Tables");
				if (UI::BeginParameterTable("Tables"))
				{

					UI::ParameterSlider2("CellPadding", (float*)&theme.Style.CellPadding, 0.0f, 20.0f);
					UI::ParameterSlider2("TableAngledHeadersTextAlign", (float*)&theme.Style.TableAngledHeadersTextAlign, 0.0f, 1.0f, "%.2f");
				}
				ImGui::EndTable();
				ImGui::Spacing();
				UI::TextCentered("Widgets");
				if (UI::BeginParameterTable("Widgets"))
				{
					UI::ParameterSlider2("WindowTitleAlign", (float*)&theme.Style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
					int window_menu_button_position = theme.Style.WindowMenuButtonPosition + 1;
					if (UI::ParameterCombo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
						theme.Style.WindowMenuButtonPosition = (ImGuiDir)(window_menu_button_position - 1);
					UI::ParameterCombo("ColorButtonPosition", (int*)&theme.Style.ColorButtonPosition, "Left\0Right\0");
					UI::ParameterSlider2("ButtonTextAlign", (float*)&theme.Style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
					ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
					UI::ParameterSlider("SeparatorTextBorderSize", &theme.Style.SeparatorTextBorderSize, 0.0f, 10.0f);
					ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
					UI::ParameterSlider2("SeparatorTextAlign", (float*)&theme.Style.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
					UI::ParameterSlider2("SeparatorTextPadding", (float*)&theme.Style.SeparatorTextPadding, 0.0f, 40.0f);
					UI::ParameterSlider("LogSliderDeadzone", &theme.Style.LogSliderDeadzone, 0.0f, 12.0f);
				}
				ImGui::EndTable();
				ImGui::Spacing();
				UI::TextCentered("Docking");
				if (UI::BeginParameterTable("Docking"))
				{
					UI::ParameterSlider("DockingSplitterSize", &theme.Style.DockingSeparatorSize, 0.0f, 12.0f);
				}
				ImGui::EndTable();

				ImGui::Spacing();
				UI::TextCentered("Misc");
				if (UI::BeginParameterTable("Misc"))
				{
					UI::ParameterSlider2("DisplayWindowPadding", (float*)&theme.Style.DisplayWindowPadding, 0.0f, 30.0f); // TODO: Add HelpMarker
					UI::ParameterSlider2("DisplaySafeAreaPadding", (float*)&theme.Style.DisplaySafeAreaPadding, 0.0f, 30.0f);

				}
				UI::EndParameterTable();

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Colors"))
			{
				m_ColorFilter.Draw("Filter colors", ImGui::GetFontSize() * 16);
				ImGui::SameLine();
				UI::HelpMarker(
					"In the color list:\n"
					"Left-click on color square to open color picker,\n"
					"Right-click to open edit options menu");
				ImGui::Spacing();
				UI::TextCentered("Editor Palette");
				if (UI::BeginParameterTable("Editor Palette"))
				{

					ColorEdit_EditorPalette("Accent", theme.EditorColors.Accent, m_SavedTheme.EditorColors.Accent);
					ColorEdit_EditorPalette("Highlight", theme.EditorColors.Highlight, m_SavedTheme.EditorColors.Highlight);
					ColorEdit_EditorPalette("NiceBlue", theme.EditorColors.NiceBlue, m_SavedTheme.EditorColors.NiceBlue);
					ColorEdit_EditorPalette("Compliment", theme.EditorColors.Compliment, m_SavedTheme.EditorColors.Compliment);
					ColorEdit_EditorPalette("Background", theme.EditorColors.Background, m_SavedTheme.EditorColors.Background);
					ColorEdit_EditorPalette("BackgroundDark", theme.EditorColors.BackgroundDark, m_SavedTheme.EditorColors.BackgroundDark);
					ColorEdit_EditorPalette("Titlebar", theme.EditorColors.Titlebar, m_SavedTheme.EditorColors.Titlebar);
					ColorEdit_EditorPalette("Border", theme.EditorColors.Border, m_SavedTheme.EditorColors.Border);
					ColorEdit_EditorPalette("PropertyField", theme.EditorColors.PropertyField, m_SavedTheme.EditorColors.PropertyField);
					ColorEdit_EditorPalette("Text", theme.EditorColors.Text, m_SavedTheme.EditorColors.Text);
					ColorEdit_EditorPalette("TextBrighter", theme.EditorColors.TextBrighter, m_SavedTheme.EditorColors.TextBrighter);
					ColorEdit_EditorPalette("TextDarker", theme.EditorColors.TextDarker, m_SavedTheme.EditorColors.TextDarker);
					ColorEdit_EditorPalette("TextError", theme.EditorColors.TextError, m_SavedTheme.EditorColors.TextError);
					ColorEdit_EditorPalette("Muted", theme.EditorColors.Muted, m_SavedTheme.EditorColors.Muted);
					ColorEdit_EditorPalette("GroupHeader", theme.EditorColors.GroupHeader, m_SavedTheme.EditorColors.GroupHeader);
					ColorEdit_EditorPalette("Selection", theme.EditorColors.Selection, m_SavedTheme.EditorColors.Selection);
					ColorEdit_EditorPalette("SelectionMuted", theme.EditorColors.SelectionMuted, m_SavedTheme.EditorColors.SelectionMuted);
					ColorEdit_EditorPalette("BackgroundPopup", theme.EditorColors.BackgroundPopup, m_SavedTheme.EditorColors.BackgroundPopup);
					ColorEdit_EditorPalette("ValidPrefab", theme.EditorColors.ValidPrefab, m_SavedTheme.EditorColors.ValidPrefab);
					ColorEdit_EditorPalette("InvalidPrefab", theme.EditorColors.InvalidPrefab, m_SavedTheme.EditorColors.InvalidPrefab);
					ColorEdit_EditorPalette("MissingMesh", theme.EditorColors.MissingMesh, m_SavedTheme.EditorColors.MissingMesh);
					ColorEdit_EditorPalette("MeshNotSet", theme.EditorColors.MeshNotSet, m_SavedTheme.EditorColors.MeshNotSet);
					ColorEdit_EditorPalette("Positive", theme.EditorColors.Positive, m_SavedTheme.EditorColors.Positive);
					ColorEdit_EditorPalette("PositiveHovered", theme.EditorColors.PositiveHovered, m_SavedTheme.EditorColors.PositiveHovered);
					ColorEdit_EditorPalette("PositiveActive", theme.EditorColors.PositiveActive, m_SavedTheme.EditorColors.PositiveActive);
					ColorEdit_EditorPalette("Negative", theme.EditorColors.Negative, m_SavedTheme.EditorColors.Negative);
					ColorEdit_EditorPalette("NegativeHovered", theme.EditorColors.NegativeHovered, m_SavedTheme.EditorColors.NegativeHovered);
					ColorEdit_EditorPalette("NegativeActive", theme.EditorColors.NegativeActive, m_SavedTheme.EditorColors.NegativeActive);
				}
				UI::EndParameterTable();
				ImGui::Spacing();
				UI::TextCentered("Editor Palette");
				if (UI::BeginParameterTable("GUI Palette"))
				{
					ColorEdit_GUIPalette("Text", theme.GUIColors.Text, m_SavedTheme.GUIColors.Text);
					ColorEdit_GUIPalette("TextDisabled", theme.GUIColors.TextDisabled, m_SavedTheme.GUIColors.TextDisabled);
					ColorEdit_GUIPalette("WindowBg", theme.GUIColors.WindowBg, m_SavedTheme.GUIColors.WindowBg);
					ColorEdit_GUIPalette("ChildBg", theme.GUIColors.ChildBg, m_SavedTheme.GUIColors.ChildBg);
					ColorEdit_GUIPalette("PopupBg", theme.GUIColors.PopupBg, m_SavedTheme.GUIColors.PopupBg);
					ColorEdit_GUIPalette("Border", theme.GUIColors.Border, m_SavedTheme.GUIColors.Border);
					ColorEdit_GUIPalette("BorderShadow", theme.GUIColors.BorderShadow, m_SavedTheme.GUIColors.BorderShadow);
					ColorEdit_GUIPalette("FrameBg", theme.GUIColors.FrameBg, m_SavedTheme.GUIColors.FrameBg);
					ColorEdit_GUIPalette("FrameBgHovered", theme.GUIColors.FrameBgHovered, m_SavedTheme.GUIColors.FrameBgHovered);
					ColorEdit_GUIPalette("FrameBgActive", theme.GUIColors.FrameBgActive, m_SavedTheme.GUIColors.FrameBgActive);
					ColorEdit_GUIPalette("TitleBg", theme.GUIColors.TitleBg, m_SavedTheme.GUIColors.TitleBg);
					ColorEdit_GUIPalette("TitleBgActive", theme.GUIColors.TitleBgActive, m_SavedTheme.GUIColors.TitleBgActive);
					ColorEdit_GUIPalette("TitleBgCollapsed", theme.GUIColors.TitleBgcollapsed, m_SavedTheme.GUIColors.TitleBgcollapsed);
					ColorEdit_GUIPalette("MenuBarBg", theme.GUIColors.MenuBarBg, m_SavedTheme.GUIColors.MenuBarBg);
					ColorEdit_GUIPalette("ScrollbarBg", theme.GUIColors.ScrollbarBg, m_SavedTheme.GUIColors.ScrollbarBg);
					ColorEdit_GUIPalette("ScrollbarGrab", theme.GUIColors.ScrollbarGrab, m_SavedTheme.GUIColors.ScrollbarGrab);
					ColorEdit_GUIPalette("ScrollbarGrabHovered", theme.GUIColors.ScrollbarGrabHovered, m_SavedTheme.GUIColors.ScrollbarGrabHovered);
					ColorEdit_GUIPalette("ScrollbarGrabActive", theme.GUIColors.ScrollbarGrabActive, m_SavedTheme.GUIColors.ScrollbarGrabActive);
					ColorEdit_GUIPalette("CheckMark", theme.GUIColors.CheckMark, m_SavedTheme.GUIColors.CheckMark);
					ColorEdit_GUIPalette("SliderGrab", theme.GUIColors.SliderGrab, m_SavedTheme.GUIColors.SliderGrab);
					ColorEdit_GUIPalette("SliderGrabActive", theme.GUIColors.SliderGrabActive, m_SavedTheme.GUIColors.SliderGrabActive);
					ColorEdit_GUIPalette("Button", theme.GUIColors.Button, m_SavedTheme.GUIColors.Button);
					ColorEdit_GUIPalette("ButtonHovered", theme.GUIColors.ButtonHovered, m_SavedTheme.GUIColors.ButtonHovered);
					ColorEdit_GUIPalette("ButtonActive", theme.GUIColors.ButtonActive, m_SavedTheme.GUIColors.ButtonActive);
					ColorEdit_GUIPalette("Separator", theme.GUIColors.Separator, m_SavedTheme.GUIColors.Separator);
					ColorEdit_GUIPalette("SeparatorHovered", theme.GUIColors.SeparatorHovered, m_SavedTheme.GUIColors.SeparatorHovered);
					ColorEdit_GUIPalette("SeparatorActive", theme.GUIColors.SeparatorActive, m_SavedTheme.GUIColors.SeparatorActive);
					ColorEdit_GUIPalette("ResizeGrip", theme.GUIColors.ResizeGrip, m_SavedTheme.GUIColors.ResizeGrip);
					ColorEdit_GUIPalette("ResizeGripHovered", theme.GUIColors.ResizeGripHovered, m_SavedTheme.GUIColors.ResizeGripHovered);
					ColorEdit_GUIPalette("ResizeGripActive", theme.GUIColors.ResizeGripActive, m_SavedTheme.GUIColors.ResizeGripActive);
					ColorEdit_GUIPalette("Tab", theme.GUIColors.Tab, m_SavedTheme.GUIColors.Tab);
					ColorEdit_GUIPalette("TabHovered", theme.GUIColors.TabHovered, m_SavedTheme.GUIColors.TabHovered);
					ColorEdit_GUIPalette("TabActive", theme.GUIColors.TabActive, m_SavedTheme.GUIColors.TabActive);
					ColorEdit_GUIPalette("TabUnfocused", theme.GUIColors.TabUnfocused, m_SavedTheme.GUIColors.TabUnfocused);
					ColorEdit_GUIPalette("TabUnfocusedActive", theme.GUIColors.TabUnfocusedActive, m_SavedTheme.GUIColors.TabUnfocusedActive);
					ColorEdit_GUIPalette("DockingPreview", theme.GUIColors.DockingPreview, m_SavedTheme.GUIColors.DockingPreview);
					ColorEdit_GUIPalette("DockingEmptyBg", theme.GUIColors.DockingEmptyBg, m_SavedTheme.GUIColors.DockingEmptyBg);
					ColorEdit_GUIPalette("PlotLines", theme.GUIColors.PlotLines, m_SavedTheme.GUIColors.PlotLines);
					ColorEdit_GUIPalette("PlotLineHovered", theme.GUIColors.PlotLineHovered, m_SavedTheme.GUIColors.PlotLineHovered);
					ColorEdit_GUIPalette("PlotHistogram", theme.GUIColors.PlotHistogram, m_SavedTheme.GUIColors.PlotHistogram);
					ColorEdit_GUIPalette("PlotHistogramHovered", theme.GUIColors.PlotHistogramHovered, m_SavedTheme.GUIColors.PlotHistogramHovered);
					ColorEdit_GUIPalette("TableHeaderBg", theme.GUIColors.TableHeaderBg, m_SavedTheme.GUIColors.TableHeaderBg);
					ColorEdit_GUIPalette("TableBorderStrong", theme.GUIColors.TableBorderStrong, m_SavedTheme.GUIColors.TableBorderStrong);
					ColorEdit_GUIPalette("TableBorderLight", theme.GUIColors.TableBorderLight, m_SavedTheme.GUIColors.TableBorderLight);
					ColorEdit_GUIPalette("TableRowBg", theme.GUIColors.TableRowBg, m_SavedTheme.GUIColors.TableRowBg);
					ColorEdit_GUIPalette("TableRowBgAlt", theme.GUIColors.TableRowBgAlt, m_SavedTheme.GUIColors.TableRowBgAlt);
					ColorEdit_GUIPalette("TextSelectedBg", theme.GUIColors.TextSelectedBg, m_SavedTheme.GUIColors.TextSelectedBg);
					ColorEdit_GUIPalette("DragDropTarget", theme.GUIColors.DragDropTarget, m_SavedTheme.GUIColors.DragDropTarget);
					ColorEdit_GUIPalette("NavHighlight", theme.GUIColors.NavHighlight, m_SavedTheme.GUIColors.NavHighlight);
					ColorEdit_GUIPalette("NavWindowingHighlight", theme.GUIColors.NavWindowingHighlight, m_SavedTheme.GUIColors.NavWindowingHighlight);
					ColorEdit_GUIPalette("NavWindowingDimBg", theme.GUIColors.NavWindowingDimBg, m_SavedTheme.GUIColors.NavWindowingDimBg);
					ColorEdit_GUIPalette("ModalWindowDibBg", theme.GUIColors.ModalWindowDibBg, m_SavedTheme.GUIColors.ModalWindowDibBg);
				}
				UI::EndParameterTable();

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

	}

	Theme Theme::GetDefault()
	{
		return LoadTheme(m_DefaultThemeName);
	}

	Theme Theme::GetSelected()
	{
		return LoadTheme(m_SelectedThemeName);
	}

	void Theme::UpdateConfirmPopup(bool& open)
	{
		ImVec2 popupSize(480, 110);

		ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
		ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;

		ImVec2 popupPos = ImVec2(
			viewportPos.x + (viewportSize.x - popupSize.x) * 0.5f,
			viewportPos.y + (viewportSize.y - popupSize.y) * 0.5f
		);

		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);

		if (ImGui::BeginPopupModal("Update Confirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.8, 0.2, 0.1, 1 });
			const char* text1 = "Updating the theme will permanently overwrite your current settings";
			const char* text2 = "This change cannot be undone";
			const char* text3 = "Are you sure you want to continue ?";

			ImVec2 textSize = ImGui::CalcTextSize(text1);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
			ImGui::Text(text1);
			textSize = ImGui::CalcTextSize(text2);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
			ImGui::Text(text2);
			textSize = ImGui::CalcTextSize(text3);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
			ImGui::Text(text3);

			ImGui::PopStyleColor();
			ImGui::NewLine();

			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 padding = style.WindowPadding;
			ImVec2 buttonSize(100, 0);
			float gap = 6;
			float contentWidth = ImGui::GetContentRegionAvail().x;
			float buttonWidth = buttonSize.x;
			float spacing = (contentWidth - 2 * buttonWidth - gap) / 2 + padding.x;
			float leftButtonOffset = spacing;
			float rightButtonOffset = spacing + buttonWidth + gap;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.7, 0.3, 0.2, 1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.8, 0.3, 0.2, 1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5, 0.2, 0.1, 1 });

			ImGui::SetCursorPosX(leftButtonOffset);
			if (ImGui::Button("Confirm", buttonSize))
			{
				open = false;
				ImGui::CloseCurrentPopup();
				UpdateTheme(Get());
				m_SavedTheme = Get();

			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3, 0.7, 0.2, 1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3, 0.8, 0.2, 1 });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2, 0.5, 0.1, 1 });
			ImGui::SetCursorPosX(rightButtonOffset);
			if (ImGui::Button("Cancel", buttonSize))
			{
				open = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor(6);
			ImGui::SetItemDefaultFocus();
			ImGui::EndPopup();
		}
	}

	void Theme::SaveNewTheme(Theme& theme)
	{
		m_NameCollision = std::find(m_ThemeNames.begin(), m_ThemeNames.end(), m_NewThemeName) != m_ThemeNames.end();
		if (m_NameCollision)
		{
			VM_CORE_ERROR("[Editor] Theme with such name already exists");
			return;
		}
		if (m_NewThemeName[0] == '\0')
		{
			VM_CORE_ERROR("[Editor] Theme name is empty");
			return;
		}
		m_SelectedThemeName = m_NewThemeName;

		YAML::Emitter out;
		EmitThemesContent(out);

		YAML::Node newTheme = FillNodeWith(theme);
		out << YAML::Key << m_NewThemeName << YAML::Value << newTheme;

		EndEmit(out);
		UploadContent(out);

		bool loaded = TryLoadThemes();
		if (!loaded)
		{
			VM_CORE_ERROR("[Editor] Failed to update themes after save");
			return;
		}
	
	}
	void Theme::LoadThemes()
	{
		YAML::Node config = YAML::LoadFile(m_ConfPath.string());
		m_DefaultThemeName = config["general"]["default_theme"].as<String>();
		m_SelectedThemeName = config["general"]["selected_theme"].as<String>();
		VM_CORE_ASSERT(!m_DefaultThemeName.empty(), "[Editor] Default theme section is missing in the Themes.ini file.");	  // TODO: Make as verify
		if (m_SelectedThemeName.empty())
			m_SelectedThemeName = m_DefaultThemeName;

		YAML::Node themesNode = config["themes"];
		VM_CORE_ASSERT(themesNode, "[Editor] Themes section is missing in the Themes.ini  file."); // TODO: Make as verify

		for (const auto& theme : themesNode)
		{
			std::string themeName = theme.first.as<std::string>();
			YAML::Node themeProperties = theme.second;
			m_Themes[themeName] = themeProperties;
		}
	}

	Theme Theme::LoadTheme(const String& themeName)
	{
		Theme theme;
		YAML::Node dt = m_Themes[themeName];

		YAML::Node style = dt["style"];
		YAML::Node editorpalette = dt["editorpalette"];
		YAML::Node guipalette = dt["guipalette"];

		GUIStyle& gs = theme.Style;
		GUIPalette& gp = theme.GUIColors;
		EditorPalette& ep = theme.EditorColors;

		{
			gs.Alpha = style["Alpha"].as<float>();
			gs.DisabledAlpha = style["DisabledAlpha"].as<float>();
			gs.WindowPadding = style["WindowPadding"].as<ImVec2>();
			gs.WindowRounding = style["WindowRounding"].as<float>();
			gs.WindowBorderSize = style["WindowBorderSize"].as<float>();
			gs.WindowMinSize = style["WindowMinSize"].as<ImVec2>();
			gs.WindowTitleAlign = style["WindowTitleAlign"].as<ImVec2>();
			gs.WindowMenuButtonPosition = (ImGuiDir)style["WindowMenuButtonPosition"].as<int>();
			gs.ChildRounding = style["ChildRounding"].as<float>();
			gs.ChildBorderSize = style["ChildBorderSize"].as<float>();
			gs.PopupRounding = style["PopupRounding"].as<float>();
			gs.PopupBorderSize = style["PopupBorderSize"].as<float>();
			gs.FramePadding = style["FramePadding"].as<ImVec2>();
			gs.FrameRounding = style["FrameRounding"].as<float>();
			gs.FrameBorderSize = style["FrameBorderSize"].as<float>();
			gs.ItemSpacing = style["ItemSpacing"].as<ImVec2>();
			gs.ItemInnerSpacing = style["ItemInnerSpacing"].as<ImVec2>();
			gs.CellPadding = style["CellPadding"].as<ImVec2>();
			gs.TouchExtraPadding = style["TouchExtraPadding"].as<ImVec2>();
			gs.IndentSpacing = style["IndentSpacing"].as<float>();
			gs.ColumnsMinSpacing = style["ColumnsMinSpacing"].as<float>();
			gs.ScrollbarSize = style["ScrollbarSize"].as<float>();
			gs.ScrollbarRounding = style["ScrollbarRounding"].as<float>();
			gs.GrabMinSize = style["GrabMinSize"].as<float>();
			gs.GrabRounding = style["GrabRounding"].as<float>();
			gs.LayoutAlign = style["LayoutAlign"].as<float>();
			gs.LogSliderDeadzone = style["LogSliderDeadzone"].as<float>();
			gs.TabRounding = style["TabRounding"].as<float>();
			gs.TabBorderSize = style["TabBorderSize"].as<float>();
			gs.TabMinWidthForCloseButton = style["TabMinWidthForCloseButton"].as<float>();
			gs.TabBarBorderSize = style["TabBarBorderSize"].as<float>();
			gs.TableAngledHeadersAngle = style["TableAngledHeadersAngle"].as<float>();
			gs.TableAngledHeadersTextAlign = style["TableAngledHeadersTextAlign"].as<ImVec2>();
			gs.ColorButtonPosition = (ImGuiDir)style["ColorButtonPosition"].as<int>();
			gs.ButtonTextAlign = style["ButtonTextAlign"].as<ImVec2>();
			gs.SelectableTextAlign = style["SelectableTextAlign"].as<ImVec2>();
			gs.SeparatorTextBorderSize = style["SeparatorTextBorderSize"].as<float>();
			gs.SeparatorTextAlign = style["SeparatorTextAlign"].as<ImVec2>();
			gs.SeparatorTextPadding = style["SeparatorTextPadding"].as<ImVec2>();
			gs.DisplayWindowPadding = style["DisplayWindowPadding"].as<ImVec2>();
			gs.DisplaySafeAreaPadding = style["DisplaySafeAreaPadding"].as<ImVec2>();
			gs.DockingSeparatorSize = style["DockingSeparatorSize"].as<float>();
			gs.MouseCursorScale = style["MouseCursorScale"].as<float>();
			gs.AntiAliasedLines = style["AntiAliasedLines"].as<bool>();
			gs.AntiAliasedLinesUseTex = style["AntiAliasedLinesUseTex"].as<bool>();
			gs.AntiAliasedFill = style["AntiAliasedFill"].as<bool>();
			gs.CurveTessellationTol = style["CurveTessellationTol"].as<float>();
			gs.CircleTessellationMaxError = style["CircleTessellationMaxError"].as<float>();

			ep.Accent = ImGui::ColorConvertFloat4ToU32(editorpalette["Accent"].as<ImVec4>());
			ep.Highlight = ImGui::ColorConvertFloat4ToU32(editorpalette["Highlight"].as<ImVec4>());
			ep.NiceBlue = ImGui::ColorConvertFloat4ToU32(editorpalette["NiceBlue"].as<ImVec4>());
			ep.Compliment = ImGui::ColorConvertFloat4ToU32(editorpalette["Compliment"].as<ImVec4>());
			ep.Background = ImGui::ColorConvertFloat4ToU32(editorpalette["Background"].as<ImVec4>());
			ep.BackgroundDark = ImGui::ColorConvertFloat4ToU32(editorpalette["BackgroundDark"].as<ImVec4>());
			ep.Titlebar = ImGui::ColorConvertFloat4ToU32(editorpalette["Titlebar"].as<ImVec4>());
			ep.Border = ImGui::ColorConvertFloat4ToU32(editorpalette["Border"].as<ImVec4>());
			ep.PropertyField = ImGui::ColorConvertFloat4ToU32(editorpalette["PropertyField"].as<ImVec4>());
			ep.Text = ImGui::ColorConvertFloat4ToU32(editorpalette["Text"].as<ImVec4>());
			ep.TextBrighter = ImGui::ColorConvertFloat4ToU32(editorpalette["TextBrighter"].as<ImVec4>());
			ep.TextDarker = ImGui::ColorConvertFloat4ToU32(editorpalette["TextDarker"].as<ImVec4>());
			ep.TextError = ImGui::ColorConvertFloat4ToU32(editorpalette["TextError"].as<ImVec4>());
			ep.Muted = ImGui::ColorConvertFloat4ToU32(editorpalette["Muted"].as<ImVec4>());
			ep.GroupHeader = ImGui::ColorConvertFloat4ToU32(editorpalette["GroupHeader"].as<ImVec4>());
			ep.Selection = ImGui::ColorConvertFloat4ToU32(editorpalette["Selection"].as<ImVec4>());
			ep.SelectionMuted = ImGui::ColorConvertFloat4ToU32(editorpalette["SelectionMuted"].as<ImVec4>());
			ep.BackgroundPopup = ImGui::ColorConvertFloat4ToU32(editorpalette["BackgroundPopup"].as<ImVec4>());
			ep.ValidPrefab = ImGui::ColorConvertFloat4ToU32(editorpalette["ValidPrefab"].as<ImVec4>());
			ep.InvalidPrefab = ImGui::ColorConvertFloat4ToU32(editorpalette["InvalidPrefab"].as<ImVec4>());
			ep.MissingMesh = ImGui::ColorConvertFloat4ToU32(editorpalette["MissingMesh"].as<ImVec4>());
			ep.MeshNotSet = ImGui::ColorConvertFloat4ToU32(editorpalette["MeshNotSet"].as<ImVec4>());
			ep.Positive = ImGui::ColorConvertFloat4ToU32(editorpalette["Positive"].as<ImVec4>());
			ep.PositiveHovered = ImGui::ColorConvertFloat4ToU32(editorpalette["PositiveHovered"].as<ImVec4>());
			ep.PositiveActive = ImGui::ColorConvertFloat4ToU32(editorpalette["PositiveActive"].as<ImVec4>());
			ep.Negative = ImGui::ColorConvertFloat4ToU32(editorpalette["Negative"].as<ImVec4>());
			ep.NegativeHovered = ImGui::ColorConvertFloat4ToU32(editorpalette["NegativeHovered"].as<ImVec4>());
			ep.NegativeActive = ImGui::ColorConvertFloat4ToU32(editorpalette["NegativeActive"].as<ImVec4>());

			gp.Text = guipalette["Text"].as<ImVec4>();
			gp.TextDisabled = guipalette["TextDisabled"].as<ImVec4>();
			gp.WindowBg = guipalette["WindowBg"].as<ImVec4>();
			gp.ChildBg = guipalette["ChildBg"].as<ImVec4>();
			gp.PopupBg = guipalette["PopupBg"].as<ImVec4>();
			gp.Border = guipalette["Border"].as<ImVec4>();
			gp.BorderShadow = guipalette["BorderShadow"].as<ImVec4>();
			gp.FrameBg = guipalette["FrameBg"].as<ImVec4>();
			gp.FrameBgHovered = guipalette["FrameBgHovered"].as<ImVec4>();
			gp.FrameBgActive = guipalette["FrameBgActive"].as<ImVec4>();
			gp.TitleBg = guipalette["TitleBg"].as<ImVec4>();
			gp.TitleBgActive = guipalette["TitleBgActive"].as<ImVec4>();
			gp.TitleBgcollapsed = guipalette["TitleBgcollapsed"].as<ImVec4>();
			gp.MenuBarBg = guipalette["MenuBarBg"].as<ImVec4>();
			gp.ScrollbarBg = guipalette["ScrollbarBg"].as<ImVec4>();
			gp.ScrollbarGrab = guipalette["ScrollbarGrab"].as<ImVec4>();
			gp.ScrollbarGrabHovered = guipalette["ScrollbarGrabHovered"].as<ImVec4>();
			gp.ScrollbarGrabActive = guipalette["ScrollbarGrabActive"].as<ImVec4>();
			gp.CheckMark = guipalette["CheckMark"].as<ImVec4>();
			gp.SliderGrab = guipalette["SliderGrab"].as<ImVec4>();
			gp.SliderGrabActive = guipalette["SliderGrabActive"].as<ImVec4>();
			gp.Button = guipalette["Button"].as<ImVec4>();
			gp.ButtonHovered = guipalette["ButtonHovered"].as<ImVec4>();
			gp.ButtonActive = guipalette["ButtonActive"].as<ImVec4>();
			gp.Header = guipalette["Header"].as<ImVec4>();
			gp.HeaderHovered = guipalette["HeaderHovered"].as<ImVec4>();
			gp.HeaderActive = guipalette["HeaderActive"].as<ImVec4>();
			gp.Separator = guipalette["Separator"].as<ImVec4>();
			gp.SeparatorHovered = guipalette["SeparatorHovered"].as<ImVec4>();
			gp.SeparatorActive = guipalette["SeparatorActive"].as<ImVec4>();
			gp.ResizeGrip = guipalette["ResizeGrip"].as<ImVec4>();
			gp.ResizeGripHovered = guipalette["ResizeGripHovered"].as<ImVec4>();
			gp.ResizeGripActive = guipalette["ResizeGripActive"].as<ImVec4>();
			gp.Tab = guipalette["Tab"].as<ImVec4>();
			gp.TabHovered = guipalette["TabHovered"].as<ImVec4>();
			gp.TabActive = guipalette["TabActive"].as<ImVec4>();
			gp.TabUnfocused = guipalette["TabUnfocused"].as<ImVec4>();
			gp.TabUnfocusedActive = guipalette["TabUnfocusedActive"].as<ImVec4>();
			gp.DockingPreview = guipalette["DockingPreview"].as<ImVec4>();
			gp.DockingEmptyBg = guipalette["DockingEmptyBg"].as<ImVec4>();
			gp.PlotLines = guipalette["PlotLines"].as<ImVec4>();
			gp.PlotLineHovered = guipalette["PlotLineHovered"].as<ImVec4>();
			gp.PlotHistogram = guipalette["PlotHistogram"].as<ImVec4>();
			gp.PlotHistogramHovered = guipalette["PlotHistogramHovered"].as<ImVec4>();
			gp.TableHeaderBg = guipalette["TableHeaderBg"].as<ImVec4>();
			gp.TableBorderStrong = guipalette["TableBorderStrong"].as<ImVec4>();
			gp.TableBorderLight = guipalette["TableBorderLight"].as<ImVec4>();
			gp.TableRowBg = guipalette["TableRowBg"].as<ImVec4>();
			gp.TableRowBgAlt = guipalette["TableRowBgAlt"].as<ImVec4>();
			gp.TextSelectedBg = guipalette["TextSelectedBg"].as<ImVec4>();
			gp.DragDropTarget = guipalette["DragDropTarget"].as<ImVec4>();
			gp.NavHighlight = guipalette["NavHighlight"].as<ImVec4>();
			gp.NavWindowingHighlight = guipalette["NavWindowingHighlight"].as<ImVec4>();
			gp.NavWindowingDimBg = guipalette["NavWindowingDimBg"].as<ImVec4>();
			gp.ModalWindowDibBg = guipalette["ModalWindowDibBg"].as<ImVec4>();

		}

		return theme;
	}

	void Theme::InitConfig()
	{
		Theme theme;
		GUIStyle& gs = theme.Style;
		GUIPalette& gp = theme.GUIColors;
		EditorPalette& ep = theme.EditorColors;

		gs.Alpha = 1.0f;
		gs.DisabledAlpha = 0.60f;
		gs.WindowPadding = ImVec2(8, 8);
		gs.WindowRounding = 0.0f;
		gs.WindowBorderSize = 1.0f;
		gs.WindowMinSize = ImVec2(32, 32);
		gs.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		gs.WindowMenuButtonPosition = ImGuiDir_Left;
		gs.ChildRounding = 0.0f;
		gs.ChildBorderSize = 1.0f;
		gs.PopupRounding = 0.0f;
		gs.PopupBorderSize = 1.0f;
		gs.FramePadding = ImVec2(4, 3);
		gs.FrameRounding = 0.0f;
		gs.FrameBorderSize = 0.0f;
		gs.ItemSpacing = ImVec2(8, 4);
		gs.ItemInnerSpacing = ImVec2(4, 4);
		gs.CellPadding = ImVec2(4, 2);
		gs.TouchExtraPadding = ImVec2(0, 0);
		gs.IndentSpacing = 21.0f;
		gs.ColumnsMinSpacing = 6.0f;
		gs.ScrollbarSize = 14.0f;
		gs.ScrollbarRounding = 9.0f;
		gs.GrabMinSize = 12.0f;
		gs.GrabRounding = 0.0f;
		gs.LayoutAlign = 0.5f;
		gs.LogSliderDeadzone = 4.0f;
		gs.TabRounding = 4.0f;
		gs.TabBorderSize = 0.0f;
		gs.TabMinWidthForCloseButton = 0.0f;
		gs.TabBarBorderSize = 1.0f;
		gs.TableAngledHeadersAngle = 35.0f * (IM_PI / 180.0f);
		gs.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.0f);
		gs.ColorButtonPosition = ImGuiDir_Right;
		gs.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		gs.SelectableTextAlign = ImVec2(0.0f, 0.0f);
		gs.SeparatorTextBorderSize = 3.0f;
		gs.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
		gs.SeparatorTextPadding = ImVec2(20.0f, 3.f);
		gs.DisplayWindowPadding = ImVec2(19, 19);
		gs.DisplaySafeAreaPadding = ImVec2(3, 3);
		gs.DockingSeparatorSize = 2.0f;
		gs.MouseCursorScale = 1.0f;
		gs.AntiAliasedLines = true;
		gs.AntiAliasedLinesUseTex = true;
		gs.AntiAliasedFill = true;
		gs.CurveTessellationTol = 1.25f;
		gs.CircleTessellationMaxError = 0.30f;

		ep.Accent = ImGui::ColorConvertFloat4ToU32(ImVec4(0.925f, 0.62f, 0.14f, 1.f));
		ep.Highlight = ImGui::ColorConvertFloat4ToU32(ImVec4(0.15f, 0.07f, 0.95f, 1.f));
		ep.NiceBlue = ImGui::ColorConvertFloat4ToU32(ImVec4(0.325f, 0.91f, 1.f, 1.f));
		ep.Compliment = ImGui::ColorConvertFloat4ToU32(ImVec4(0.305f, 0.59f, 0.65f, 1.f));
		ep.Background = ImGui::ColorConvertFloat4ToU32(ImVec4(0.14f, 0.14f, 0.14f, 1.f));
		ep.BackgroundDark = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.1f, 0.1f, 1.f));
		ep.Titlebar = ImGui::ColorConvertFloat4ToU32(ImVec4(0.082f, 0.082f, 0.082f, 1.f));
		ep.Border = ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 1.f));
		ep.PropertyField = ImGui::ColorConvertFloat4ToU32(ImVec4(0.059f, 0.059f, 0.059f, 1.f));
		ep.Text = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8f, 0.8f, 0.8f, 1.f));
		ep.TextBrighter = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f));
		ep.TextDarker = ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.f));
		ep.TextError = ImGui::ColorConvertFloat4ToU32(ImVec4(0.9f, 0.2f, 0.2f, 1.f));
		ep.Muted = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 1.f));
		ep.GroupHeader = ImGui::ColorConvertFloat4ToU32(ImVec4(0.18f, 0.18f, 0.18f, 1.f));
		ep.Selection = ImGui::ColorConvertFloat4ToU32(ImVec4(0.93f, 0.75f, 0.46f, 1.f));
		ep.SelectionMuted = ImGui::ColorConvertFloat4ToU32(ImVec4(0.93f, 0.79f, 0.56f, 0.09f));
		ep.BackgroundPopup = ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 1.f));
		ep.ValidPrefab = ImGui::ColorConvertFloat4ToU32(ImVec4(0.32f, 0.7f, 0.87f, 1.f));
		ep.InvalidPrefab = ImGui::ColorConvertFloat4ToU32(ImVec4(0.87f, 0.17f, 0.17f, 1.f));
		ep.MissingMesh = ImGui::ColorConvertFloat4ToU32(ImVec4(0.9f, 0.4f, 0.3f, 1.f));
		ep.MeshNotSet = ImGui::ColorConvertFloat4ToU32(ImVec4(0.98f, 0.4f, 0.09f, 1.f));
		ep.Positive = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.7f, 0.2f, 1.f));
		ep.PositiveHovered = ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.8f, 0.2f, 1.f));
		ep.PositiveActive = ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.5f, 0.1f, 1.f));
		ep.Negative = ImGui::ColorConvertFloat4ToU32(ImVec4(0.7f, 0.3f, 0.2f, 1.f));
		ep.NegativeHovered = ImGui::ColorConvertFloat4ToU32(ImVec4(0.8, 0.3f, 0.2f, 1.f));
		ep.NegativeActive = ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.2f, 0.1f, 1.f));

		gp.Text = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		gp.TextDisabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		gp.WindowBg = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		gp.ChildBg = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		gp.PopupBg = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		gp.Border = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		gp.BorderShadow = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		gp.FrameBg = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
		gp.FrameBgHovered = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		gp.FrameBgActive = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		gp.TitleBg = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		gp.TitleBgActive = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
		gp.TitleBgcollapsed = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		gp.MenuBarBg = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		gp.ScrollbarBg = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		gp.ScrollbarGrab = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		gp.ScrollbarGrabHovered = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		gp.ScrollbarGrabActive = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		gp.CheckMark = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		gp.SliderGrab = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		gp.SliderGrabActive = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		gp.Button = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		gp.ButtonHovered = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
		gp.ButtonActive = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
		gp.Header = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
		gp.HeaderHovered = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		gp.HeaderActive = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
		gp.Separator = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
		gp.SeparatorHovered = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		gp.SeparatorActive = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
		gp.ResizeGrip = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		gp.ResizeGripHovered = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
		gp.ResizeGripActive = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
		gp.Tab = ImLerp(ImVec4(0.40f, 0.40f, 0.90f, 0.45f), ImVec4(0.16f, 0.29f, 0.48f, 1.00f), 0.80f);
		gp.TabHovered = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		gp.TabActive = ImLerp(ImVec4(0.53f, 0.53f, 0.87f, 0.80f), ImVec4(0.16f, 0.29f, 0.48f, 1.00f), 0.60f);
		gp.TabUnfocused = ImLerp(ImLerp(ImVec4(0.40f, 0.40f, 0.90f, 0.45f), ImVec4(0.16f, 0.29f, 0.48f, 1.00f), 0.80f), ImVec4(0.04f, 0.04f, 0.04f, 1.00f), 0.80f);
		gp.TabUnfocusedActive = ImLerp(ImLerp(ImVec4(0.53f, 0.53f, 0.87f, 0.80f), ImVec4(0.16f, 0.29f, 0.48f, 1.00f), 0.60f), ImVec4(0.04f, 0.04f, 0.04f, 1.00f), 0.40f);
		gp.DockingPreview = ImVec4(0.40f, 0.40f, 0.90f, 0.31f);
		gp.DockingEmptyBg = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		gp.PlotLines = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		gp.PlotLineHovered = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		gp.PlotHistogram = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		gp.PlotHistogramHovered = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
		gp.TableHeaderBg = ImVec4(0.78f, 0.87f, 0.98f, 1.00f);
		gp.TableBorderStrong = ImVec4(0.57f, 0.57f, 0.64f, 1.00f);
		gp.TableBorderLight = ImVec4(0.68f, 0.68f, 0.74f, 1.00f);
		gp.TableRowBg = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		gp.TableRowBgAlt = ImVec4(0.30f, 0.30f, 0.30f, 0.09f);
		gp.TextSelectedBg = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		gp.DragDropTarget = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		gp.NavHighlight = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		gp.NavWindowingHighlight = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
		gp.NavWindowingDimBg = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
		gp.ModalWindowDibBg = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);



		YAML::Node themeNode = FillNodeWith(theme);

		YAML::Node generalNode;
		generalNode["default_theme"] = m_DefaultThemeName;
		YAML::Node selectedTheme;
		generalNode["selected_theme"] = m_SelectedThemeName;

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "general" << YAML::Value << generalNode;

		out << YAML::Key << "themes" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "DefaultDark" << YAML::Value << themeNode;
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(m_ConfPath);
		if (!fout.is_open())
		{
			VM_CORE_ERROR("[Editor] Failed to open file for writing: %s", m_ConfPath.string());
			return;
		}
		fout << out.c_str();
		fout.close();

		Application::Get().GetGUIContext()->SetTheme(theme);
	}

	bool Theme::FileExists(const String& filename)
	{
		std::ifstream file(filename);
		return file.good();
	}

	bool Theme::TryLoadThemes()
	{
		bool confExists = std::filesystem::exists(m_ConfPath.string());
		if (!confExists)
		{
			VM_CORE_ERROR("[Editor] {} does not exist. Cannot load themes", m_ConfPath.string());
			InitConfig();
			VM_CORE_INFO("[Editor] {} was created with default configuration", m_ConfPath.string());
		}
		LoadThemes();
		if (m_Themes.empty())
		{
			VM_CORE_ERROR("[Editor] Tried to load themes. {} is empty or corrupted", m_ConfPath.string());
			return false;
		}
		m_ThemeNames.clear();
		for (const auto& [themeName, properties] : m_Themes)
		{
			m_ThemeNames.push_back(themeName);
		}
		auto it = std::find(m_ThemeNames.begin(), m_ThemeNames.end(), m_SelectedThemeName);
		if (it != m_ThemeNames.end())
		{
			m_ThemeIdx = std::distance(m_ThemeNames.begin(), it);
		}
		return true;
	}

	YAML::Node Theme::FillNodeWith(Theme& theme)
	{
		GUIStyle& gs = theme.Style;
		GUIPalette& gp = theme.GUIColors;
		EditorPalette& ep = theme.EditorColors;

		YAML::Node currentTheme;

		YAML::Node style;
		YAML::Node editorpalette;
		YAML::Node guipalette;
		{
			style["Alpha"] = gs.Alpha;
			style["DisabledAlpha"] = gs.DisabledAlpha;
			style["WindowPadding"] = gs.WindowPadding;
			style["WindowRounding"] = gs.WindowRounding;
			style["WindowBorderSize"] = gs.WindowBorderSize;
			style["WindowMinSize"] = gs.WindowMinSize;
			style["WindowTitleAlign"] = gs.WindowTitleAlign;
			style["WindowMenuButtonPosition"] = (int)gs.WindowMenuButtonPosition;
			style["ChildRounding"] = gs.ChildRounding;
			style["ChildBorderSize"] = gs.ChildBorderSize;
			style["PopupRounding"] = gs.PopupRounding;
			style["PopupBorderSize"] = gs.PopupBorderSize;
			style["FramePadding"] = gs.FramePadding;
			style["FrameRounding"] = gs.FrameRounding;
			style["FrameBorderSize"] = gs.FrameBorderSize;
			style["ItemSpacing"] = gs.ItemSpacing;
			style["ItemInnerSpacing"] = gs.ItemInnerSpacing;
			style["CellPadding"] = gs.CellPadding;
			style["TouchExtraPadding"] = gs.TouchExtraPadding;
			style["IndentSpacing"] = gs.IndentSpacing;
			style["ColumnsMinSpacing"] = gs.ColumnsMinSpacing;
			style["ScrollbarSize"] = gs.ScrollbarSize;
			style["ScrollbarRounding"] = gs.ScrollbarRounding;
			style["GrabMinSize"] = gs.GrabMinSize;
			style["GrabRounding"] = gs.GrabRounding;
			style["LayoutAlign"] = gs.LayoutAlign;
			style["LogSliderDeadzone"] = gs.LogSliderDeadzone;
			style["TabRounding"] = gs.TabRounding;
			style["TabBorderSize"] = gs.TabBorderSize;
			style["TabMinWidthForCloseButton"] = gs.TabMinWidthForCloseButton;
			style["TabBarBorderSize"] = gs.TabBarBorderSize;
			style["TableAngledHeadersAngle"] = gs.TableAngledHeadersAngle;
			style["TableAngledHeadersTextAlign"] = gs.TableAngledHeadersTextAlign;
			style["ColorButtonPosition"] = (int)gs.ColorButtonPosition;
			style["ButtonTextAlign"] = gs.ButtonTextAlign;
			style["SelectableTextAlign"] = gs.SelectableTextAlign;
			style["SeparatorTextBorderSize"] = gs.SeparatorTextBorderSize;
			style["SeparatorTextAlign"] = gs.SeparatorTextAlign;
			style["SeparatorTextPadding"] = gs.SeparatorTextPadding;
			style["DisplayWindowPadding"] = gs.DisplayWindowPadding;
			style["DisplaySafeAreaPadding"] = gs.DisplaySafeAreaPadding;
			style["DockingSeparatorSize"] = gs.DockingSeparatorSize;
			style["MouseCursorScale"] = gs.MouseCursorScale;
			style["AntiAliasedLines"] = gs.AntiAliasedLines;
			style["AntiAliasedLinesUseTex"] = gs.AntiAliasedLinesUseTex;
			style["AntiAliasedFill"] = gs.AntiAliasedFill;
			style["CurveTessellationTol"] = gs.CurveTessellationTol;
			style["CircleTessellationMaxError"] = gs.CircleTessellationMaxError;

			editorpalette["Accent"] = ImGui::ColorConvertU32ToFloat4(ep.Accent);
			editorpalette["Highlight"] = ImGui::ColorConvertU32ToFloat4(ep.Highlight);
			editorpalette["NiceBlue"] = ImGui::ColorConvertU32ToFloat4(ep.NiceBlue);
			editorpalette["Compliment"] = ImGui::ColorConvertU32ToFloat4(ep.Compliment);
			editorpalette["Background"] = ImGui::ColorConvertU32ToFloat4(ep.Background);
			editorpalette["BackgroundDark"] = ImGui::ColorConvertU32ToFloat4(ep.BackgroundDark);
			editorpalette["Titlebar"] = ImGui::ColorConvertU32ToFloat4(ep.Titlebar);
			editorpalette["Border"] = ImGui::ColorConvertU32ToFloat4(ep.Border);
			editorpalette["PropertyField"] = ImGui::ColorConvertU32ToFloat4(ep.PropertyField);
			editorpalette["Text"] = ImGui::ColorConvertU32ToFloat4(ep.Text);
			editorpalette["TextBrighter"] = ImGui::ColorConvertU32ToFloat4(ep.TextBrighter);
			editorpalette["TextDarker"] = ImGui::ColorConvertU32ToFloat4(ep.TextDarker);
			editorpalette["TextError"] = ImGui::ColorConvertU32ToFloat4(ep.TextError);
			editorpalette["Muted"] = ImGui::ColorConvertU32ToFloat4(ep.Muted);
			editorpalette["GroupHeader"] = ImGui::ColorConvertU32ToFloat4(ep.GroupHeader);
			editorpalette["Selection"] = ImGui::ColorConvertU32ToFloat4(ep.Selection);
			editorpalette["SelectionMuted"] = ImGui::ColorConvertU32ToFloat4(ep.SelectionMuted);
			editorpalette["BackgroundPopup"] = ImGui::ColorConvertU32ToFloat4(ep.BackgroundPopup);
			editorpalette["ValidPrefab"] = ImGui::ColorConvertU32ToFloat4(ep.ValidPrefab);
			editorpalette["InvalidPrefab"] = ImGui::ColorConvertU32ToFloat4(ep.InvalidPrefab);
			editorpalette["MissingMesh"] = ImGui::ColorConvertU32ToFloat4(ep.MissingMesh);
			editorpalette["MeshNotSet"] = ImGui::ColorConvertU32ToFloat4(ep.MeshNotSet);
			editorpalette["Positive"] = ImGui::ColorConvertU32ToFloat4(ep.Positive);
			editorpalette["PositiveHovered"] = ImGui::ColorConvertU32ToFloat4(ep.PositiveHovered);
			editorpalette["PositiveActive"] = ImGui::ColorConvertU32ToFloat4(ep.PositiveActive);
			editorpalette["Negative"] = ImGui::ColorConvertU32ToFloat4(ep.Negative);
			editorpalette["NegativeHovered"] = ImGui::ColorConvertU32ToFloat4(ep.NegativeHovered);
			editorpalette["NegativeActive"] = ImGui::ColorConvertU32ToFloat4(ep.NegativeActive);

			guipalette["Text"] = gp.Text;
			guipalette["TextDisabled"] = gp.TextDisabled;
			guipalette["WindowBg"] = gp.WindowBg;
			guipalette["ChildBg"] = gp.ChildBg;
			guipalette["PopupBg"] = gp.PopupBg;
			guipalette["Border"] = gp.Border;
			guipalette["BorderShadow"] = gp.BorderShadow;
			guipalette["FrameBg"] = gp.FrameBg;
			guipalette["FrameBgHovered"] = gp.FrameBgHovered;
			guipalette["FrameBgActive"] = gp.FrameBgActive;
			guipalette["TitleBg"] = gp.TitleBg;
			guipalette["TitleBgActive"] = gp.TitleBgActive;
			guipalette["TitleBgcollapsed"] = gp.TitleBgcollapsed;
			guipalette["MenuBarBg"] = gp.MenuBarBg;
			guipalette["ScrollbarBg"] = gp.ScrollbarBg;
			guipalette["ScrollbarGrab"] = gp.ScrollbarGrab;
			guipalette["ScrollbarGrabHovered"] = gp.ScrollbarGrabHovered;
			guipalette["ScrollbarGrabActive"] = gp.ScrollbarGrabActive;
			guipalette["CheckMark"] = gp.CheckMark;
			guipalette["SliderGrab"] = gp.SliderGrab;
			guipalette["SliderGrabActive"] = gp.SliderGrabActive;
			guipalette["Button"] = gp.Button;
			guipalette["ButtonHovered"] = gp.ButtonHovered;
			guipalette["ButtonActive"] = gp.ButtonActive;
			guipalette["Header"] = gp.Header;
			guipalette["HeaderHovered"] = gp.HeaderHovered;
			guipalette["HeaderActive"] = gp.HeaderActive;
			guipalette["Separator"] = gp.Separator;
			guipalette["SeparatorHovered"] = gp.SeparatorHovered;
			guipalette["SeparatorActive"] = gp.SeparatorActive;
			guipalette["ResizeGrip"] = gp.ResizeGrip;
			guipalette["ResizeGripHovered"] = gp.ResizeGripHovered;
			guipalette["ResizeGripActive"] = gp.ResizeGripActive;
			guipalette["Tab"] = gp.Tab;
			guipalette["TabHovered"] = gp.TabHovered;
			guipalette["TabActive"] = gp.TabActive;
			guipalette["TabUnfocused"] = gp.TabUnfocused;
			guipalette["TabUnfocusedActive"] = gp.TabUnfocusedActive;
			guipalette["DockingPreview"] = gp.DockingPreview;
			guipalette["DockingEmptyBg"] = gp.DockingEmptyBg;
			guipalette["PlotLines"] = gp.PlotLines;
			guipalette["PlotLineHovered"] = gp.PlotLineHovered;
			guipalette["PlotHistogram"] = gp.PlotHistogram;
			guipalette["PlotHistogramHovered"] = gp.PlotHistogramHovered;
			guipalette["TableHeaderBg"] = gp.TableHeaderBg;
			guipalette["TableBorderStrong"] = gp.TableBorderStrong;
			guipalette["TableBorderLight"] = gp.TableBorderLight;
			guipalette["TableRowBg"] = gp.TableRowBg;
			guipalette["TableRowBgAlt"] = gp.TableRowBgAlt;
			guipalette["TextSelectedBg"] = gp.TextSelectedBg;
			guipalette["DragDropTarget"] = gp.DragDropTarget;
			guipalette["NavHighlight"] = gp.NavHighlight;
			guipalette["NavWindowingHighlight"] = gp.NavWindowingHighlight;
			guipalette["NavWindowingDimBg"] = gp.NavWindowingDimBg;
			guipalette["ModalWindowDibBg"] = gp.ModalWindowDibBg;

		}

		currentTheme["style"] = style;
		currentTheme["editorpalette"] = editorpalette;
		currentTheme["guipalette"] = guipalette;


		return currentTheme;
	}

	bool Theme::ThemeSelector(const char* label)
	{
		std::vector<const char*> item_cstrs;
		for (const auto& item : m_ThemeNames)
		{
			item_cstrs.push_back(item.c_str());
		}
		static int prev_ThemeIdx = -1;
		if (ImGui::Combo(label, &m_ThemeIdx, item_cstrs.data(), item_cstrs.size()));
		{
			if (m_ThemeIdx != prev_ThemeIdx)
			{
				m_SelectedThemeName = m_ThemeNames[m_ThemeIdx];
				SetTheme(m_SelectedThemeName);
				prev_ThemeIdx = m_ThemeIdx;
				YAML::Emitter out;
				EmitThemesContent(out);
				EndEmit(out);
				UploadContent(out);

			}

			return true;
		}
		return false;
	}

	void Theme::SetTheme(const String& themeName)
	{
		m_SavedTheme = LoadTheme(themeName);
		Application::Get().GetGUIContext()->SetTheme(m_SavedTheme);
	}

	void Theme::UpdateTheme(Theme& theme)
	{
		YAML::Node currentTheme = FillNodeWith(theme);
		m_Themes[m_ThemeNames[m_ThemeIdx]] = currentTheme;
		YAML::Emitter out;
		EmitThemesContent(out);
		EndEmit(out);
		UploadContent(out);

	}

	void Theme::EmitThemesContent(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "general" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "default_theme" << YAML::Value << m_DefaultThemeName;
		out << YAML::Key << "selected_theme" << YAML::Value << m_SelectedThemeName;
		out << YAML::EndMap;

		out << YAML::Key << "themes" << YAML::Value << YAML::BeginMap;
		for (const auto& [themeName, properties] : m_Themes)
		{
			out << YAML::Key << themeName << YAML::Value << properties;
		}
	}

	void Theme::EndEmit(YAML::Emitter& out)
	{
		YAML::EndMap;
		YAML::EndMap;
	}
	void Theme::UploadContent(YAML::Emitter& out)
	{
		std::ofstream fout(m_ConfPath);
		if (!fout.is_open())
		{
			VM_CORE_ERROR("[Editor] Failed to open file for writing: %s", m_ConfPath.string());
			return;
		}
		fout << out.c_str();
		fout.close();
	}

	void Theme::ColorEdit_EditorPalette(const char* label, ImU32& themeVar, ImU32& savedVar)
	{
		if (!m_ColorFilter.PassFilter(label))
			return;
		Theme& theme = Get();
		UI::ParameterColorU32(label, themeVar, 0, [&]()
							  {
								  float space = ImGui::GetContentRegionAvail().x;
								  float title = ImGui::CalcTextSize(label).x;
								  float hint = ImGui::CalcTextSize("?").x;
								  if (memcmp(&themeVar, &savedVar, sizeof(themeVar)) != 0)
								  {
								  	float saveB = ImGui::CalcTextSize("Save").x;
								  	float revertB = ImGui::CalcTextSize("Revert").x;
								  	float gap = saveB + revertB + title + hint + theme.Style.ItemInnerSpacing.x * 2 + theme.Style.FramePadding.x * 6;
								  
								  	// TODO: Check docs/FONTS.md  to use icons				
								  	ImGui::SameLine(0.0f, space - gap); UI::ShiftCursorY(-2); if (ImGui::Button("Save")) { if (m_ThemeNames[m_ThemeIdx] != m_DefaultThemeName) { savedVar = themeVar; UpdateTheme(m_SavedTheme); } }
								  	ImGui::SameLine(0.0f, theme.Style.ItemInnerSpacing.x); UI::ShiftCursorY(-2);  if (ImGui::Button("Revert")) { themeVar = savedVar; }
								  	ImGui::SameLine(0.0f, theme.Style.ItemInnerSpacing.x); UI::ShiftCursorY(-2);
								  }
								  else
								  {
								  	float gap = hint + title + theme.Style.FramePadding.x * 2;
								  	ImGui::SameLine(0.0f, space - gap); UI::ShiftCursorY(-2);
								  }
								  ImGui::PushID(label);
								  if (ImGui::Button("?"))
								  {
									  DebugFlashColorEditor(themeVar);
								  }
								  ImGui::PopID();
								  ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
								  ImGui::SameLine();
							  });
	}

	void Theme::ColorEdit_GUIPalette(const char* label, ImVec4& themeVar, ImVec4& savedVar)
	{
		if (!m_ColorFilter.PassFilter(label))
			return;
		Theme& theme = Get();
		UI::ParameterColor4(label, themeVar, 0, [&]()
							{
								float space = ImGui::GetContentRegionAvail().x;
								float title = ImGui::CalcTextSize(label).x;
								float hint = ImGui::CalcTextSize("?").x;
								if (memcmp(&themeVar, &savedVar, sizeof(themeVar)) != 0)
								{
									float saveB = ImGui::CalcTextSize("Save").x;
									float revertB = ImGui::CalcTextSize("Revert").x;
									float gap = saveB + revertB + title + hint + theme.Style.ItemInnerSpacing.x * 2 + theme.Style.FramePadding.x * 6;

									// TODO: Check docs/FONTS.md  to use icons				
									ImGui::SameLine(0.0f, space - gap); UI::ShiftCursorY(-2); if (ImGui::Button("Save")) { if (m_ThemeNames[m_ThemeIdx] != m_DefaultThemeName) { savedVar = themeVar; UpdateTheme(m_SavedTheme); } }
									ImGui::SameLine(0.0f, theme.Style.ItemInnerSpacing.x); UI::ShiftCursorY(-2);  if (ImGui::Button("Revert")) { themeVar = savedVar; }
									ImGui::SameLine(0.0f, theme.Style.ItemInnerSpacing.x); UI::ShiftCursorY(-2);
								}
								else
								{
									float gap = hint + title + theme.Style.FramePadding.x * 2;
									ImGui::SameLine(0.0f, space - gap); UI::ShiftCursorY(-2);
								}
								ImGui::PushID(label);
								if (ImGui::Button("?"))
								{
									DebugFlashColorGUI(themeVar);
								}
								ImGui::PopID();
								ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
								ImGui::SameLine();
							});
	}

	void Theme::DebugFlashColorEditor(ImU32& color)
	{
		GUIContext* gcontext = Application::Get().GetGUIContext();
		GUIDebug& gdebug = gcontext->GetDebug();
		DebugFlashColorStopEditor();
		gdebug.FlashColorTime = 1.0f;
		gdebug.FlashColorU32 = &color;
		gdebug.FlashColorU32Backup = color;
	}

	void Theme::DebugFlashColorGUI(ImVec4& color)
	{
		GUIContext* gcontext = Application::Get().GetGUIContext();
		GUIDebug& gdebug = gcontext->GetDebug();
		DebugFlashColorStopGUI();
		gdebug.FlashColorTime = 1.0f;
		gdebug.FlashColor4 = &color;
		gdebug.FlashColor4Backup = color;

	}

	void Theme::DebugFlashColorStopGUI()
	{
		GUIContext* gcontext = Application::Get().GetGUIContext();
		GUIDebug& gdebug = gcontext->GetDebug();
		if (!gdebug.FlashColor4)
			return;
		*gdebug.FlashColor4 = gdebug.FlashColor4Backup;
		gdebug.FlashColor4 = nullptr;
	}

	void Theme::DebugFlashColorStopEditor()
	{
		GUIContext* gcontext = Application::Get().GetGUIContext();
		GUIDebug& gdebug = gcontext->GetDebug();
		if (!gdebug.FlashColorU32)
			return;
		*gdebug.FlashColorU32 = gdebug.FlashColorU32Backup;
		gdebug.FlashColorU32 = nullptr;

	}



}