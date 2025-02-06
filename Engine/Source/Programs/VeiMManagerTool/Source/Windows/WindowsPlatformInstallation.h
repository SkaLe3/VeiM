#pragma once

#include "CoreDefines.h"

#include <string>

struct WindowsPlatformInstallation
{
	static bool LaunchEditor(const std::wstring& rootDirName, const std::wstring& explicitFileName, const std::wstring& arguments);

	static bool SelectEngineInstallation(std::wstring& identifier);
};