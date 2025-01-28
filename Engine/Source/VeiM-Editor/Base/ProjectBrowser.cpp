#include "ProjectBrowser.h"
#include "VeiM/Core/Application.h"
#include "VeiM/UI/UI.h"
#include "VeiM/UI/Theme.h"
#include "VeiM/Platform/Windows/WindowsUtils.h"
#include "VeiM/Core/Paths.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <filesystem>

namespace VeiM
{

	ProjectBrowserWindow::ProjectBrowserWindow()
	{
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/OtherGame/OtherGame.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/SecondGame/SecondGame.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/Pong/Pong.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/Driftaway/Driftaway.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/ThirdGame/ThirdGame.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/FlappyBird/FlappyBird.vmproject"));
		m_Projects.emplace_back(std::make_shared<ProjectInfo>("D:/Dev/Projects/VeiM/Samples/BestGame/BestGame.vmproject"));

		LoadTemplates("Templates");
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
			PropertiesSection();
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
			Close();
		}

		return true;

	}

	void ProjectBrowserWindow::Open(EProjectBrowserConfig config)
	{
		m_bOpen = true;
		m_Config = config;
		//if (m_Config == ProjectBrowserConfig::ProjectBrowser)
			//Application::Get().GetWindow().Hide();
	}

	void ProjectBrowserWindow::Close()
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

	void ProjectBrowserWindow::LoadTemplates(const std::filesystem::path& inTemplateFolder)
	{
		m_Templates.clear();
		if (std::filesystem::exists(inTemplateFolder) && std::filesystem::is_directory(inTemplateFolder))
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(inTemplateFolder))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".vmproject")
				{
					std::filesystem::path projectPath = std::filesystem::path(Paths::GetPath(entry.path().wstring()));
					std::filesystem::path thumbnailPath = projectPath / "Media" / "Thumbnail.png";
					m_Templates.emplace_back(std::make_shared<ProjectInfo>(entry.path(), thumbnailPath));
				}
			}
		}
	}

	void ProjectBrowserWindow::ContentSection()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 contentRegion = ImGui::GetContentRegionAvail();

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
		if (UI::ButtonSelectableFramed("Projects", m_Category == EContentCategory::Projects, categoryButtonSize, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent)))
		{
			m_Category = EContentCategory::Projects;
			m_SelectedProject = nullptr;
			strncpy(m_ProjectLocation, "", sizeof(m_ProjectLocation) - 1);
			m_ProjectLocation[sizeof(m_ProjectLocation) - 1] = '\0';
			strncpy(m_ProjectName, "", sizeof(m_ProjectName) - 1);
			m_ProjectName[sizeof(m_ProjectName) - 1] = '\0';
		}
		ImGui::SameLine();

		if (UI::ButtonSelectableFramed("Templates", m_Category == EContentCategory::Templates, categoryButtonSize, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent)))
		{
			m_Category = EContentCategory::Templates;

			String projectDir = std::filesystem::path(Paths::DefaultProjectsDir()).string();

			m_SelectedProject = nullptr;
			strncpy(m_ProjectLocation, projectDir.c_str(), sizeof(m_ProjectLocation) - 1);
			m_ProjectLocation[sizeof(m_ProjectLocation) - 1] = '\0';
			strncpy(m_ProjectName, "MyProject", sizeof(m_ProjectName) - 1);
			m_ProjectName[sizeof(m_ProjectName) - 1] = '\0';
		}
		ImGui::PopFont();
		ImGui::EndGroup();

		if (m_Category == EContentCategory::Projects)
		{
			ProjectsContent();
		}
		if (m_Category == EContentCategory::Templates)
		{
			TemplatesContent();
		}


	}

	void ProjectBrowserWindow::ControlSection()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		bool bProjectCategoryActive = m_Category == EContentCategory::Projects;


		glm::vec2 region = ImGui::GetContentRegionAvail();

		ImVec2 textSize = ImGui::CalcTextSize("A");
		float itemHeight = textSize.y + ImGui::GetStyle().FramePadding.y * 2;
		float groupSizeY = itemHeight * 3 + style.ItemSpacing.y * 2;
		float inputTextWidth = region.x - ImGui::CalcTextSize("Project Location").x - style.ItemSpacing.x;

		float cursorPosY = (region.y - groupSizeY) * 0.5;
		UI::ShiftCursorY(cursorPosY);

		ImGui::BeginGroup();
		if (bProjectCategoryActive)
			ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.2f, 0.2f, 0.2f, 1.f });
		ImGuiInputTextFlags inputFlags = 0;
		if (bProjectCategoryActive)
			inputFlags |= ImGuiInputTextFlags_ReadOnly;

		ImGui::Text("Project Location");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(inputTextWidth);
		ImGui::InputText("##Project Location Input", m_ProjectLocation, IM_ARRAYSIZE(m_ProjectLocation), inputFlags);


		ImGui::Text("Project Name     ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(inputTextWidth);
		ImGui::InputText("##Project Name Input", m_ProjectName, IM_ARRAYSIZE(m_ProjectName), inputFlags);
		if (bProjectCategoryActive)
			ImGui::PopStyleColor();

		ImGui::EndGroup();

		ImGui::BeginGroup();


		glm::vec2 buttonSize = { 100, itemHeight };

		cursorPosY = region.y - itemHeight;

		ImGui::SetCursorPosY(cursorPosY);
		if (bProjectCategoryActive)
		{

			if (ImGui::Button("Browse...", buttonSize))
			{
				String filename;
				bool bSuccess = FileDialogs::OpenFileDialog(Application::Get().GetWindow().GetNativeWindow(), "Open Project", "", "", "VeiM Project File|*.vmproject", filename);
				if (bSuccess)
				{
					m_Projects.emplace_back(std::make_shared<ProjectInfo>(std::filesystem::path(filename)));
					SelectProject(m_Projects.back());
				}
			}
			ImGui::SameLine();
			if (!m_SelectedProject)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::Button("Open", buttonSize);
			if (!m_SelectedProject)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		else
		{
			ImU32 accentColor = UI::Theme::Get().EditorColors.Accent;
			ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Colors::ColorWithMultipliedValue(accentColor, 1.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Colors::ColorWithMultipliedValue(accentColor, 0.8f));
			if (ImGui::Button("Create", buttonSize))
			{

			}
			ImGui::PopStyleColor(3);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", buttonSize))
		{
			Close();
		}

		ImGui::EndGroup();
	}


	void ProjectBrowserWindow::PropertiesSection()
	{
		if (m_SelectedProject)
		{
			float availableWidth = ImGui::GetContentRegionAvail().x;
			float height = (float)m_PreviewImage->GetHeight() / (float)m_PreviewImage->GetWidth() * availableWidth;
			ImVec2 previewSize = { availableWidth,  height };
			ImGui::Image((ImTextureID)m_PreviewImage->GetData(), previewSize);
		}
	}

	void ProjectBrowserWindow::ProjectsContent()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		float availableWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(availableWidth);
		ImGui::InputTextWithHint("##FilterProjects", "Filter Projects...", m_SearchQuery, IM_ARRAYSIZE(m_SearchQuery));
		m_FilteredProjects.clear();
		String searchQueryLower = ToLower(m_SearchQuery);
		for (const std::shared_ptr<ProjectInfo>& project : m_Projects)
		{
			String projectLower = ToLower(project->GetProjectName());
			if (projectLower.find(searchQueryLower) != std::string::npos)
			{
				m_FilteredProjects.push_back(project);
			}
		}
		glm::vec2 projectRegion = ImGui::GetContentRegionAvail();
		glm::vec2 tileSize = { 100, 140 };
		int32 tilesInLine = projectRegion.x / tileSize.x;
		float gap = (projectRegion.x - (tilesInLine * tileSize.x + (tilesInLine - 1) * style.ItemSpacing.x)) * 0.5f;

		for (int i = 0; i < m_FilteredProjects.size(); i++)
		{
			if (i % tilesInLine)
			{
				ImGui::SameLine();
			}
			else
			{
				UI::ShiftCursorX(gap);
			}
			bool isSelected = m_SelectedProject == m_FilteredProjects[i];

			uint32 textureID = m_FilteredProjects[i]->Thumbnail.GetData();
			glm::vec2 imageSize;
			imageSize.x = m_FilteredProjects[i]->Thumbnail.GetWidth();
			imageSize.y = m_FilteredProjects[i]->Thumbnail.GetHeight();

			ImVec4 imageUV = { 0, 0, 1, 1 };
			imageUV.z = imageSize.y / imageSize.x;
			float uvOffset = (1 - imageUV.z) * 0.5f;
			imageUV.x += uvOffset;
			imageUV.z += uvOffset;

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent));
			if (UI::SelectableTileImage(m_FilteredProjects[i]->GetProjectName().c_str(), (void*)textureID, isSelected, tileSize, { tileSize.x, tileSize.x }, imageUV))
			{
				SelectProject(m_FilteredProjects[i]);
			}
			ImGui::PopStyleColor();
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

	void ProjectBrowserWindow::TemplatesContent()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		glm::vec2 projectRegion = ImGui::GetContentRegionAvail();
		glm::vec2 tileSize = { 100, 140 };
		int32 tilesInLine = projectRegion.x / tileSize.x;
		float gap = (projectRegion.x - (tilesInLine * tileSize.x + (tilesInLine - 1) * style.ItemSpacing.x)) * 0.5f;

		for (int i = 0; i < m_Templates.size(); i++)
		{
			if (i % tilesInLine)
			{
				ImGui::SameLine();
			}
			else
			{
				UI::ShiftCursorX(gap);
			}
			bool isSelected = m_SelectedProject == m_Templates[i];

			uint32 textureID = m_Templates[i]->Thumbnail.GetData();
			glm::vec2 imageSize;
			imageSize.x = m_Templates[i]->Thumbnail.GetWidth();
			imageSize.y = m_Templates[i]->Thumbnail.GetHeight();

			ImVec4 imageUV = { 0, 0, 1, 1 };
			imageUV.z = imageSize.y / imageSize.x;
			float uvOffset = (1 - imageUV.z) * 0.5f;
			imageUV.x += uvOffset;
			imageUV.z += uvOffset;

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent));
			if (UI::SelectableTileImage(m_Templates[i]->GetProjectName().c_str(), (void*)textureID, isSelected, tileSize, { tileSize.x, tileSize.x }, imageUV))
			{
				SelectProject(m_Templates[i]);
				std::filesystem::path projectPath = std::filesystem::path(Paths::GetPath(m_SelectedProject->Path.wstring()));
				std::filesystem::path previewPath = projectPath / "Media" / "Preview.png";

				m_PreviewImage = std::make_unique<Image>(previewPath.string());
			}
			ImGui::PopStyleColor();
		}
	}

	void ProjectBrowserWindow::SelectProject(std::shared_ptr<ProjectInfo> projectInfo)
	{
		m_SelectedProject = projectInfo;
		std::string projectPath = m_SelectedProject->GetProjectPath();
		if (m_Category == EContentCategory::Projects)
		{
			strncpy(m_ProjectLocation, projectPath.c_str(), sizeof(m_ProjectLocation) - 1);
			m_ProjectLocation[sizeof(m_ProjectLocation) - 1] = '\0';
		}

		std::string projectName = m_SelectedProject->GetProjectName();
		if (m_Category == EContentCategory::Projects)
		{
			strncpy(m_ProjectName, projectName.c_str(), sizeof(m_ProjectName) - 1);
			m_ProjectName[sizeof(m_ProjectName) - 1] = '\0';
		}
	}

	VeiM::String ProjectInfo::GetProjectName() const
	{
		String name = Path.stem().filename().string();
		const std::string prefix = "Template_";

		if (name.rfind(prefix, 0) == 0) {
			return name.substr(prefix.size());
		}
		return name;
	}

}

