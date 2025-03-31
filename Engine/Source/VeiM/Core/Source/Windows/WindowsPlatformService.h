#pragma once

#include "Generic/GenericPlatformService.h"
#include "CoreDefines.h"

namespace VeiM
{
	struct ProcessHandle : BaseProcessHandle<HANDLE>
	{

	};

	struct CORE_API WindowsPlatformService : public GenericPlatformService
	{
	public:
		static const TCHAR* GetSystemErrorMessage(TCHAR* outBuffer, int32 bufferCount, int32 errorCode);

		static const TCHAR* BaseDir();
		static void SetCurrentWorkingDirectoryToBaseDir();
		static TCHAR* ExecutablePath();
		static ProcessHandle CreateProc(const TCHAR* URL, const TCHAR* params, uint32* outProcessID, const TCHAR* optionalWorkingDirectory, void* output);
		static bool IsProcessActive(ProcessHandle& handle);
		static void WaitForProcess(ProcessHandle& handle);
		static void CloseProcess(ProcessHandle& handle);
		static uint32 GetProcessExitCode(ProcessHandle& handle);
		static void ExploreFolder(const TCHAR* filePath);
		static void AddDllDirPath(const TCHAR* dir);
	};

	typedef WindowsPlatformService PlatformService;
}