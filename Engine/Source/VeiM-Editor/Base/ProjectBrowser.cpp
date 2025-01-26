#include "ProjectBrowser.h"
#include "VeiM/Core/Application.h"
#include "VeiM/UI/UI.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>

namespace VeiM
{

	ProjectBrowserWindow::ProjectBrowserWindow()
	{
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/TestVeiMGame/TestVeiMGame.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D/Dev/Projects/VeiM/Samples/OtherGame/OtherGame.vmproject"));
	}

	bool ProjectBrowserWindow::Render()
	{
		if (!m_bOpen)
			return false;

		ImGuiStyle& style = ImGui::GetStyle();

		// TODO: Make it as array with values
		// TODO: Add conversion to uint8
		const char* windowTitle = nullptr;
		switch (m_Config)
		{
		case EProjectBrowserConfig::NewProject:
			windowTitle = "New Project";
			break;
		case EProjectBrowserConfig::OpenProject:
			windowTitle = "Open Project";
			break;
		case EProjectBrowserConfig::None:
		case EProjectBrowserConfig::ProjectBrowser:
		default:
			windowTitle = "Project Browser";
			break;
		}

		glm::vec2 screenSize = Application::Get().GetWindow().GetScreenSize();
		ImVec2 windowSize = ImVec2(800, 600);
		ImVec2 windowPos = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);

		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(windowSize);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoSavedSettings;

		float dividerWidth = 2;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(dividerWidth, dividerWidth)); // Item Spacing - Push
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));			// Window Title Align - Push
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));						// Window Padding - Push
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));						// Frame Padding - Push
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TitleBg))); // WindowBg - Push
		ImGui::Begin(windowTitle, &m_bOpen, windowFlags);
		ImGui::PopStyleColor();																// WindowBg - Pop
		ImGui::PopStyleVar();																// Frame Padding - Pop


		int32 bHasProperties = 0;
		if (m_Category == EContentCategory::Templates) bHasProperties = 1;
		glm::vec2 region = ImGui::GetContentRegionAvail();
		glm::vec2 controlSize = { region.x, region.y * 0.25f };
		glm::vec2 propertiesSize = { bHasProperties * region.x * 0.3, bHasProperties * (region.y - controlSize.y - dividerWidth) };
		glm::vec2 contentSize = { region.x - propertiesSize.x - dividerWidth, region.y - controlSize.y - dividerWidth };

		ImGuiChildFlags childFlags = ImGuiChildFlags_AlwaysUseWindowPadding;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 10));					// Window Padding - Push
		ImGui::BeginChild("ContentSection", contentSize, childFlags);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });							// Item Spacing - Push
		ContentSection();
		ImGui::PopStyleVar();																// Item Spacing - Pop
		ImGui::EndChild();

		if (bHasProperties)
		{
			ImGui::SameLine();
			ImGui::BeginChild("PropertiesSection", propertiesSize, childFlags);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });						// Item Spacing - Push
			ImGui::Button("Dummy button");
			ImGui::SameLine();
			ImGui::Button("Dummy button 2");
			ImGui::PopStyleVar();															// Item Spacing - Pop
			ImGui::EndChild();
		}

		ImGui::BeginChild("ControlSection", controlSize, childFlags);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });							// Item Spacing - Push
		ControlSection();
		ImGui::PopStyleVar();																// Item Spacing - Pop
		ImGui::EndChild();

		ImGui::End();
		ImGui::PopStyleVar();																// Window Padding - Pop
		// Window Padding - Pop
		// Window Title Align - Pop
		ImGui::PopStyleVar(3);																// Item Spacing - Pop

		ImGui::PopStyleVar();
		// Handle window closed by user
		if (!m_bOpen)
		{
			Hide();
		}

		return true;

	}

	void ProjectBrowserWindow::Show(EProjectBrowserConfig config)
	{
		m_bOpen = true;
		m_Config = config;
		//if (m_Config == ProjectBrowserConfig::ProjectBrowser)
			//Application::Get().GetWindow().Hide();
	}

	void ProjectBrowserWindow::Hide()
	{
		m_bOpen = false;
		//Application::Get().GetWindow().Show();
	}



	VeiM::String ProjectBrowserWindow::ToLower(const String& str)
	{
		String lowerStr = str;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
		return lowerStr;
	}

	void ProjectBrowserWindow::ContentSection()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

		glm::vec4 categoryFrameColor = ImGui::ColorConvertU32ToFloat4(IM_COL32(231, 183, 17, 255));
		glm::vec2 categoryButtonSize = { contentRegion.x * 0.5 - style.ItemSpacing.x, 60 };

		glm::vec2 groupSize = categoryButtonSize * 2.f + (glm::vec2)style.ItemSpacing;
		ImVec2 groupPos(
			(contentRegion.x - groupSize.x) * 0.5f,
			(contentRegion.y - groupSize.y) * 0.5f
		);

		UI::ShiftCursorX(groupPos.x);
		ImGui::BeginGroup();

		ImFont* bigBoldFont = ImGui::GetIO().Fonts->Fonts[1];
		ImGui::PushFont(bigBoldFont);
		if (UI::ButtonSelectableFramed("Projects", m_Category == EContentCategory::Projects, categoryButtonSize, categoryFrameColor))
		{
			m_Category = EContentCategory::Projects;
		}
		ImGui::SameLine();

		if (UI::ButtonSelectableFramed("Templates", m_Category == EContentCategory::Templates, categoryButtonSize, categoryFrameColor))
		{
			m_Category = EContentCategory::Templates;
		}
		ImGui::PopFont();
		ImGui::EndGroup();

		if (m_Category == EContentCategory::Projects)
		{
			float availableWidth = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(availableWidth);
			ImGui::InputTextWithHint("##FilterProjects", "Filter Projects...", m_SearchQuery, IM_ARRAYSIZE(m_SearchQuery));
			m_FilteredProjects.clear();
			String searchQueryLower = ToLower(m_SearchQuery);
			for (const std::shared_ptr<ProjectInfo>& project : m_Projects)
			{
				String projectLower = ToLower(project->Path.stem().filename().string());
				if (projectLower.find(searchQueryLower) != std::string::npos)
				{
					m_FilteredProjects.push_back(project);
				}
			}
			for (int i = 0; i < m_FilteredProjects.size(); i++)
			{
				if (i != 0)	ImGui::SameLine();
				ImGui::Button(m_FilteredProjects[i]->Path.stem().filename().string().c_str(), { 100, 140 });
			}
			if (m_Projects.empty())
			{
				const char* noProjectMessage = "You don't have any projects yet :(";
				float textPos = (ImGui::GetWindowWidth() - ImGui::CalcTextSize(noProjectMessage).x) * 0.5f;
				ImGui::SetCursorPosX(textPos);
				UI::ShiftCursorY(40);
				ImGui::Text(noProjectMessage);
			}
		}


	}

	void ProjectBrowserWindow::ControlSection()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		glm::vec2 region = ImGui::GetContentRegionAvail();

		ImVec2 textSize = ImGui::CalcTextSize("A");
		float itemHeight = textSize.y + ImGui::GetStyle().FramePadding.y * 2;
		float groupSizeY = itemHeight * 3 + style.ItemSpacing.y * 2;
		float inputTextWidth = region.x - ImGui::CalcTextSize("Project Location").x - style.ItemSpacing.x;

		float cursorPosY = (region.y - groupSizeY) * 0.5;
		UI::ShiftCursorY(cursorPosY);

		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.2f, 0.2f, 0.2f, 1.f });

		ImGui::Text("Project Location");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(inputTextWidth);
		ImGui::InputText("##Project Location Input", m_ProjectLocation, IM_ARRAYSIZE(m_ProjectLocation));


		ImGui::Text("Project Name     ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(inputTextWidth);
		ImGui::InputText("##Project Name Input", m_ProjectName, IM_ARRAYSIZE(m_ProjectName));
		ImGui::PopStyleColor();

		ImGui::EndGroup();

		ImGui::BeginGroup();


		glm::vec2 buttonSize = { 100, itemHeight };

		cursorPosY = region.y - itemHeight;

		ImGui::SetCursorPosY(cursorPosY);
		ImGui::Button("Browse...", buttonSize);
		ImGui::SameLine();
		ImGui::Button("Open", buttonSize);
		ImGui::SameLine();
		ImGui::Button("Cancel", buttonSize);

		ImGui::EndGroup();


	}

}

