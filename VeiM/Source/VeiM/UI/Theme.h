#pragma once

#include <imgui.h>

#include "VeiM/Core/CoreDefines.h"
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace YAML
{
	class Node;
}
namespace VeiM::UI
{

	struct EditorPalette
	{
		// TODO: Add color for Positive and Negavite buttons so it is consistent
		ImU32 Accent = IM_COL32(236, 158, 36, 255);
		ImU32 Highlight = IM_COL32(39, 185, 242, 255);
		ImU32 NiceBlue = IM_COL32(83, 232, 254, 255);
		ImU32 Compliment = IM_COL32(78, 151, 166, 255);
		ImU32 Background = IM_COL32(36, 36, 36, 255);
		ImU32 BackgroundDark = IM_COL32(26, 26, 26, 255);
		ImU32 Titlebar = IM_COL32(21, 21, 21, 255);
		ImU32 Border = IM_COL32(51, 51, 51, 255);
		ImU32 PropertyField = IM_COL32(15, 15, 15, 255);
		ImU32 Text = IM_COL32(205, 205, 205, 255);
		ImU32 TextBrighter = IM_COL32(255, 255, 255, 255);
		ImU32 TextDarker = IM_COL32(128, 128, 128, 255);
		ImU32 TextError = IM_COL32(230, 51, 51, 255);
		ImU32 Muted = IM_COL32(77, 77, 77, 255);
		ImU32 GroupHeader = IM_COL32(47, 47, 47, 255);
		ImU32 Selection = IM_COL32(237, 192, 119, 255);
		ImU32 SelectionMuted = IM_COL32(237, 201, 142, 23);
		ImU32 BackgroundPopup = IM_COL32(50, 50, 50, 255);
		ImU32 ValidPrefab = IM_COL32(82, 179, 222, 255);
		ImU32 InvalidPrefab = IM_COL32(222, 43, 43, 255);
		ImU32 MissingMesh = IM_COL32(230, 102, 76, 255);
		ImU32 MeshNotSet = IM_COL32(250, 101, 23, 255);
		ImU32 Positive = IM_COL32(180, 75, 50, 255);
		ImU32 PositiveHovered = IM_COL32(205, 75, 50, 255);
		ImU32 PositiveActive = IM_COL32(128, 50, 25, 255);
		ImU32 Negative = IM_COL32(75, 180, 50, 255);
		ImU32 NegativeHovered = IM_COL32(75, 205, 50, 255);
		ImU32 NegativeActive = IM_COL32(50, 128, 25, 255);
	};
	struct GUIPalette
	{
		ImVec4 Text;
		ImVec4 TextDisabled;
		ImVec4 WindowBg;
		ImVec4 ChildBg;
		ImVec4 PopupBg;
		ImVec4 Border;
		ImVec4 BorderShadow;
		ImVec4 FrameBg;
		ImVec4 FrameBgHovered;
		ImVec4 FrameBgActive;
		ImVec4 TitleBg;
		ImVec4 TitleBgActive;
		ImVec4 TitleBgcollapsed;
		ImVec4 MenuBarBg;
		ImVec4 ScrollbarBg;
		ImVec4 ScrollbarGrab;
		ImVec4 ScrollbarGrabHovered;
		ImVec4 ScrollbarGrabActive;
		ImVec4 CheckMark;
		ImVec4 SliderGrab;
		ImVec4 SliderGrabActive;
		ImVec4 Button;
		ImVec4 ButtonHovered;
		ImVec4 ButtonActive;
		ImVec4 Header;
		ImVec4 HeaderHovered;
		ImVec4 HeaderActive;
		ImVec4 Separator;
		ImVec4 SeparatorHovered;
		ImVec4 SeparatorActive;
		ImVec4 ResizeGrip;
		ImVec4 ResizeGripHovered;
		ImVec4 ResizeGripActive;
		ImVec4 Tab;
		ImVec4 TabHovered;
		ImVec4 TabActive;
		ImVec4 TabUnfocused;
		ImVec4 TabUnfocusedActive;
		ImVec4 DockingPreview;
		ImVec4 DockingEmptyBg;
		ImVec4 PlotLines;
		ImVec4 PlotLineHovered;
		ImVec4 PlotHistogram;
		ImVec4 PlotHistogramHovered;
		ImVec4 TableHeaderBg;
		ImVec4 TableBorderStrong;
		ImVec4 TableBorderLight;
		ImVec4 TableRowBg;
		ImVec4 TableRowBgAlt;
		ImVec4 TextSelectedBg;
		ImVec4 DragDropTarget;
		ImVec4 NavHighlight;
		ImVec4 NavWindowingHighlight;
		ImVec4 NavWindowingDimBg;
		ImVec4 ModalWindowDibBg;
	};
	struct GUIStyle
	{
		float       Alpha;                      
		float       DisabledAlpha;              
		ImVec2      WindowPadding;              
		float       WindowRounding;             
		float       WindowBorderSize;           
		ImVec2      WindowMinSize;              
		ImVec2      WindowTitleAlign;           
		ImGuiDir    WindowMenuButtonPosition;   
		float       ChildRounding;              
		float       ChildBorderSize;            
		float       PopupRounding;              
		float       PopupBorderSize;            
		ImVec2      FramePadding;               
		float       FrameRounding;              
		float       FrameBorderSize;            
		ImVec2      ItemSpacing;                
		ImVec2      ItemInnerSpacing;           
		ImVec2      CellPadding;                
		ImVec2      TouchExtraPadding;         
		float       IndentSpacing;              
		float       ColumnsMinSpacing;          
		float       ScrollbarSize;              
		float       ScrollbarRounding;          
		float       GrabMinSize;                
		float       GrabRounding;               
		float       LayoutAlign;                
		float       LogSliderDeadzone;          
		float       TabRounding;                
		float       TabBorderSize;              
		float       TabMinWidthForCloseButton;  
		float       TabBarBorderSize;           
		float       TableAngledHeadersAngle;    
		ImVec2      TableAngledHeadersTextAlign;
		ImGuiDir    ColorButtonPosition;        
		ImVec2      ButtonTextAlign;            
		ImVec2      SelectableTextAlign;        
		float       SeparatorTextBorderSize;    
		ImVec2      SeparatorTextAlign;         
		ImVec2      SeparatorTextPadding;       
		ImVec2      DisplayWindowPadding;       
		ImVec2      DisplaySafeAreaPadding;     
		float       DockingSeparatorSize;       
		float       MouseCursorScale;           
		bool        AntiAliasedLines;           
		bool        AntiAliasedLinesUseTex;     
		bool        AntiAliasedFill;            
		float       CurveTessellationTol;       
		float       CircleTessellationMaxError;
	};

	struct Theme
	{
		static Theme& Get();
		static void Editor();
		static Theme GetDefault();	 // Default editor theme provided by developer
		static Theme GetSelected();
		static bool TryLoadThemes(); // Call on application init

		static void DebugFlashColorEditor(ImU32& color);
		static void DebugFlashColorGUI(ImVec4& color);
		static void DebugFlashColorStopEditor();
		static void DebugFlashColorStopGUI();

		EditorPalette EditorColors;
		GUIPalette GUIColors;
		GUIStyle Style;

	private:
		static std::unordered_map<String, YAML::Node> m_Themes;
		static String m_DefaultThemeName;
		static String m_SelectedThemeName;
		static int32 m_ThemeIdx;
		static std::vector<String> m_ThemeNames;
		static char m_NewThemeName[32];
		static bool m_NameCollision;
		static Theme m_SavedTheme;
		static ImGuiTextFilter m_ColorFilter;
		static std::filesystem::path m_ConfPath;

	private:

		static void SaveNewTheme(Theme& theme);
		static void UpdateTheme(Theme& theme);

		static void EmitThemesContent(YAML::Emitter& out);
		static void EndEmit(YAML::Emitter& out);
		static void UploadContent(YAML::Emitter& out);

		static void UpdateConfirmPopup(bool& open);

		static void LoadThemes();
		static Theme LoadTheme(const String& themeName);
		static void SetTheme(const String& themeName);
		// Should only be called by the engine itself, when Themes.ini does not exist
		static void InitConfig();

		static YAML::Node FillNodeWith(Theme& theme);

		static bool FileExists(const String& filename);

	private:
		static bool ThemeSelector(const char* label);
		static void ColorEdit_EditorPalette(const char* label, ImU32& themeVar, ImU32& savedVar);
		static void ColorEdit_GUIPalette(const char* label, ImVec4& themeVar, ImVec4& savedVar);
	};

}