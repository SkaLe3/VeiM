#include <VeiM/Core/EntryPoint.h>

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
		specs.AppConfig.WorkingDirectory = std::filesystem::current_path().string() +  "/../../../";
		specs.AppConfig.EngineContentPath = "Engine/Content/";

		specs.WndConfig.Title = "VeiM Editor";
		specs.WndConfig.Width = 800;
		specs.WndConfig.Height = 600;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = true;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.IconPath = "UI/Icons/VeiM_Logo.png";
		specs.WndConfig.Mode = EWindowMode::Windowed;


		Application* app = new VeiMEditor(specs);
		app->PushLayer(new EditorLayer());
		return app;
	}
}


