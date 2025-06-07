#pragma once
#include "Application/Application.h"
#include "Logging/Log.h" 
#include "CoreDefines.h"

#include <vector>
#include <string>
#include <locale>
#include <codecvt>

#if !IS_UNIFIED
static VeiM::String s_AppName = "DefaultApp"; 
static VeiM::String s_AppTitle = "Default App";
#endif



extern VeiM::Application* VeiM::CreateApplication(const std::vector<VeiM::String>& arguments, const VeiM::String& name, const VeiM::String& title );

namespace VeiM
{
	int Main(const std::vector<String>& arguments)
	{
		Log::Init();
		StringID::RegisterCommonStrings();
		VeiM::Application* app = VeiM::CreateApplication(arguments, s_AppName, s_AppTitle);
		app->Run();
		delete app;

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