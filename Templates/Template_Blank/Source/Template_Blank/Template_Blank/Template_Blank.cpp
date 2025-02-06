#include "Template_Blank.h"

#include <VeiM/Core/EntryPoint.h>
#include <VeiM/Core/PlatformService.h>

#include "Template_BlankLayer.h"

namespace VeiM
{
	class Template_Blank : public Application
	{
	public:
		Template_Blank(const ApplicationSpecification& appSpecs)
			: Application(appSpecs)
		{

		}

		~Template_Blank()
		{

		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = "Template_Blank";
		specs.AppConfig.EnableConsole = true;
		specs.AppConfig.WorkingDirectory = PlatformService::BaseDir();

		specs.WndConfig.Title = "Template_Blank";
		specs.WndConfig.Width = 800;
		specs.WndConfig.Height = 600;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = false;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.Mode = EWindowMode::Windowed;


		Application* app = new Template_Blank(specs);
		app->PushLayer(new Template_BlankLayer());
		return app;
	}
}