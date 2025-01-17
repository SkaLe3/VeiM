#include "WindowsPlatformService.h"
#include "VeiM/Core/Paths.h"

#include <windows.h>
#include <tchar.h>
#include <filesystem>
#include <string>

namespace VeiM
{

	const TCHAR* WindowsPlatformService::BaseDir()
	{
		static TCHAR exePath[MAX_PATH] = TEXT("");
		GetModuleFileName(NULL, exePath, MAX_PATH);
		std::wstring exePathStr(exePath);
		std::wstring baseDir = Paths::GetPath(exePathStr);
		static TCHAR dirPath[MAX_PATH] = TEXT("");
		wcscpy_s(dirPath, baseDir.c_str());
		return dirPath;
	}

	void WindowsPlatformService::SetCurrentWorkingDirectoryToBaseDir()
	{
		std::filesystem::current_path(std::wstring(BaseDir()));
		//::SetCurrentDirectoryW(BaseDir());
	}

}

