#include <VeiM/EntryPoint.h>

VeiM::Application* VeiM::CreateApplication(int argc, char** argv)
{
	VeiM::ApplicationSpecification spec;
	spec.Name = "VeiM Engine";
	spec.CustomTitleBar = true;
	spec.CenterWindow = true;
	spec.IconPath = "";

	VeiM::Application* app = new VeiM::Application();

	return app;
}