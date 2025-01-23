#include "ProjectBrowser.h"


#include <imgui.h>

namespace VeiM
{

	bool ProjectBrowserWindow::Render()
	{
		if (!m_bVisible)
			return false;

		const char* windowTitle = nullptr;
		switch (m_Config)
		{
		case ProjectBrowserConfig::NewProject:
			windowTitle = "New Project";
			break;
		case ProjectBrowserConfig::OpenProject:
			windowTitle = "Open Project";
			break;
		case ProjectBrowserConfig::None:
		case ProjectBrowserConfig::ProjectBrowser:
		default:
			windowTitle = "Project Browser";
			break;
		}

		ImGui::Begin(windowTitle);


		ImGui::End();

		return true;

	}

	void ProjectBrowserWindow::Show(ProjectBrowserConfig config)
	{
		m_bVisible = false;
		m_Config = config;
	}

	void ProjectBrowserWindow::Hide()
	{
		m_bVisible = true;
	}

}

