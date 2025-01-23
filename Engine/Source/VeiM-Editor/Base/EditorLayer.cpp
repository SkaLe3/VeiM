#include "EditorLayer.h"
#include "VeiM/Core/Application.h"
#include "VeiM/UI/UI.h"
#include "VeiM/UI/Theme.h" 

#include <iostream> // TODO: remove

#include <glad/glad.h> // TODO: remove
#include <GLFW/glfw3.h>


const char* triangleVertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* triangleFragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color
}
)";



namespace VeiM
{
	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{

	}


	void EditorLayer::OnAttach()
	{
		CreateTitleBar();
		Application::Get().GetWindow().SetTitlebarHitTestCallback([this]() { return m_TitleBar->IsHovered(); });

		// Triangle VAO
		float triangleVertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};
		triangleVAO, triangleVBO;
		glGenVertexArrays(1, &triangleVAO);
		glGenBuffers(1, &triangleVBO);

		glBindVertexArray(triangleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Compile shaders
		triangleShader = ShaderStatics::CreateProgram(triangleVertexShaderSource, triangleFragmentShaderSource);

		m_Framebuffer.Invalidate(1280, 720);
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		glfwGetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), &display_w, &display_h);
		m_Framebuffer.Invalidate(display_w, display_h);
		m_Framebuffer.Bind();

		// TODO: Put in RHI
		glClearColor(0.8, 0.8, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);



		glUseProgram(triangleShader);
		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void EditorLayer::OnGUI()
	{
		// We are using the ImGuiWindowFlags_NoDocking flag to make parent window not dockable into, 
		// because it would be confusing to have two docking targets  within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (!Application::Get().GetWindow().HasCustomTitlebar() && m_TitleBar->GetMenubarCallback())
			window_flags |= ImGuiWindowFlags_MenuBar;

		const bool isMaximized = Application::Get().GetWindow().GetWindowMode() == EWindowMode::WindowedFullscreen;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_WindowBorderSize);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, UI::Theme::Get().EditorColors.Titlebar);
		ImGui::PushStyleColor(ImGuiCol_Border, UI::Theme::Get().EditorColors.Border);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(3);

		m_TitleBar->OnGUI();
		ImGui::SetCursorPosY(m_TitleBar->GetHeight());


		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = m_MinWinSizeX;
		ImGui::DockSpace(ImGui::GetID("MyDockspace"));

		ImGuiWindowsRender();
		ThemeEditorRender();
		static GUIDebug& gdebug = Application::Get().GetGUIContext()->GetDebug();
		if (gdebug.FlashColorTime > 0)
		{
			float hue = fmod(gdebug.FlashColorTime / 0.5f, 1.0f);
			float r;
			float g;
			float b;
			ImGui::ColorConvertHSVtoRGB(hue, 1.0f, 1.0f, r, g, b);
			if (gdebug.FlashColor4)
			{
				*gdebug.FlashColor4 = ImVec4(r, g, b, 1.0f);
			}	
			if (gdebug.FlashColorU32)
			{
				*gdebug.FlashColorU32 = IM_COL32(r * 255.0f, g * 255.0f, b * 255.0f, 255.0f);
			}

			gdebug.FlashColorTime -= Application::Get().GetDeltaTime();
			if (gdebug.FlashColorTime <= 0)
			{
				UI::Theme::DebugFlashColorStopGUI();
				UI::Theme::DebugFlashColorStopEditor();
			}
		}
		ImGui::End();


		// Render
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		ImGui::Image(reinterpret_cast<void*>(m_Framebuffer.GetTexture()), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();


		m_ProjectBrowser.Render();
	}

	void EditorLayer::ImGuiWindowMenu()
	{
		if (ImGui::BeginMenu("ImGui"))
		{
			ImGui::MenuItem("Fps", "", &m_ImGuiWindows.Fps);
			ImGui::MenuItem("Demo", "", &m_ImGuiWindows.Demo);
			ImGui::MenuItem("Metrics/Debugger", "", &m_ImGuiWindows.Metrics);
			ImGui::MenuItem("Debug Log", "", &m_ImGuiWindows.DebugLog);
			ImGui::MenuItem("ID Stack Tool", "", &m_ImGuiWindows.IdStackTool);
			ImGui::MenuItem("About", "", &m_ImGuiWindows.About);
			ImGui::MenuItem("Style Editor", "", &m_ImGuiWindows.StyleEditor);
			ImGui::MenuItem("User Guide", "", &m_ImGuiWindows.UserGuide);
			ImGui::EndMenu();
		}
	}

	void EditorLayer::ImGuiWindowsRender()
	{
		if (m_ImGuiWindows.Fps) { ImGuiWindows::ShowFpsWindow(&m_ImGuiWindows.Fps); }
		if (m_ImGuiWindows.Demo) { ImGui::ShowDemoWindow(&m_ImGuiWindows.Demo); }
		if (m_ImGuiWindows.Metrics) { ImGui::ShowMetricsWindow(&m_ImGuiWindows.Metrics); }
		if (m_ImGuiWindows.DebugLog) { ImGui::ShowDebugLogWindow(&m_ImGuiWindows.DebugLog); }
		if (m_ImGuiWindows.IdStackTool) { ImGui::ShowIDStackToolWindow(&m_ImGuiWindows.IdStackTool); }
		if (m_ImGuiWindows.About) { ImGui::ShowAboutWindow(&m_ImGuiWindows.About); }
		if (m_ImGuiWindows.StyleEditor)
		{
			ImGui::Begin("Dear ImGui Style Editor", &m_ImGuiWindows.StyleEditor);
			ImGui::ShowStyleEditor();
			ImGui::End();
		}
		//if (m_ImGuiWindows.UserGuide) { ImGui::ShowUserGuide(&m_ImGuiWindows.UserGuide); }





	}

	void EditorLayer::ThemeEditorRender()
	{
		if (!m_ThemeEditor)
			return;
		ImGui::Begin("Theme Editor", &m_ThemeEditor);
		UI::Theme::Editor();
		Application::Get().GetGUIContext()->UpdateTheme();

		ImGui::End();
	}

	void EditorLayer::CreateTitleBar()
	{
		m_TitleBar = std::make_shared<UI::TitleBar>("VeiM Engine");
		m_TitleBar->SetMenubarCallback([this]()
									   {
										   UI::ShiftCursorY(2);

										   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 30));
										   if (ImGui::BeginMenu("File"))
										   {
											   ImGui::PopStyleVar();
											   ImGui::Spacing();
											   ImGui::SeparatorText("Level");
											   ImGui::Spacing();

											   ImGui::Indent(10.f);
											   if (ImGui::MenuItem("New Level", "Ctrl+N", false))
											   {
												   int32_t dummy = 2;
											   }
											   if (ImGui::MenuItem("Open Level", "Ctrl+O", false))
											   {
												   int32_t dummy = 2;
											   }
											   ImGui::Unindent(10.f);

											   ImGui::Spacing();
											   ImGui::SeparatorText("Project");
											   ImGui::Spacing();

											   ImGui::Indent(10.f);
											   if (ImGui::MenuItem("Open Project..."))
											   {
												   m_ProjectBrowser.Show();
											   }
											   ImGui::Unindent(10.f);

											   ImGui::Spacing();
											   ImGui::SeparatorText("Exit");
											   ImGui::Spacing();
											   if (ImGui::MenuItem("Exit", NULL, false))
											   {
												   Application::Get().Close();
											   }
											   ImGui::EndMenu();

										   }
										   else
										   {
											   ImGui::PopStyleVar();
										   }

										   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 30));
										   if (ImGui::BeginMenu("Edit"))
										   {
											   ImGui::PopStyleVar();
											   if (ImGui::MenuItem("Undo", "Ctrl+Z", false))
											   {
												   int32_t dummy = 2;
											   }
											   if (ImGui::MenuItem("Redo", "Ctrl+Y", false))
											   {

											   }
											   ImGui::Spacing();
											   ImGui::Separator();
											   ImGui::Spacing();
											   if (ImGui::MenuItem("Project Settings...", NULL, false))
											   {

											   }
											   ImGui::EndMenu();

										   }
										   else
										   {
											   ImGui::PopStyleVar();
										   }
										   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 30));
										   if (ImGui::BeginMenu("Window"))
										   {
											   ImGui::PopStyleVar();
											   ImGui::Spacing();
											   ImGui::Separator();
											   ImGui::Spacing();
											   ImGuiWindowMenu();
											   ImGui::MenuItem("Theme Editor", "", &m_ThemeEditor);
											   ImGui::Spacing();
											   ImGui::Separator();
											   ImGui::EndMenu();

										   }
										   else
										   {
											   ImGui::PopStyleVar();
										   }
										   ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 30));
										   if (ImGui::BeginMenu("Help"))
										   {
											   ImGui::PopStyleVar();
											   if (ImGui::MenuItem("About", NULL, false))
											   {

											   }

											   if (ImGui::MenuItem("Github", NULL, false))
											   {

											   }
											   ImGui::EndMenu();
										   }
										   else
										   {
											   ImGui::PopStyleVar();
										   }
									   });
	}


	void EditorLayer::OpenProject()
	{
	   //VeiM::String filepath = 
	}

}