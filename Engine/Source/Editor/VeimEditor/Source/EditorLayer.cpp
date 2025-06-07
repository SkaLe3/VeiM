#include "EditorLayer.h"
#include "Application/Application.h"
#include "UI/UI.h"
#include "UI/Theme.h" 
#include "HAL/PlatformService.h"
#include "Settings/EditorMisc.h"
#include "Windows/WindowsUtils.h"
#include "Engine/Reflection.h"
#include "Engine/CoreObject.h"
#include "Engine/ObjectPtr.h"

#include "Editor/Editor.h"
#include "Engine/Classes/Editor/EditorEngine.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/Entity.h"

#include "LevelEditor/LevelEditorWidgets.h"
#include "Engine/CoreObjectStatics.h"
namespace VeiM
{
	EditorLayer* g_EditorLayer;


	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
		, Widget(nullptr)
	{
		g_EditorLayer = this;
	}


	void EditorLayer::OnAttach()
	{
		CreateTitleBar();
		Application::Get().GetWindow().SetTitlebarHitTestCallback([this]() { return g_IsRunning ? m_TitleBar->IsHovered() : false; });
		EditorMisc::Get().OnInit();

		String projectFilePath;
		String gameName;
		bool bOpenProject = Application::Get().ParseProjectFilePath(projectFilePath, gameName);

		if (!bOpenProject)
		{
			m_ProjectBrowser = CreateWidget<ProjectBrowser>(this);
			m_ProjectBrowser->Open();
		}
		else
		{
			m_LevelEditor = CreateWidget<LevelEditor>(this);
		}
	}

	void EditorLayer::OnDetach()
	{
		EditorMisc::Get().OnShutdown();

	}

	void EditorLayer::OnUpdateGUI()
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
		UpdateStatus();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(3);

		m_TitleBar->OnGUI();
		if (m_LevelEditor)
		{
			float toolbarSize = 32;
			float toolBarPos = m_TitleBar->GetHeight();
			float dockspacePos = toolBarPos + toolbarSize;
			ImGui::SetCursorPosY(toolBarPos);
			m_LevelEditor->OnToolBar(toolbarSize);
			ImGui::SetCursorPosY(dockspacePos);
		}
		else
		{
			ImGui::SetCursorPosY(m_TitleBar->GetHeight());
		}


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

		OnGUI();
	}

	bool EditorLayer::OnGUI()
	{
		Widget::OnGUI();

		if (!m_ProjectBrowser)
		{
			RenderGCInfo();
			RenderClassRegistry();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Directional Light shadow map");
			UpdateStatus();
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			ImGui::Image(reinterpret_cast<void*>(Application::Get().DebugGetFramebufferTexture()), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
			ImGui::PopStyleVar();
		}
		return true;
	}

	bool EditorLayer::OnKeyType(const InputKeyTypeEvent& keyTypeEvent)
	{
		return Widget::OnKeyType(keyTypeEvent);
	}

	bool EditorLayer::OnKeyDown(const InputKeyEvent& keyEvent)
	{
		return Widget::OnKeyDown(keyEvent);
	}

	bool EditorLayer::OnKeyUp(const InputKeyEvent& keyEvent)
	{
		return Widget::OnKeyUp(keyEvent);
	}

	bool EditorLayer::OnMouseUp(const InputMouseEvent& mouseEvent)
	{
		return Widget::OnMouseUp(mouseEvent);
	}

	bool EditorLayer::OnMouseDown(const InputMouseEvent& mouseEvent)
	{
		return Widget::OnMouseDown(mouseEvent);
	}

	bool EditorLayer::OnMouseDoubleClick(const InputMouseEvent& mouseEvent)
	{
		return Widget::OnMouseDoubleClick(mouseEvent);
	}

	bool EditorLayer::OnMouseMove(const InputMouseEvent& mouseEvent)
	{
		return Widget::OnMouseMove(mouseEvent);
	}

	bool EditorLayer::OnMouseWheel(const InputMouseEvent& mouseEvent)
	{
		return Widget::OnMouseWheel(mouseEvent);
	}

	void EditorLayer::OnFinishInput()
	{
		Widget::OnFinishInput();
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
			UpdateStatus();
			ImGui::ShowStyleEditor();
			ImGui::End();
		}
		//if (m_ImGuiWindows.UserGuide) { ImGui::ShowUserGuide(&m_ImGuiWindows.UserGuide); }
	}

	void EditorLayer::ThemeEditorRender()
	{
		if (!m_bOpenThemeEditor)
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, 6.f));
		ImGui::Begin("Theme Editor", &m_bOpenThemeEditor);
		UpdateStatus();
		ImGui::PopStyleVar();
		UI::Theme::Editor();
		Application::Get().GetGUIContext()->UpdateTheme();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::CreateTitleBar()
	{
		m_TitleBar = MakeShared<UI::TitleBar>("VeiM Engine");
		m_TitleBar->SetMenubarCallback([this]()
			{
				UI::ShiftCursorY(2);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 30));
				if (ImGui::BeginMenu("File"))
				{
					ImGui::PopStyleVar();
					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("Level");
					ImGui::PopStyleColor();
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
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("Project");
					ImGui::PopStyleColor();
					ImGui::Spacing();

					ImGui::Indent(10.f);
					if (ImGui::MenuItem("Open Project..."))
					{
						m_ProjectBrowser->Open();
					}
					ImGui::Unindent(10.f);

					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("Exit");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					if (ImGui::MenuItem("Exit", NULL, false))
					{
						Close();
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
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("GUI");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					ImGui::Indent(10.f);
					ImGuiWindowMenu();
					ImGui::MenuItem("Theme Editor", "", &m_bOpenThemeEditor);
					ImGui::Unindent(10.f);

					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("Level Editor");
					ImGui::PopStyleColor();
					ImGui::Spacing();

					ImGui::Indent(10.f);
					m_LevelEditor->OnWindowMenu();
					ImGui::Unindent(10.f);

					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::SeparatorText("Debug");
					ImGui::PopStyleColor();
					ImGui::Spacing();

					ImGui::Indent(10.f);
					ImGui::MenuItem("GC Info", "", &m_bOpenGCInfo);
					ImGui::MenuItem("Class Registry", "", &m_bOpenClassRegistry);
					ImGui::Unindent(10.f);

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


	void EditorLayer::Close()
	{
		g_Editor->CloseEditor();
	}


	void EditorLayer::RenderClassRegistry()
	{
		if (!m_bOpenClassRegistry)
			return;
		static std::unordered_map<StringID, ClassDescriptor*> classes = ClassRegistry::GetAllClasses();

		ImGui::Begin("Class Registry");
		UpdateStatus();
		ImGui::Text("Registered count: %d", classes.size());

		for (const auto& [className, classDescriptor] : classes)
		{
			if (ImGui::TreeNode(className.ToString().c_str()))
			{
				ImGui::Text("Parent: %s", classDescriptor->ParentClass ? classDescriptor->ParentClass->Name.ToString().c_str() : "No Parent");
				if (ImGui::TreeNode("Properties"))
				{
					for (const auto& [propName, propDescriptor] : classDescriptor->Properties)
					{
						if (ImGui::TreeNode(propName.ToString().data()))
						{
							ImGui::Text("Name: %s", propName.ToString().data());
							ImGui::Text("Type: %s", propDescriptor.GetTypeName());
							ImGui::Text("Offset: %d", propDescriptor.Offset);
							ImGui::Text("Size: %d", propDescriptor.Size);
							ImGui::Text("Type Index: %s", propDescriptor.TypeIndex.name());
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Methods"))
				{
					ImGui::Text("NOT YET AVAILABLE");
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void EditorLayer::RenderGCInfo()
	{
		if (!m_bOpenGCInfo)
			return;

		ImGui::Begin("GC Stats", &m_bOpenGCInfo);
		float availWidthWindow = ImGui::GetContentRegionAvail().x;
		uint32 statCount = 0;
		statCount = GarbageCollector::Get().GetObjectsCount();
		ImGui::Text("Objects Alive:"); ImGui::SameLine(availWidthWindow - ImGui::CalcTextSize(std::to_string(statCount).data()).x); ImGui::Text("%d", statCount);
		statCount = GarbageCollector::Get().GetRootObjectsCount();
		ImGui::Text("As Roots:"); ImGui::SameLine(availWidthWindow - ImGui::CalcTextSize(std::to_string(statCount).data()).x); ImGui::Text("%d", statCount);
		statCount = GarbageCollector::Get().GetStrongPtrRegisteredCount();
		ImGui::Text("StrongPtr:"); ImGui::SameLine(availWidthWindow - ImGui::CalcTextSize(std::to_string(statCount).data()).x); ImGui::Text("%d", statCount);
		statCount = GarbageCollector::Get().GetWeakPtrRegisteredCount();
		ImGui::Text("WeakPtr:"); ImGui::SameLine(availWidthWindow - ImGui::CalcTextSize(std::to_string(statCount).data()).x); ImGui::Text("%d", statCount);
		statCount = GarbageCollector::Get().GetLastCollectedCount();
		ImGui::Text("Collected Last Collection (non 0):"); ImGui::SameLine(availWidthWindow - ImGui::CalcTextSize(std::to_string(statCount).data()).x); ImGui::Text("%d", statCount);
		ImGui::Text(" Name");

		ImGui::SameLine(availWidthWindow - ImGui::CalcTextSize("Type").x);
		ImGui::Text("%s", "Type");

		if (ImGui::Button("Collect"))
		{
			GarbageCollector::Get().CollectGarbage(true);
		}

		if (ImGui::BeginListBox("##GCObjectsList", ImVec2(-FLT_MIN, -FLT_MIN)))
		{
			std::unordered_set<Object*>& gcObjects = GarbageCollector::Get().Debug_GetAllObjects();
			float availWidth = ImGui::GetContentRegionAvail().x;
			for (Object* gcObject : gcObjects)
			{
				const char* typeName = gcObject->GetClass()->Name.Get();
				float typeNameLength = ImGui::CalcTextSize(typeName).x;
				ImGui::TextUnformatted(gcObject->GetName().data());

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Creator: %s", gcObject->GetCreator() ? gcObject->GetCreator()->GetName().data() : "null");
					ImGui::EndTooltip();
				}

				ImGui::SameLine(availWidth - typeNameLength);
				ImGui::TextDisabled("%s", typeName);


			}
			ImGui::EndListBox();
		}
		ImGui::End();
	}

}