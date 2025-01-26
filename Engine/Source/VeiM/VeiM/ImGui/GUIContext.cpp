#include "GUIContext.h"

#include "VeiM/Core/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

namespace VeiM
{
	GUIContext::GUIContext()
		:Layer("ImGuiLayer")
	{

	}

	void GUIContext::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.IniFilename = "Engine/Config/imgui.ini";

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		float fontSize = 16.0f;
		String fontPath = Application::Get().GetEngineResourcePath().string() + "/UI/Fonts/";
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Bold.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Bold.ttf").c_str(), fontSize * 1.5);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Black.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-BlackItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-BoldItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Italic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Light.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-LightItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Medium.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-MediumItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Thin.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-ThinItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-Bold.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-BoldItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-Italic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-Light.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-LightItalic.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-Regular.ttf").c_str(), fontSize);
		io.Fonts->AddFontFromFileTTF((fontPath + "Nunito-Sans/NunitoSans_10pt-SemiBold.ttf").c_str(), fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF((fontPath + "Roboto/Roboto-Regular.ttf").c_str(), fontSize);



		ImGui::StyleColorsDark();


		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // TODO: This doesnt work. Find out why
		}

		UI::Theme::TryLoadThemes();
		m_Theme = UI::Theme::GetSelected();
		UpdateTheme();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		// - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != nullptr);


	}

	void GUIContext::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void GUIContext::EnableDocking(Window& window)
	{
		
	}

	void GUIContext::RenderDockspace()
	{
		

	}

	void GUIContext::BeginFrame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GUIContext::EndFrame()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Rendering
		ImGui::Render();
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

		if (!main_is_minimized)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		// Present Main Platform Window
		if (main_is_minimized)
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	void GUIContext::UpdateTheme()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		UI::GUIPalette& gp = m_Theme.GUIColors;
		UI::GUIStyle& gs = m_Theme.Style;

		style->Alpha = gs.Alpha;
		style->DisabledAlpha = gs.DisabledAlpha;
		style->WindowPadding = gs.WindowPadding;
		style->WindowRounding = gs.WindowRounding;
		style->WindowBorderSize = gs.WindowBorderSize;
		style->WindowMinSize = gs.WindowMinSize;
		style->WindowTitleAlign = gs.WindowTitleAlign;
		style->WindowMenuButtonPosition = gs.WindowMenuButtonPosition;
		style->ChildRounding = gs.ChildRounding;
		style->ChildBorderSize = gs.ChildBorderSize;
		style->PopupRounding = gs.PopupRounding;
		style->PopupBorderSize = gs.PopupBorderSize;
		style->FramePadding = gs.FramePadding;
		style->FrameRounding = gs.FrameRounding;
		style->FrameBorderSize = gs.FrameBorderSize;
		style->ItemSpacing = gs.ItemSpacing;
		style->ItemInnerSpacing = gs.ItemInnerSpacing;
		style->CellPadding = gs.CellPadding;
		style->TouchExtraPadding = gs.TouchExtraPadding;
		style->IndentSpacing = gs.IndentSpacing;
		style->ColumnsMinSpacing = gs.ColumnsMinSpacing;
		style->ScrollbarSize = gs.ScrollbarSize;
		style->ScrollbarRounding = gs.ScrollbarRounding;
		style->GrabMinSize = gs.GrabMinSize;
		style->GrabRounding = gs.GrabRounding;
		style->LayoutAlign = gs.LayoutAlign;
		style->LogSliderDeadzone = gs.LogSliderDeadzone;
		style->TabRounding = gs.TabRounding;
		style->TabBorderSize = gs.TabBorderSize;
		style->TabMinWidthForCloseButton = gs.TabMinWidthForCloseButton;
		style->TabBarBorderSize = gs.TabBarBorderSize;
		style->TableAngledHeadersAngle = gs.TableAngledHeadersAngle;
		style->TableAngledHeadersTextAlign = gs.TableAngledHeadersTextAlign;
		style->ColorButtonPosition = gs.ColorButtonPosition;
		style->ButtonTextAlign = gs.ButtonTextAlign;
		style->SelectableTextAlign = gs.SelectableTextAlign;
		style->SeparatorTextBorderSize = gs.SeparatorTextBorderSize;
		style->SeparatorTextAlign = gs.SeparatorTextAlign;
		style->SeparatorTextPadding = gs.SeparatorTextPadding;
		style->DisplayWindowPadding = gs.DisplayWindowPadding;
		style->DisplaySafeAreaPadding = gs.DisplaySafeAreaPadding;
		style->DockingSeparatorSize = gs.DockingSeparatorSize;
		style->MouseCursorScale = gs.MouseCursorScale;
		style->AntiAliasedLines = gs.AntiAliasedLines;
		style->AntiAliasedLinesUseTex = gs.AntiAliasedLinesUseTex;
		style->AntiAliasedFill = gs.AntiAliasedFill;
		style->CurveTessellationTol = gs.CurveTessellationTol;
		style->CircleTessellationMaxError = gs.CircleTessellationMaxError;


		colors[ImGuiCol_Text] = gp.Text;
		colors[ImGuiCol_TextDisabled] = gp.TextDisabled;
		colors[ImGuiCol_WindowBg] = gp.WindowBg;
		colors[ImGuiCol_ChildBg] = gp.ChildBg;
		colors[ImGuiCol_PopupBg] = gp.PopupBg;
		colors[ImGuiCol_Border] = gp.Border;
		colors[ImGuiCol_BorderShadow] = gp.BorderShadow;
		colors[ImGuiCol_FrameBg] = gp.FrameBg;
		colors[ImGuiCol_FrameBgHovered] = gp.FrameBgHovered;
		colors[ImGuiCol_FrameBgActive] = gp.FrameBgActive;
		colors[ImGuiCol_TitleBg] = gp.TitleBg;
		colors[ImGuiCol_TitleBgActive] = gp.TitleBgActive;
		colors[ImGuiCol_TitleBgCollapsed] = gp.TitleBgcollapsed;
		colors[ImGuiCol_MenuBarBg] = gp.MenuBarBg;
		colors[ImGuiCol_ScrollbarBg] = gp.ScrollbarBg;
		colors[ImGuiCol_ScrollbarGrab] = gp.ScrollbarGrab;
		colors[ImGuiCol_ScrollbarGrabHovered] = gp.ScrollbarGrabHovered;
		colors[ImGuiCol_ScrollbarGrabActive] = gp.ScrollbarGrabActive;
		colors[ImGuiCol_CheckMark] = gp.CheckMark;
		colors[ImGuiCol_SliderGrab] = gp.SliderGrab;
		colors[ImGuiCol_SliderGrabActive] = gp.SliderGrabActive;
		colors[ImGuiCol_Button] = gp.Button;
		colors[ImGuiCol_ButtonHovered] = gp.ButtonHovered;
		colors[ImGuiCol_ButtonActive] = gp.ButtonActive;
		colors[ImGuiCol_Header] = gp.Header;
		colors[ImGuiCol_HeaderHovered] = gp.HeaderHovered;
		colors[ImGuiCol_HeaderActive] = gp.HeaderActive;
		colors[ImGuiCol_Separator] = gp.Separator;
		colors[ImGuiCol_SeparatorHovered] = gp.SeparatorHovered;
		colors[ImGuiCol_SeparatorActive] = gp.SeparatorActive;
		colors[ImGuiCol_ResizeGrip] = gp.ResizeGrip;
		colors[ImGuiCol_ResizeGripHovered] = gp.ResizeGripHovered;
		colors[ImGuiCol_ResizeGripActive] = gp.ResizeGripActive;
		colors[ImGuiCol_Tab] = gp.Tab;
		colors[ImGuiCol_TabHovered] = gp.TabHovered;
		colors[ImGuiCol_TabActive] = gp.TabActive;
		colors[ImGuiCol_TabUnfocused] = gp.TabUnfocused;
		colors[ImGuiCol_TabUnfocusedActive] = gp.TabUnfocusedActive;
		colors[ImGuiCol_DockingPreview] = gp.DockingPreview;
		colors[ImGuiCol_DockingEmptyBg] = gp.DockingEmptyBg;
		colors[ImGuiCol_PlotLines] = gp.PlotLines;
		colors[ImGuiCol_PlotLinesHovered] = gp.PlotLineHovered;
		colors[ImGuiCol_PlotHistogram] = gp.PlotHistogram;
		colors[ImGuiCol_PlotHistogramHovered] = gp.PlotHistogramHovered;
		colors[ImGuiCol_TableHeaderBg] = gp.TableHeaderBg;
		colors[ImGuiCol_TableBorderStrong] = gp.TableBorderStrong;
		colors[ImGuiCol_TableBorderLight] = gp.TableBorderLight;
		colors[ImGuiCol_TableRowBg] = gp.TableRowBg;
		colors[ImGuiCol_TableRowBgAlt] = gp.TableRowBgAlt;
		colors[ImGuiCol_TextSelectedBg] = gp.TextSelectedBg;
		colors[ImGuiCol_DragDropTarget] = gp.DragDropTarget;
		colors[ImGuiCol_NavHighlight] = gp.NavHighlight;
		colors[ImGuiCol_NavWindowingHighlight] = gp.NavWindowingHighlight;
		colors[ImGuiCol_NavWindowingDimBg] = gp.NavWindowingDimBg;
		colors[ImGuiCol_ModalWindowDimBg] = gp.ModalWindowDibBg;
	}

}