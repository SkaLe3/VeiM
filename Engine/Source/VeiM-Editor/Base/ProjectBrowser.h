#pragma once
#include "VeiM/Core/CoreDefines.h"

namespace VeiM
{
	enum class ProjectBrowserConfig : uint8
	{
		None = 0,
		ProjectBrowser,
		NewProject,
		OpenProject

	};

	class ProjectBrowserWindow
	{
	public:
		bool Render();

		void Show(ProjectBrowserConfig config = ProjectBrowserConfig::ProjectBrowser);
		void Hide();


	private:		
		ProjectBrowserConfig m_Config = ProjectBrowserConfig::ProjectBrowser;
		bool m_bVisible = false;

	};
}