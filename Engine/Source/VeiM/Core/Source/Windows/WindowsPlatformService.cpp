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

	ProcessHandle WindowsPlatformService::CreateProc(const TCHAR* URL, const TCHAR* params, uint32* outProcessID, const TCHAR* optionalWorkingDirectory, void* output)
	{
		SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
		HANDLE hStdOutRead = NULL, hStdOutWrite = NULL;
		HANDLE hStdErrRead = NULL, hStdErrWrite = NULL;
		bool captureOutput = (output != nullptr);
		if (captureOutput)
		{
			if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0) ||
				!CreatePipe(&hStdErrRead, &hStdErrWrite, &saAttr, 0))
			{
				VM_CORE_WARN("Failed to create pipes for process output.");
				return ProcessHandle();
			}

			SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);
			SetHandleInformation(hStdErrRead, HANDLE_FLAG_INHERIT, 0);
		}

		STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION processInfo;

		if (captureOutput)
		{
			startupInfo.dwFlags |= STARTF_USESTDHANDLES;
			startupInfo.hStdOutput = hStdOutWrite;
			startupInfo.hStdError = hStdErrWrite;
			startupInfo.hStdInput = NULL;
		}

		std::wstring commandLine = std::wstring(TEXT("\"")) + URL + TEXT("\"");
		if (params != nullptr)
		{
			commandLine += TEXT(" ");
			commandLine += params;
		}

		if (!CreateProcess(
			NULL,
			commandLine.data(),
			nullptr,
			nullptr,
			captureOutput,
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
			std::wstring paramsWString(params != nullptr ? params : TEXT(""));
			std::string paramsString(paramsWString.begin(), paramsWString.end());
			VM_CORE_WARN("Failed to Create Process: {0} ({1})", messageString, errorCode);
			VM_CORE_WARN("URL: {0} {1}", urlString, paramsString);
			if (captureOutput)
			{
				CloseHandle(hStdOutRead);
				CloseHandle(hStdOutWrite);
				CloseHandle(hStdErrRead);
				CloseHandle(hStdErrWrite);
			}
			if (outProcessID != nullptr)
			{
				*outProcessID = 0;
			}
			return ProcessHandle();
		}
		if (outProcessID != nullptr)
		{
			*outProcessID = processInfo.dwProcessId;
		}
		if (captureOutput)
		{
			CloseHandle(hStdOutWrite);
			CloseHandle(hStdErrWrite);

			constexpr DWORD BUFFER_SIZE = 4096;
			BYTE buffer[BUFFER_SIZE];
			DWORD bytesRead;
			std::size_t totalBytesRead = 0;
			std::size_t outputSize = 1024;

			if (output)
			{
				memset(output, 0, outputSize);
			}

			while (ReadFile(hStdOutRead, buffer, BUFFER_SIZE - 1, &bytesRead, NULL) || 
				ReadFile(hStdErrRead, buffer, BUFFER_SIZE - 1, &bytesRead, NULL))
			{
				std::size_t remainingSpace = outputSize - totalBytesRead - 1;
				std::size_t copySize = (bytesRead < remainingSpace) ? bytesRead : remainingSpace;

				if (copySize > 0)
				{
					memcpy(reinterpret_cast<BYTE*>(output) + totalBytesRead, buffer, copySize);
					totalBytesRead += copySize;
				}

				if (totalBytesRead >= outputSize - 1)
				{
					break; // Avoid overflow
				}
			}

			CloseHandle(hStdOutRead);
			CloseHandle(hStdErrRead);
		}

		::CloseHandle(processInfo.hThread);
		return ProcessHandle(processInfo.hProcess);
	}

	bool WindowsPlatformService::IsProcessActive(ProcessHandle& handle)
	{
		bool bIsActive = true;
		uint32 result = ::WaitForSingleObject(handle.Get(), 0);
		if (result != WAIT_TIMEOUT)
		{
			bIsActive = false;
		}
		return bIsActive;
	}

	void WindowsPlatformService::WaitForProcess(ProcessHandle& handle)
	{
		::WaitForSingleObject(handle.Get(), INFINITE);
	}

	void WindowsPlatformService::CloseProcess(ProcessHandle& handle)
	{
		if (handle.IsValid())
		{
			::CloseHandle(handle.Get());
			handle.Invalidate();
		}
	}

	uint32 WindowsPlatformService::GetProcessExitCode(ProcessHandle& handle)
	{
		DWORD exitCode;
		::GetExitCodeProcess(handle.Get(), &exitCode);
		return static_cast<uint32>(exitCode);
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

	void WindowsPlatformService::AddDllDirPath(const TCHAR* dir)
	{
		::SetDllDirectory(dir);
	}

}

