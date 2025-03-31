#include "GameProject/ProjectBrowser.h"
#include "Application/Application.h"
#include "UI/UI.h"
#include "UI/Theme.h"
#include "Windows/WindowsUtils.h"
#include "Misc/Paths.h"
#include "Settings/EditorMisc.h"
#include "GameProject/GameProjectUtils.h"
#include "DesktopPlatformModule.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <filesystem>

namespace VeiM
{
	struct TemplateItem
	{
		String Name;
		String Description;
		SharedPtr<Image> Thumbnail;
		SharedPtr<Image> PreviewImage;
		fs::path projectFilename;
	};

	bool TemplateTileWidget(SharedPtr<TemplateItem> item, bool selected = false)
	{
		ImTextureID textureID = (ImTextureID)(void*)item->Thumbnail->GetData();
		glm::vec2 tileSize = { 101, 150 };
		glm::vec2 imageSize = item->Thumbnail->GetSize();
		float uvoffset = (1 - imageSize.y / imageSize.x) * 0.5f;
		ImVec2 uv0 = { uvoffset, 0 };
		ImVec2 uv1 = { 1 - uvoffset, 1 };


		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 item_size = ImGui::CalcItemSize(tileSize, imageSize.x, imageSize.y * 1.2f);

		ImRect item_rect(pos, ImVec2(pos.x + item_size.x, pos.y + item_size.y));

		ImGui::ItemSize(item_size); // Reserve space for the widget
		if (!ImGui::ItemAdd(item_rect, ImGui::GetID(item->Name.c_str()))) // Register the item
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(item_rect, ImGui::GetID(item->Name.c_str()), &hovered, &held);

		ImU32 colorBg = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImU32 colorHover = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImU32 colorActive = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImU32 colorText = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text));

		ImU32 currentColor = colorBg;

		if (hovered)
			currentColor = colorHover;

		if (held || selected)
			currentColor = colorActive;

		draw_list->AddRectFilled(item_rect.Min, item_rect.Max, currentColor, ImGui::GetStyle().FrameRounding);


		ImVec2 imageMin = item_rect.Min;
		ImVec2 imageMax = ImVec2(item_rect.Min.x + item_size.x, item_rect.Min.y + item_size.x);

		ImU32 colorImageBackground = IM_COL32(70, 70, 70, 255);
		draw_list->AddRectFilled(imageMin, imageMax, colorImageBackground, ImGui::GetStyle().FrameRounding, ImDrawFlags_RoundCornersTop);

		draw_list->AddImage(
			textureID,
			imageMin,
			imageMax,
			ImVec2(uv0.x, uv0.y),
			ImVec2(uv1.x, uv1.y),
			IM_COL32_WHITE);

		if (hovered || held || selected)
			draw_list->AddRect(item_rect.Min, item_rect.Max, currentColor, ImGui::GetStyle().FrameRounding, ImDrawFlags_None, 2);

		ImVec2 text_size = ImGui::CalcTextSize(item->Name.c_str());
		ImVec2 text_pos = ImVec2(
			imageMin.x + 3, imageMax.y + 2
		);
		draw_list->AddText(text_pos, colorText, item->Name.c_str());

		return held;
	}

	void ProjectBrowser::Open()
	{
		m_bOpen = true;
		Application::Get().GetWindow().Hide();
	}

	void ProjectBrowser::Close()
	{
		m_bOpen = false;
		Application::Get().Close();
	}

	ProjectBrowser::ProjectBrowser()
	{
		m_ProjectBrowserHandler = MakeShared<ProjectBrowserHandler>();

		UI::Utils::SetCharArrayData(m_CurrentProjectLocation, 256, "");
		UI::Utils::SetCharArrayData(m_CurrentProjectName, 256, "");

		// TODO: Use desktop platform to find all installations and read their configs
		m_TemplatesList = LoadTemplateProjects();
		SetDefaultProjectLocation();

		m_bCreationValid = true;
		m_bUseVisualStudio = true;
		m_Mode = EProjectBrowserMode::Projects;

		if (!m_TemplatesList.empty())
			m_SelectedTempalteItem = m_TemplatesList.front();

		m_OpenButton = [this]() { if (ImGui::Button("Open", ImVec2{ 100, 22 })) m_ProjectBrowserHandler->OnOpenProject(); };
		m_CreateButton = [this]() { if (ImGui::Button("Create", ImVec2{ 100, 22 })) CreateAndOpenProject(); };
	}

	bool ProjectBrowser::OnGUI()
	{
		if (!m_bOpen)
			return false;

		UpdateCreationValidity();

		glm::vec2 screenSize = Application::Get().GetWindow().GetScreenSize();
		ImVec2 windowPos = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoSavedSettings;

		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(800, 600));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TitleBg)));

		ImGui::Begin("Project Browser", &m_bOpen, windowFlags);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 3));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 10));
		RenderContentViewSection(m_Mode);

		if (m_Mode == EProjectBrowserMode::Projects)
			RenderProjectsPathSection();
		if (m_Mode == EProjectBrowserMode::Templates)
			RenderTemplatesPathSection();

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::End();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(4);

		// Handle window closed by user
		if (!m_bOpen)
		{
			Close();
		}
		return true;
	}

	SharedPtr<TemplateItem> ProjectBrowser::GetSelectedTemplateItem() const
	{
		return m_SelectedTempalteItem;
	}

	std::vector<std::shared_ptr<TemplateItem>> ProjectBrowser::LoadTemplateProjects()
	{
		std::vector<std::shared_ptr<TemplateItem>> templates;
		std::wstring templateRootFolder = TEXT("Templates");

		if (std::filesystem::exists(templateRootFolder) && std::filesystem::is_directory(templateRootFolder))
		{
			for (const auto& entry : std::filesystem::recursive_directory_iterator(templateRootFolder))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".vmproject")
				{
					std::filesystem::path projectFile = fs::absolute(entry.path());
					std::filesystem::path projectPath = std::filesystem::path(Paths::GetPath(entry.path().wstring()));
					std::filesystem::path thumbnailPath = projectPath / "Media" / "Thumbnail.png";
					std::filesystem::path previewPath = projectPath / "Media" / "Preview.png";

					String templateName = projectFile.stem().filename().string();
					const std::string templatePrefix = "Template_";
					if (templateName.rfind(templatePrefix, 0) == 0) {
						templateName = templateName.substr(templatePrefix.size());
					}
					SharedPtr<Image> thumbnailImage = MakeShared<Image>(thumbnailPath.string());
					SharedPtr<Image> previewImage = MakeShared<Image>(previewPath.string());

					SharedPtr<TemplateItem> templateProject = MakeShared<TemplateItem>(templateName,
						"Description placeholder. Description should be retrieved from some info file in template",
						thumbnailImage,
						previewImage,
						projectFile);

					templates.emplace_back(templateProject);
				}
			}
		}
		return templates;
	}

	void ProjectBrowser::HandlePathBrowseButtonClicked()
	{
		IDesktopPlatform* desktopPlatform = DesktopPlatformModule::Get();
		if (desktopPlatform)
		{
			std::wstring folderName;
			bool bSuccess = desktopPlatform->OpenDirectoryDialog(Application::Get().GetWindow().GetNativeWindow(), TEXT("Choose a project location"), m_LastBrowsePath, folderName);
			if (bSuccess)
			{
				folderName = Paths::Absolute(folderName);
				m_LastBrowsePath = folderName;
				UI::Utils::SetCharArrayData(m_CurrentProjectLocation, 256, UI::Utils::WStringToString(folderName));
			}
		}
	}

	void ProjectBrowser::UpdateCreationValidity()
	{
		m_bCreationValid = true;
		SharedPtr<TemplateItem> selectedTemplateItem = GetSelectedTemplateItem();
		if (!selectedTemplateItem)
		{
			m_bCreationValid = false;
			m_CreationErrorMessage = "No template selected";
		}
		else
		{
			fs::path currentProjectFilePath = m_CurrentProjectLocation;
			fs::path currentProjectFileName = m_CurrentProjectName;
			if (!currentProjectFilePath.is_absolute())
			{
				m_bCreationValid = false;
				m_CreationErrorMessage = "The folder path is invalid";
			}
			else
			{
				String errorMessage;
				if (!GameProjectUtils::IsProjectFileValid(GetProjectFilename(), errorMessage))
				{
					m_bCreationValid = false;
					m_CreationErrorMessage = errorMessage;
				}
			}
			if (std::distance(currentProjectFileName.begin(), currentProjectFileName.end()) > 1)
			{
				m_bCreationValid = false;
				m_CreationErrorMessage = "The project may not contain path separator";
			}
			else
			{
				String errorMessage;
				if (!GameProjectUtils::IsProjectFileValid(GetProjectFilename(), errorMessage))
				{
					m_bCreationValid = false;
					m_CreationErrorMessage = errorMessage;
				}
			}
		}
	}

	fs::path ProjectBrowser::GetCurrentProjectFileName() const
	{
		return m_Mode == EProjectBrowserMode::Projects ? m_ProjectBrowserHandler->GetSelectedProjectName() : m_CurrentProjectName;
	}

	fs::path ProjectBrowser::GetCurrentProjectFilePath() const
	{
		return m_Mode == EProjectBrowserMode::Projects ? m_ProjectBrowserHandler->GetSelectedProjectFile() : m_CurrentProjectName;

	}

	VeiM::fs::path ProjectBrowser::GetProjectFilename() const
	{
		if (m_Mode == EProjectBrowserMode::Templates);
		fs::path currentProjectLocation(m_CurrentProjectLocation);
		if (currentProjectLocation.empty())
		{
			return fs::path("");
		}
		else
		{
			const fs::path projectName = m_CurrentProjectName;
			const fs::path projectPath = fs::absolute(m_CurrentProjectLocation);
			const fs::path filename = projectName.string() + "." + "vmproject";
			fs::path projectFilename = projectPath / projectName / filename;
			projectFilename = fs::absolute(projectFilename);
			return projectFilename;
		}
	}

	void ProjectBrowser::RenderContentViewSection(EProjectBrowserMode mode)
	{
		ImGui::BeginChild("ModeSelection", { 800, 80 }, ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		ImGui::BeginGroup();
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		if (UI::ButtonSelectableFramed("Projects", m_Mode == EProjectBrowserMode::Projects, { 379, 60 }, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent)))
		{
			m_Mode = EProjectBrowserMode::Projects;
			UI::Utils::SetCharArrayData(m_CurrentProjectLocation, 256, "");
			UI::Utils::SetCharArrayData(m_CurrentProjectName, 256, "");
		}
		ImGui::SameLine();
		if (UI::ButtonSelectableFramed("Templates", m_Mode == EProjectBrowserMode::Templates, { 379, 60 }, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent)))
		{
			m_Mode = EProjectBrowserMode::Templates;
			String projectDir = std::filesystem::path(Paths::DefaultProjectsDir()).string();
			UI::Utils::SetCharArrayData(m_CurrentProjectLocation, 256, projectDir.c_str());
			UI::Utils::SetCharArrayData(m_CurrentProjectName, 256, "MyProject");
		}
		ImGui::PopFont();
		ImGui::EndGroup();
		ImGui::PopStyleVar();
		ImGui::EndChild();

		if (mode == EProjectBrowserMode::Projects)
		{
			m_ProjectBrowserHandler->OnGUI();
		}
		if (mode == EProjectBrowserMode::Templates)
		{
			RenderTemplatesView();
		}
	}

	void ProjectBrowser::RenderTemplatesView()
	{
		int32 tilesInLine = 5;

		ImGui::BeginChild("ContentViewSection", { 577, 345 }, ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		for (int i = 0; i < m_TemplatesList.size(); i++)
		{
			if (i % tilesInLine)
			{
				ImGui::SameLine();
			}
			bool isSelected = m_SelectedTempalteItem == m_TemplatesList[i];

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent));
			if (TemplateTileWidget(m_TemplatesList[i], isSelected))
			{
				OnSelectTemplate(m_TemplatesList[i]);
				HandleTemplateSelection(m_TemplatesList[i]);
			}
			ImGui::PopStyleColor();
		}

		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
		ImGui::BeginChild("TemplateProperties", { 221, 345 }, ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		RenderTemplateProperties();
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void ProjectBrowser::RenderTemplatesPathSection()
	{
		ImGui::BeginChild("PathSection", { 800, 143 }, ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		ImGui::SetCursorPosY(22);
		ImGui::BeginGroup();
		ImGui::Text("Project Location"); ImGui::SameLine();
		ImGui::SetNextItemWidth(632);
		ImGui::InputText("##Project Location Input", m_CurrentProjectLocation, IM_ARRAYSIZE(m_CurrentProjectLocation)); ImGui::SameLine(0, 4);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2, 2 });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		if (ImGui::ImageButton((void*)(intptr_t)UI::Theme::GetIcon("Folder")->GetData(), ImVec2(18, 18)))
		{
			HandlePathBrowseButtonClicked();
		}
		ImGui::SetItemTooltip("Browse for a folder");
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::Text("Project Name     "); ImGui::SameLine();
		ImGui::SetNextItemWidth(658);
		ImGui::InputText("##Project Name Input", m_CurrentProjectName, IM_ARRAYSIZE(m_CurrentProjectName));
		ImGui::EndGroup();
		ImGui::SetCursorPosY(98);
		RenderTemplatesDialogButtons();
		ImGui::SameLine();
		ImGui::Checkbox("Use Visual Studio", &m_bUseVisualStudio);
		if (!m_bCreationValid)
		{
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.f, 0.2f, 0.2f, 1.0f });
			ImGui::Text(m_CreationErrorMessage.c_str());
			ImGui::PopStyleColor();
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}

	void ProjectBrowser::RenderProjectsPathSection()
	{
		char projectFileLocation[256];
		char projectFileName[256];
		UI::Utils::SetCharArrayData(projectFileLocation, 256, GetCurrentProjectFilePath().string());
		UI::Utils::SetCharArrayData(projectFileName, 256, GetCurrentProjectFileName().string());

		ImGui::BeginChild("PathSection", { 800, 143 }, ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		ImGui::SetCursorPosY(22);
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.2f, 0.2f, 0.2f, 1.f });
		ImGui::Text("Project Location"); ImGui::SameLine();
		ImGui::SetNextItemWidth(658);
		ImGui::InputText("##Project Location Input", projectFileLocation, IM_ARRAYSIZE(projectFileLocation), ImGuiInputTextFlags_ReadOnly);
		ImGui::Text("Project Name     "); ImGui::SameLine();
		ImGui::SetNextItemWidth(658);
		ImGui::InputText("##Project Name Input", projectFileName, IM_ARRAYSIZE(projectFileName), ImGuiInputTextFlags_ReadOnly);
		ImGui::PopStyleColor();
		ImGui::EndGroup();
		ImGui::SetCursorPosY(98);
		RenderProjectsDialogButtons();
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}

	void ProjectBrowser::RenderTemplatesDialogButtons()
	{
		ImU32 accentColor = UI::Theme::Get().EditorColors.Accent;
		ImGui::BeginGroup();
		ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Colors::ColorWithMultipliedValue(accentColor, 1.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Colors::ColorWithMultipliedValue(accentColor, 0.8f));
		UI::CondEnableWidget(m_CreateButton, CanCreateProject()); ImGui::SameLine();
		ImGui::PopStyleColor(3);
		if (ImGui::Button("Cancel", ImVec2{ 100, 22 })) OnCancel();
		ImGui::EndGroup();
	}

	void ProjectBrowser::RenderProjectsDialogButtons()
	{
		ImGui::BeginGroup();
		if (ImGui::Button("Browse...", ImVec2{ 100, 22 })) m_ProjectBrowserHandler->OnBrowseToProject(); ImGui::SameLine();
		UI::CondEnableWidget(m_OpenButton, m_ProjectBrowserHandler->HasSelectedProjectFile()); ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2{ 100, 22 }))	OnCancel();
		ImGui::EndGroup();
	}

	void ProjectBrowser::RenderTemplateProperties()
	{
		SharedPtr<Image> previewImage = m_SelectedTempalteItem->PreviewImage;
		float availableWidth = ImGui::GetContentRegionAvail().x;
		float height = (float)previewImage->GetHeight() / (float)previewImage->GetWidth() * availableWidth;
		ImGui::Image((ImTextureID)previewImage->GetData(), { availableWidth,  height });
	}

	void ProjectBrowser::RenderCreatingProjectPopup()
	{

	}

	void ProjectBrowser::OnCancel() const
	{
		Application::Get().Close();
	}

	void ProjectBrowser::OnSelectTemplate(SharedPtr<TemplateItem> templateItem)
	{
		m_SelectedTempalteItem = templateItem;
	}

	void ProjectBrowser::CreateAndOpenProject()
	{
		if (!CanCreateProject())
			return;

		fs::path projectFile = GetProjectFilename();

		if (!CreateProject(projectFile))
			return;

		if (!m_bUseVisualStudio)
		{
			// TODO: Add pop up that project is created;
			m_ProjectBrowserHandler->OnUpdateProjects();
			return;
		}
		
		if (GameProjectUtils::CompileGameProject(projectFile))
		{
			Application::Get().Close();
			OpenIDE(projectFile);
			OpenProject(projectFile);
			
		}
		// TODO: Add pop up that failed to compile
	}

	bool ProjectBrowser::CreateProject(const fs::path& projectFile)
	{
		if (!m_SelectedTempalteItem)
		{
			return false;
		}
		String errorMessage;
		ProjectCreateInfo createInfo{ projectFile, m_SelectedTempalteItem->projectFilename};
		if (!GameProjectUtils::CreateProject(createInfo, errorMessage))
		{
			// TODO: Change to popup
			m_CreationErrorMessage = errorMessage;
			return false;
		}
		// TODO: Use GameProjectUtils
		// Add validation everywhere
		return true;
	}

	bool ProjectBrowser::OpenIDE(const fs::path& projectFile)
	{
		String errorMessage;
		if (!GameProjectUtils::OpenProjectInIDE(projectFile, errorMessage))
		{
			// TODO: Add popup
			return false;
		}
		return true;
	}

	bool ProjectBrowser::OpenProject(const fs::path& projectFile)
	{
		String errorMessage;
		if (!GameProjectUtils::OpenProject(projectFile, errorMessage))
		{
			// TODO: Add popup
			return false;
		}
		return true;
	}

	void ProjectBrowser::HandleTemplateSelection(SharedPtr<TemplateItem> templateItem)
	{
		UpdateCreationValidity();
	}

	void ProjectBrowser::SetDefaultProjectLocation()
	{
		std::filesystem::path defaultProjectFilePath;

		// TODO: Get previous project creation paths from editor settings from config
		// Before that, use hardcoded value
		defaultProjectFilePath = fs::path(Paths::DefaultProjectsDir());
		if (defaultProjectFilePath.empty())
		{
			// Use desktop platform to retrieve default project creation path
		}
		// Normalize and make platformfilename

		String projectName = "MyProject";

		String errorMessage;
		if (!GameProjectUtils::IsProjectFileValid(defaultProjectFilePath / projectName / (projectName + ".vmproject"), errorMessage))
		{
			VM_CORE_WARN("Failed to set default project location: {0}", errorMessage);
			projectName = "";
		}
		if (!defaultProjectFilePath.empty())
		{
			fs::path normalizedFilePath = fs::absolute(defaultProjectFilePath);
			UI::Utils::SetCharArrayData(m_CurrentProjectLocation, sizeof(m_CurrentProjectLocation), normalizedFilePath.string());
			UI::Utils::SetCharArrayData(m_CurrentProjectName, sizeof(m_CurrentProjectName), projectName);
		}
	}

	bool ProjectBrowser::CanCreateProject()
	{
		return m_bCreationValid;
	}
}

