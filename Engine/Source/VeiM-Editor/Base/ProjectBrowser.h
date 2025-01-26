#pragma once
#include "VeiM/Core/CoreDefines.h"
#include "VeiM/Core/Image.h"
#include "VeiM/Core/Application.h"

#include <imgui.h>

#include <filesystem>
#include <vector>

namespace VeiM
{
	enum class EProjectBrowserConfig : uint8
	{
		None = 0,
		ProjectBrowser,
		NewProject,
		OpenProject

	};

	enum class EContentCategory : uint8
	{
		Projects,
		Templates
	};

	struct ProjectInfo
	{
		std::filesystem::path Path;
		Image Thumbnail;

		ProjectInfo(const std::filesystem::path& inProjectPath, const std::filesystem::path& inThumbnailPath)
			:Thumbnail(inThumbnailPath.string()), Path(inProjectPath) {}

		ProjectInfo(const std::filesystem::path& inProjectPath)
			: ProjectInfo(inProjectPath, Application::Get().GetEngineResourcePath() / "UI/Thumbnails/ProjectThumbnail.png") {}
	
	};



	class ProjectBrowserWindow
	{
	public:
		ProjectBrowserWindow();
		bool Render();

		void Show(EProjectBrowserConfig config = EProjectBrowserConfig::ProjectBrowser);
		void Hide();

	private:
		String ToLower(const String& str);

		void ContentSection();
		void ControlSection();

	private:
		EProjectBrowserConfig m_Config = EProjectBrowserConfig::ProjectBrowser;
		EContentCategory m_Category = EContentCategory::Projects;
		bool m_bOpen = true;

		char m_SearchQuery[128] = "";
		char m_ProjectLocation[256] = "";
		char m_ProjectName[256] = "";

		std::vector<std::shared_ptr<ProjectInfo>> m_Projects;
		std::vector<std::shared_ptr<ProjectInfo>> m_FilteredProjects;



	};
}