#pragma once

#include "VeiM/Core/CoreDefines.h"

namespace VeiM
{
	class Time
	{
	public:
		static float GetTime();
	};

	// TODO: Move to DesktopPlatformModule
	class FileDialogs
	{
	public:
		static bool OpenFileDialog(const void* parentWindowHandle, const String& dialogTitle, const String& defaultPath, const String& defaultFile, const String& fileTypes, String& outFilename);
	};
}