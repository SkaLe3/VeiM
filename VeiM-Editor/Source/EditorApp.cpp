#include <VeiM/EntryPoint.h>


VeiM::Application* VeiM::CreateApplication(int argc, char** argv)
{
	return new Application();
}