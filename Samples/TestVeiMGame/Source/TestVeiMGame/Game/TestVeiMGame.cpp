#include "TestVeiMGame.h"

#include <VeiM/Core/EntryPoint.h>
#include <VeiM/Core/PlatformService.h>

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

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = "TestGame";
		specs.AppConfig.EnableConsole = true;
		specs.AppConfig.WorkingDirectory = PlatformService::BaseDir();

		specs.WndConfig.Title = "Test Game";
		specs.WndConfig.Width = 800;
		specs.WndConfig.Height = 600;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = false;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.Mode = EWindowMode::Windowed;


		Application* app = new TestGame(specs);
		app->PushLayer(new GameLayer());
		return app;
	}
}