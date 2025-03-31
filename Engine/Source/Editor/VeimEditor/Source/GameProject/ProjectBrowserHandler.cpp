#include "ProjectBrowserHandler.h"
#include "Application/Application.h"
#include "Settings/EditorMisc.h"
#include "GameProject/GameProjectUtils.h"
#include "DesktopPlatformModule.h"
#include "UI/UI.h"
#include "Misc/Paths.h"
#include "UI/Image.h"
#include "HAL/PlatformService.h"


#include <format>

namespace VeiM
{
	struct ProjectItem
	{
		String Name;
		std::wstring EngineIdentifier;
		fs::path ProjectFile;
		SharedPtr<Image> Thumbnail;

		String GetEngineLabel()
		{
			std::wstring baseDir = Paths::GetPath(DesktopPlatformModule::Get()->BaseDir());
			std::wstring currentIdentifier;
			if (DesktopPlatformModule::Get()->NormalizeEngineRootDir(baseDir))
			{
				DesktopPlatformModule::Get()->GetEngineIdentifierFromRootDir(baseDir, currentIdentifier);
				if (EngineIdentifier == currentIdentifier)
				{
					return "Engine Current";
				}
				else
				{
					// TODO: Add version
					return "Engine ?";
				}
			}
		}
	};

	bool ProjectTileWidget(SharedPtr<ProjectItem> item, bool selected = false)
	{
		SharedPtr<Image> thumbnail = item->Thumbnail ? item->Thumbnail : UI::Theme::GetIcon("DefaultThumbnail");
		ImTextureID textureID = (ImTextureID)(void*)thumbnail->GetData();
		glm::vec2 tileSize = { 101, 150 };
		glm::vec2 imageSize = thumbnail->GetSize();
		float uvoffset = (1 - imageSize.y / imageSize.x) * 0.5f;
		ImVec2 uv0 = { uvoffset, 0 };
		ImVec2 uv1 = { 1 - uvoffset, 1 };


		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 item_size = ImGui::CalcItemSize(tileSize, imageSize.x, imageSize.y * 1.2f);

		ImRect item_rect(pos, ImVec2(pos.x + item_size.x, pos.y + item_size.y));

		ImGui::ItemSize(item_size);
		if (!ImGui::ItemAdd(item_rect, ImGui::GetID(item->Name.c_str()))) 
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(item_rect, ImGui::GetID(item->Name.c_str()), &hovered, &held);

		ImU32 colorBg = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImU32 colorHover = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImU32 colorActive = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImU32 colorText = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)); 
		ImU32 colorEngineText = IM_COL32(170, 170, 170, 255);

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

		ImVec2 name_pos = ImVec2(imageMin.x + 3, imageMax.y + 2);
		ImVec2 engine_pos = ImVec2(name_pos.x, name_pos.y + 30);
		draw_list->AddText(name_pos, colorText, item->Name.c_str());
		draw_list->AddText(engine_pos, colorEngineText, item->GetEngineLabel().c_str());

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			std::wstring engineDir;
			DesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(item->EngineIdentifier, engineDir);
			engineDir = Paths::Absolute(engineDir);

			ImGui::BeginTooltip();
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
			ImGui::Text(item->Name.c_str());
			ImGui::PopFont();
			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 2, 2 });
			ImGui::Text(item->ProjectFile.string().c_str());
			ImGui::Text(UI::Utils::WStringToString(engineDir).c_str());
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::EndTooltip();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,6 });
		float minContextMenuWidth = glm::max(ImGui::CalcTextSize(item->Name.c_str()).x, ImGui::CalcTextSize("            Show on Disk").x - 30);
		ImGui::SetNextWindowSize({ minContextMenuWidth + 40, 70 }, ImGuiCond_Always);
		if (ImGui::BeginPopupContextItem((item->Name + " Context Menu").c_str()))
		{
			UI::ShiftCursorY(10);
			ImGui::SeparatorText(item->Name.c_str());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5,5 });
			if (ImGui::MenuItem("            Show on Disk"))
			{
				PlatformService::ExploreFolder(item->ProjectFile.wstring().c_str());
			}
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(2);
		return held;
	}

	String ToLower(const String& str)
	{
		String lowerStr = str;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
		return lowerStr;
	}


	ProjectBrowserHandler::ProjectBrowserHandler()
	{
		m_CurrentSelectedProjectPath = "";
		UI::Utils::SetCharArrayData(m_SearchQuery, 128, "");

		OnUpdateProjects();
	}

	void ProjectBrowserHandler::OnGUI()
	{
		int32 tilesInLine = 7;

		m_FilteredProjectsList.clear();
		String searchQueryLower = ToLower(m_SearchQuery);
		for (const std::shared_ptr<ProjectItem>& project : m_ProjectsList)
		{
			String projectLower = ToLower(project->Name);
			if (projectLower.find(searchQueryLower) != std::string::npos)
			{
				m_FilteredProjectsList.push_back(project);
			}
		}

		ImGui::BeginChild("ContentViewSection", { 798, 345 }, ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 10 });
		ImGui::SetNextItemWidth(734);
		ImGui::InputTextWithHint("##FilterProjects", "Filter Projects...", m_SearchQuery, IM_ARRAYSIZE(m_SearchQuery)); 
		ImGui::SameLine();
		if (ImGui::Button("R", {22, 22}))
		{
			OnUpdateProjects();
		}
		RenderProjectsView();
		if (m_ProjectsList.empty())
		{
			ImGui::SetCursorPosX(300);
			UI::ShiftCursorY(40);
			ImGui::Text("You don't have any projects yet :(");
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}

	bool ProjectBrowserHandler::HasProjects() const
	{
		return !m_ProjectsList.empty();
	}

	void ProjectBrowserHandler::OnBrowseToProject()
	{
		const std::wstring projectFileDescription = TEXT("VeiM Project File");
		const std::wstring projectFileExtension = TEXT("*.vmproject");
		const std::wstring filter = std::format(TEXT("{} ({})|{}"), projectFileDescription, projectFileExtension, projectFileExtension);

		std::wstring filename;
		IDesktopPlatform* desktopPlatform = DesktopPlatformModule::Get();
		bool bSuccess = false;
		if (desktopPlatform)
		{
			void* parentWindow = Application::Get().GetWindow().GetNativeWindow();
			bSuccess = desktopPlatform->OpenFileDialog(
				parentWindow,
				TEXT("Open Project"),
				TEXT(""),
				TEXT(""),
				filter,
				filename);
		}
		if (bSuccess)
		{
			fs::path projectFilename = filename;
			m_ProjectsList.emplace_back(CreateProjectItem(projectFilename));
			//TODO: Add path to project folder in config
		}
	}

	void ProjectBrowserHandler::OnOpenProject()
	{
		OpenSelectedProject();
	}

	bool ProjectBrowserHandler::HasSelectedProjectFile() const
	{
		return !GetSelectedProjectFile().empty();
	}

	fs::path ProjectBrowserHandler::GetSelectedProjectFile() const
	{
		if (m_SelectedProjectItem)
			return m_SelectedProjectItem->ProjectFile;
		return "";
	}

	String ProjectBrowserHandler::GetSelectedProjectName() const
	{
		return m_SelectedProjectItem->Name;
	}

	void ProjectBrowserHandler::OpenSelectedProject()
	{
		if (m_CurrentSelectedProjectPath.empty())
			return;

		OpenProject(m_CurrentSelectedProjectPath);
	}

	bool ProjectBrowserHandler::OpenProject(const fs::path& inProjectFile)
	{
		String errorMessage;

		std::wstring projectFile = inProjectFile;
		std::wstring projectIdentifier;
		DesktopPlatformModule::Get()->GetEngineIdentifierForProject(projectFile, projectIdentifier);

		// TODO: Check for version, abort if project version higher than the engine;

		if (!GameProjectUtils::OpenProject(projectFile, errorMessage))
		{
			// Make Message Dialog
			return false;
		}
		return true;
	}

	void ProjectBrowserHandler::OnUpdateProjects()
	{
		LoadProjects();

		if (HasProjects())
			HandleProjectSelection(m_ProjectsList.front());
	}

	void ProjectBrowserHandler::RenderProjectsView()
	{
		for (int i = 0; i < m_FilteredProjectsList.size(); i++)
		{
			if (i % 7)
			{
				ImGui::SameLine();
			}
			bool isSelected = m_SelectedProjectItem == m_FilteredProjectsList[i];

			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().EditorColors.Accent));
			if (ProjectTileWidget(m_FilteredProjectsList[i], isSelected))
			{
				HandleProjectSelection(m_FilteredProjectsList[i]);
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				HandleProjectDoubleClick();
			}
			ImGui::PopStyleColor();
		}
	}

	void ProjectBrowserHandler::HandleProjectSelection(SharedPtr<ProjectItem> projectItem)
	{
		m_SelectedProjectItem = projectItem;
		m_CurrentSelectedProjectPath = m_SelectedProjectItem->ProjectFile;
	}

	void ProjectBrowserHandler::HandleProjectDoubleClick()
	{
		OpenSelectedProject();
	}

	SharedPtr<ProjectItem> ProjectBrowserHandler::CreateProjectItem(const fs::path& inProjectFile)
	{
		fs::path projectFilePath = fs::absolute(inProjectFile);
		fs::path thumbnailPath = (projectFilePath.parent_path() / "Saved" / "Thumbnail.png");
		String name = projectFilePath.stem().string();
		SharedPtr<Image> thumbnail = fs::exists(thumbnailPath) ? MakeShared<Image>(thumbnailPath.string()) : nullptr;
		std::wstring engineIdentifier;
		DesktopPlatformModule::Get()->GetEngineIdentifierForProject(projectFilePath, engineIdentifier);
		return MakeShared<ProjectItem>(name, engineIdentifier, projectFilePath, thumbnail);
	}

	void ProjectBrowserHandler::LoadProjects()
	{
		// Temporary, until configs are implemented
		std::vector<fs::path> createdProjectsPaths = { "D:/Dev/Projects/VeiM/Samples", fs::path(Paths::DefaultProjectsDir()) };
		for (size_t i = 0; i < createdProjectsPaths.size(); i++)
		{
			createdProjectsPaths[i] = fs::absolute(createdProjectsPaths[i]);
		}
		m_ProjectsList.clear();
		for (const auto& projectPath : createdProjectsPaths)
		{
			if (std::filesystem::exists(projectPath) && std::filesystem::is_directory(projectPath))
			{
				for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath))
				{
					if (entry.is_regular_file() && entry.path().extension() == ".vmproject")
					{
						m_ProjectsList.emplace_back(CreateProjectItem(entry.path()));
					}
				}
			}
		}
	}
}

