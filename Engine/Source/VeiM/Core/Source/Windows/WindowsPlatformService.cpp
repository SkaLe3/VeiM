#include "Windows/WindowsPlatformService.h"
#include "Misc/Paths.h"
#include "Logging/Log.h"

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <filesystem>
#include <string>

namespace VeiM
{

	const TCHAR* WindowsPlatformService::GetSystemErrorMessage(TCHAR* outBuffer, int32 bufferCount, int32 errorCode)
	{
		*outBuffer = TEXT('\0');
		if (errorCode == 0)
		{
			errorCode = GetLastError();
		}
		DWORD size = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			0, // Default language
			outBuffer,
			bufferCount,
			NULL
		);

		return (size > 0) ? outBuffer : nullptr;
	}

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
	}

	TCHAR* WindowsPlatformService::ExecutablePath()
	{
		static TCHAR result[512] = {};
		if (!result[0])
		{
			if (!GetModuleFileName(NULL, result, MAX_PATH))
			{
				result[0] = TCHAR('\0');
			}
		}
		return result;
	}

	bool WindowsPlatformService::CreateProc(const TCHAR* URL, const TCHAR* params, uint32* outProcessID, const TCHAR* optionalWorkingDirectory)
	{
		STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION processInfo;

		std::wstring commandLine = std::wstring(TEXT("\"")) + URL + TEXT("\" ") + params;
		if (!CreateProcess(NULL,
			commandLine.data(),
			nullptr,
			nullptr,
			FALSE,
			0,
			NULL,
			optionalWorkingDirectory,
			&startupInfo,
			&processInfo))
		{
			DWORD errorCode = GetLastError();
			TCHAR errorMessage[512];
			GetSystemErrorMessage(errorMessage, 512, errorCode);
			std::wstring messageWString(errorMessage);
			std::string messageString(messageWString.begin(), messageWString.end());
			std::wstring urlWString(URL);
			std::string urlString(urlWString.begin(), urlWString.end());
			std::wstring paramsWString(params);
			std::string paramsString(paramsWString.begin(), paramsWString.end());
			VM_CORE_WARN("Failed to Create Process: {0} ({1})", messageString, errorCode);
			VM_CORE_WARN("URL: {0} {1}", urlString, paramsString);
			if (outProcessID != nullptr)
			{
				*outProcessID = 0;
			}
			return false;
		}
		if (outProcessID != nullptr)
		{
			*outProcessID = processInfo.dwProcessId;
		}
		::CloseHandle(processInfo.hThread);
		::CloseHandle(processInfo.hProcess);
		return true;
	}

	void WindowsPlatformService::ExploreFolder(const TCHAR* filePath)
	{
		if (fs::exists(filePath))
		{
			if (fs::is_directory(filePath))
			{
				::ShellExecuteW(NULL, TEXT("open"), filePath, NULL, NULL, SW_SHOWDEFAULT);
			}
			if (fs::is_regular_file(filePath))
			{
				fs::path parentPath = fs::path(filePath).parent_path();
				::ShellExecuteW(NULL, TEXT("open"), parentPath.wstring().c_str(), NULL, NULL, SW_SHOWDEFAULT);
			}
		}
	}
}

