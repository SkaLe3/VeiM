#include "EditorLayer.h"
#include "Application/Application.h"
#include "UI/UI.h"
#include "UI/Theme.h" 
#include "HAL/PlatformService.h"
#include "Settings/EditorMisc.h"
#include "Windows/WindowsUtils.h"

#include <iostream> // TODO: remove

#include "Test/Base.h"


const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
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


		TestRenderer::Init();
		// Compile shaders
		uint32 myshader = ShaderStatics::CreateProgram(vertexShaderSource, fragmentShaderSource);

		m_Mesh = new QuadMesh();

		m_Mesh->SetShader(myshader);
		m_Framebuffer.Invalidate(1280, 720);




		EditorMisc::Get().OnInit();
		std::vector<String> cmdArgs = Application::Get().GetConfig().CommandLineArgs;

		bool bOpenProject = std::any_of(cmdArgs.begin(), cmdArgs.end(), [](const String& item)
			{
				std::filesystem::path path(item);
				return path.has_extension() && path.extension() == ".vmproject";
			});


		if (!bOpenProject)
			m_ProjectBrowser.Open();
	}

	void EditorLayer::OnDetach()
	{
		EditorMisc::Get().OnShutdown();

	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		GetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), display_w, display_h);
		m_Framebuffer.Invalidate(display_w, display_h);
		m_Framebuffer.Bind();
		TestRenderer::Clear();

		TestRenderer::RenderMesh(m_Mesh);
		m_Framebuffer.UnBind();
	}

	void EditorLayer::OnGUI()
	{
		ImGui::SetCurrentContext(GUIContext::GetImGuiContext());
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(reinterpret_cast<void*>(m_Framebuffer.GetTexture()), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();


		m_ProjectBrowser.OnGUI();
		TestClassMetadataDisplay();
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, 6.f));
		ImGui::Begin("Theme Editor", &m_ThemeEditor);
		ImGui::PopStyleVar();
		UI::Theme::Editor();
		Application::Get().GetGUIContext()->UpdateTheme();

		ImGui::End();
		ImGui::PopStyleVar();

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
						m_ProjectBrowser.Open();
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


	void EditorLayer::TestClassMetadataDisplay()
	{
		const auto& classRegistry = ReflectionSystem::Get().GetClassRegistry();
		ImGui::Begin("Registered Classes");
		ImGui::Text("%d", classRegistry.size());

		for (const auto& [className, classInfo] : classRegistry)
		{
			if (ImGui::TreeNode(className.c_str()))
			{
				if (ImGui::TreeNode("Properties"))
				{
					for (const auto& prop : classInfo.properties)
					{
						ImGui::Text(prop.c_str());
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Methods"))
				{
					for (const auto& method : classInfo.methods)
					{
						ImGui::Text(method.c_str());
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}


		ImGui::End();
	}

}