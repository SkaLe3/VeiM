#pragma once
#include "CoreDefines.h"
#include "UI/Image.h"
#include "Application/Application.h"
#include "GameProject/ProjectBrowserHandler.h"

#include <imgui.h>

#include <filesystem>
#include <vector>

namespace VeiM
{
	enum class EProjectBrowserMode : uint8
	{
		Projects,
		Templates
	};

	struct TemplateItem;


	// TODO: Add CreatedProjectsPath in EditorSettings.ini
	// Show only path in tooltip, avoid project file
	class ProjectBrowser
	{
	public:
		void Open();
		void Close();

	public:
		ProjectBrowser();
		bool OnGUI();

	private:
		SharedPtr<TemplateItem> GetSelectedTemplateItem() const;
		std::vector<SharedPtr<TemplateItem>> LoadTemplateProjects();

		void HandlePathBrowseButtonClicked();
		void UpdateCreationValidity();
		fs::path GetCurrentProjectFileName() const;
		fs::path GetCurrentProjectFilePath() const;
		fs::path GetProjectFilename() const;

		void RenderContentViewSection(EProjectBrowserMode mode);
		void RenderTemplatesView();
		void RenderTemplatesPathSection();
		void RenderProjectsPathSection();
		void RenderTemplatesDialogButtons();
		void RenderProjectsDialogButtons();
		void RenderTemplateProperties();

		void OnCancel() const;
		void OnSelectTemplate(SharedPtr<TemplateItem> templateItem);

		void CreateAndOpenProject();
		bool CreateProject(const fs::path& projectFile);

		void HandleTemplateSelection(SharedPtr<TemplateItem> templateItem);
		void SetDefaultProjectLocation();

		bool CanCreateProject();
	private:
		bool m_bOpen = false;

		EProjectBrowserMode m_Mode = EProjectBrowserMode::Projects;

		std::vector<SharedPtr<TemplateItem>> m_TemplatesList;
		std::vector<SharedPtr<TemplateItem>> m_FilteredTemplatesList;
		SharedPtr<TemplateItem> m_SelectedTempalteItem = nullptr;

		SharedPtr<ProjectBrowserHandler> m_ProjectBrowserHandler;

		char m_CurrentProjectLocation[256];
		char m_CurrentProjectName[256];
		std::wstring m_LastBrowsePath;

		String m_CreationErrorMessage;
		bool m_bCreationValid;

		std::function<void()> m_CreateButton;
		std::function<void()> m_OpenButton;
	};
}