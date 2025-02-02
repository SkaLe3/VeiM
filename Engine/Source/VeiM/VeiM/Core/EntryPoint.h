#pragma once
#include "Application.h"
#include "VeiM/IO/Log.h"

extern VeiM::Application* VeiM::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace VeiM
{
	int Main(int argc, char** argv)
	{
		Log::Init();
		while (g_ApplicationRunning)
		{
			VeiM::Application* app = VeiM::CreateApplication(argc, argv);
			app->Run();
			delete app;
		}
		return 0;
	}
}

#ifdef VM_SHIPPING
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return VeiM::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return VeiM::Main(argc, argv);
}

#endif // VM_DIST