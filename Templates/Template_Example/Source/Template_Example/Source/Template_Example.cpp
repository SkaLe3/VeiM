#include "Template_Example.h"

#ifdef IS_UNIFIED
#include <VeiM/Core/EntryPoint.h>
#include <VeiM/Core/PlatformService.h>

namespace VeiM
{
	class Template_Example : public Application
	{
	public:
		Template_Example(const ApplicationSpecification& appSpecs)
			: Application(appSpecs)
		{

		}

		~Template_Example()
		{

		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = "Template_Example";
		specs.AppConfig.EnableConsole = true;
		specs.AppConfig.WorkingDirectory = PlatformService::BaseDir();
		specs.AppConfig.CommandLineArgs = arguments;

		specs.WndConfig.Title = "Template_Example";
		specs.WndConfig.Width = 800;
		specs.WndConfig.Height = 600;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = false;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.Mode = EWindowMode::Windowed;
		// TODO: Make default icon

		Application* app = new Template_Example(specs);
		return app;
	}
}

#endif
