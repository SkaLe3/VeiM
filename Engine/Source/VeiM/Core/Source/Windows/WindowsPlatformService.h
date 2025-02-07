#pragma once

#include "Generic/GenericPlatformService.h"
#include "CoreDefines.h"

namespace VeiM
{
	struct CORE_API WindowsPlatformService : public GenericPlatformService
	{
	public:
		static const TCHAR* GetSystemErrorMessage(TCHAR* outBuffer, int32 bufferCount, int32 errorCode);

		static const TCHAR* BaseDir();
		static void SetCurrentWorkingDirectoryToBaseDir();
		static TCHAR* ExecutablePath();
		static bool CreateProc(const TCHAR* URL, const TCHAR* params, uint32* outProcessID, const TCHAR* optionalWorkingDirectory);
		static void ExploreFolder(const TCHAR* filePath);
		static void AddDllDirPath(const TCHAR* dir);
	};

	typedef WindowsPlatformService PlatformService;
}