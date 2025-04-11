#include "TestVeiMGame.h"

#ifdef IS_UNIFIED
#include <Application/EntryPoint.h>
#include <HAL/PlatformService.h>

#include "GameLayer.h"

namespace VeiM
{
	class TestGame : public Application
	{
	public:
		TestGame(const ApplicationSpecification& appSpecs)
			: Application(appSpecs)
		{

		}

		~TestGame()
		{

		}
	};

	Application* CreateApplication(const std::vector<String>& arguments)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = "TestGame";
		specs.AppConfig.EnableConsole = true;
		specs.AppConfig.CommandLineArgs = arguments;

		specs.WndConfig.Title = "Test Game";
		specs.WndConfig.Width = 800;
		specs.WndConfig.Height = 600;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = false;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.Mode = EWindowMode::Windowed;
		// TODO: Make default icon


		Application* app = new TestGame(specs);
		app->PushLayer(new GameLayer());
		return app;
	}
}

#endif