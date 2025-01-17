#include <VeiM/Core/EntryPoint.h>
#include <VeiM/Core/PlatformService.h>

#include "EditorLayer.h"

namespace VeiM
{
	class VeiMEditor : public Application
	{
	public:
		VeiMEditor(const ApplicationSpecification& appSpecs)
			: Application(appSpecs)
		{

		}

		~VeiMEditor()
		{

		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = "VeiM Editor";
		specs.AppConfig.EnableConsole = true; // TODO: Add option to enable console in game shipping build, or to choose it instead of editor console in debug and development editor build
		specs.AppConfig.WorkingDirectory = PlatformService::BaseDir();
		specs.AppConfig.EngineContentPath = "Engine/Content/";

		specs.WndConfig.Title = "VeiM Editor";
		specs.WndConfig.Width = 800;
		specs.WndConfig.Height = 600;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = true;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.IconPath = "UI/Icons/VeiM_Logo.png";
		specs.WndConfig.Mode = EWindowMode::Windowed;

		// TEMP
		for (int idx = 0; idx < argc; idx++)
		{
			VM_TRACE("{0}: {1}", idx, argv[idx]);
		}

		Application* app = new VeiMEditor(specs);
		app->PushLayer(new EditorLayer());
		return app;
	}
}


