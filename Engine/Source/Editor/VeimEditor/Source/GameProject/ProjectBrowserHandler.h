#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	struct ProjectItem;

	class ProjectBrowserHandler
	{
	public:
		ProjectBrowserHandler();
		~ProjectBrowserHandler() {}

		void OnGUI();

		bool HasProjects() const;
		bool HasSelectedProjectFile() const;

		fs::path GetSelectedProjectFile() const;
		String GetSelectedProjectName() const;

		void OpenSelectedProject();
		void OnBrowseToProject();
		void OnOpenProject();
		void OnUpdateProjects();

	private:
		void RenderProjectsView();

		void LoadProjects();
		bool OpenProject(const fs::path& inProjectFile);

		void HandleProjectSelection(SharedPtr<ProjectItem> projectItem);
		void HandleProjectDoubleClick();
		SharedPtr<ProjectItem> CreateProjectItem(const fs::path& inProjectFile);
	private:
		std::vector<std::shared_ptr<ProjectItem>> m_ProjectsList;
		std::vector<std::shared_ptr<ProjectItem>> m_FilteredProjectsList;
		SharedPtr<ProjectItem> m_SelectedProjectItem;

		char m_SearchQuery[128];

		fs::path m_CurrentSelectedProjectPath;
	};
}