#pragma once
#include "Application.h"
#include "VeiM/IO/Log.h" 
#include "VeiM/Core/CoreDefines.h"

#include <vector>
#include <string>
#include <locale>
#include <codecvt>

extern VeiM::Application* VeiM::CreateApplication(const std::vector<VeiM::String>& arguments);
bool g_ApplicationRunning = true;

namespace VeiM
{
	int Main(const std::vector<String>& arguments)
	{
		Log::Init();
		while (g_ApplicationRunning)
		{
			VeiM::Application* app = VeiM::CreateApplication(arguments);
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
	extern int __argc;
	extern char** __argv;

	std::vector<VeiM::String> arguments;
	for (int idx = 1; idx < __argc; idx++)
	{
		arguments.push_back(__argv[idx]);
	}

	return VeiM::Main(arguments);
}

#else

int main(int argC, char** argV)
{
	std::vector<VeiM::String> arguments;
	for (int idx = 1; idx < argC; idx++)
	{
		arguments.emplace_back(argV[idx]);
	}
	return VeiM::Main(arguments);
}

#endif 