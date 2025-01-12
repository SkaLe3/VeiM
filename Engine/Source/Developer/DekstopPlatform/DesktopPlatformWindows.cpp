#include "DesktopPlatformWindows.h"
#include "Paths.h"
#include "WindowsRegistry.h"

#include <vector>
#include <windows.h>

#include <iostream>
#include <cstdlib>

#define UNLIKELY(x)	(!!(x))

#define verify(expr) {if(UNLIKELY(!(expr))) {do {std::abort();}	while (0); } }


using namespace VeiM;

static const TCHAR* InstallationSubKey = TEXT("SOFTWARE\\SkaLe Studio\\VeiM Engine\\Builds");


static bool RemoveFromEnd(std::wstring& inPath, const std::wstring& suffix)
{
	if (inPath.size() >= suffix.size() && inPath.compare(inPath.size() - suffix.size(), suffix.size(), suffix) == 0)
	{
		inPath.resize(inPath.size() - suffix.size());
		return true;
	}
	return false;
}

DesktopPlatformWindows::DesktopPlatformWindows()
{
	// TODO: may be add installationtimestamp
}

bool DesktopPlatformWindows::NormalizeEngineRootDir(std::wstring& rootDir)
{
	std::wstring normalizedRootDir = rootDir;
	Paths::NormalizeDirectoryName(normalizedRootDir);

	if (IsValidRootDirectory(normalizedRootDir))
	{
		rootDir = normalizedRootDir;
		return true;
	}

	if (!RemoveFromEnd(normalizedRootDir, TEXT("/Engine")))
	{
		if (!RemoveFromEnd(normalizedRootDir, TEXT("/Engine/Binaries/Engine")))
		{
			RemoveFromEnd(normalizedRootDir, TEXT("/Engine/Binaries/Engine/Win64"));
		}
	}

	if (IsValidRootDirectory(normalizedRootDir))
	{
		rootDir = normalizedRootDir;
		return true;
	}

	return false;
}

bool DesktopPlatformWindows::IsValidRootDirectory(const std::wstring& rootDir)
{
	std::filesystem::path engineBinariesDirPath = std::filesystem::path(rootDir) / TEXT("Engine/Binaries");
	std::wstring engineBinariesDirName = engineBinariesDirPath.wstring();
	Paths::NormalizeDirectoryName(engineBinariesDirName);
	if (!std::filesystem::exists(engineBinariesDirName) || !std::filesystem::is_directory(engineBinariesDirName))
	{
		return false;
	}

	std::filesystem::path engineBuildDirPath = std::filesystem::path(rootDir) / TEXT("Engine/Build");
	std::wstring engineBuildDirName = engineBuildDirPath.wstring();
	Paths::NormalizeDirectoryName(engineBuildDirName);
	if (!std::filesystem::exists(engineBuildDirName) || !std::filesystem::is_directory(engineBuildDirName))
	{
		return false;
	}

	return true;
}

bool DesktopPlatformWindows::GetEngineIdentifierFromRootDir(const std::wstring& rootDir, std::wstring& outIdentifier)
{
	std::unordered_map<std::wstring, std::wstring> installations;
	EnumerateEngineInstallations(installations);

	std::wstring normalizedRootDir = rootDir;
	Paths::CollapseRelativeDirectories(normalizedRootDir);
	Paths::NormalizeDirectoryName(normalizedRootDir);

	for (const auto& installation : installations)
	{
		if (installation.second == normalizedRootDir)
		{
			outIdentifier = installation.first;
			return true;
		}
	}

	return RegisterEngineInstallation(normalizedRootDir, outIdentifier);
}

bool DesktopPlatformWindows::RegisterEngineInstallation(const std::wstring& rootDir, std::wstring& outIdentifier)
{
	bool bRes = false;

	if (IsValidRootDirectory(rootDir))
	{
		HKEY hRootKey;
		if (RegCreateKeyEx(HKEY_CURRENT_USER, InstallationSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRootKey, NULL) == ERROR_SUCCESS)
		{
			VeiM::uint32 result[4] = { 0, 0, 0, 0 };
			verify(CoCreateGuid((GUID*)&result) == S_OK);

			wchar_t buffer[40];
			swprintf(buffer, sizeof(buffer) / sizeof(wchar_t),
					 L"{%08X-%04X-%04X-%04X-%04X%08X}",
					 result[0],
					 result[1] >> 16,
					 result[1] & 0xFFFF,
					 result[2] >> 16,
					 result[2] & 0xFFFF,
					 result[3]);

			std::wstring newIdentifier = std::wstring(buffer);

			LRESULT setResult = RegSetValueEx(hRootKey, newIdentifier.c_str(), 0, REG_SZ, (const BYTE*)(rootDir.c_str()), (rootDir.length() + 1) * sizeof(TCHAR));
			RegCloseKey(hRootKey);

			if (setResult == ERROR_SUCCESS)
			{
				outIdentifier = newIdentifier;
				bRes = true;
			}
		}
	}
	return bRes;
}

void DesktopPlatformWindows::EnumerateEngineInstallations(std::unordered_map<std::wstring, std::wstring>& outInstallations)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, InstallationSubKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		// Get a list of all directories
		std::vector<std::wstring> uniqueDirectories;

		std::vector<std::wstring> invalidKeys;
		for (::DWORD index = 0;; index++)
		{
			TCHAR valueName[256];
			TCHAR valueData[MAX_PATH];
			::DWORD valueType = 0;
			::DWORD valueNameLength = sizeof(valueName) / sizeof(valueName[0]);
			::DWORD valueDataSize = sizeof(valueData);

			LRESULT result = RegEnumValue(hKey, index, valueName, &valueNameLength, NULL, &valueType, (BYTE*)&valueData[0], &valueDataSize);
			if (result == ERROR_SUCCESS)
			{
				VeiM::int32 valueDataLength = valueDataSize / sizeof(TCHAR);
				if (valueDataLength > 0 && valueData[valueDataLength - 1] == 0) valueDataLength--;

				std::wstring installDirectory = std::wstring(valueData, valueDataLength);
				installDirectory = std::filesystem::canonical(std::filesystem::path(installDirectory)).wstring(); // Collapse relative directories
				Paths::NormalizeDirectoryName(installDirectory);

				if (IsValidRootDirectory(installDirectory))
				{
					outInstallations.insert({ valueName, installDirectory });
					uniqueDirectories.push_back(installDirectory);
				}
				else
				{
					invalidKeys.push_back(valueName);
				}
			}
			else if (result == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
		}

		for (const std::wstring& invalidKey : invalidKeys)
		{
			RegDeleteValue(hKey, invalidKey.c_str());
		}

		RegCloseKey(hKey);

	}
}

bool DesktopPlatformWindows::VerifyFileAssociations()
{
	std::vector<RegistryRootedKey*> keys;
	GetRequiredRegistrySetting(keys);

	for (int32 idx = 0; idx < keys.size(); idx++)
	{
		if (!keys[idx]->IsUpToDate(true))
		{
			return false;
		}
	}

	return true;
}

bool DesktopPlatformWindows::UpdateFileAssociations()
{
	std::vector<RegistryRootedKey*> keys;
	GetRequiredRegistrySetting(keys);

	for (int32 idx = 0; idx < keys.size(); idx++)
	{
		if (!keys[idx]->Write(true))
		{
			return false;
		}
	}

	return true;
}

void DesktopPlatformWindows::GetRequiredRegistrySetting(std::vector<RegistryRootedKey*>& rootedKeys)
{
	std::wstring defaultVersionSelectorName = ExecutableName(false);
	if (!defaultVersionSelectorName.starts_with(TEXT("VeiMManagerTool")))
	{
		defaultVersionSelectorName = TEXT("VeiMManagerTool.exe");
	}
	std::wstring executableFileName = std::filesystem::path(Paths::Absolute(Paths::EngineDir())) / TEXT("Binaries/Engine/Win64") / defaultVersionSelectorName;

	Paths::NormalizeDirectoryName(executableFileName);
	Paths::MakeWindowsFileName(executableFileName);
	std::wstring quotedExecutableFileName = std::wstring(TEXT("\"")) + executableFileName + std::wstring(TEXT("\""));

	RegistryRootedKey* userRootExtensionKey = new RegistryRootedKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Classes\\.vmproject"));
	rootedKeys.push_back(userRootExtensionKey);

	RegistryRootedKey* rootExtensionKey = new RegistryRootedKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Classes\\.vmproject"));
	rootExtensionKey->Key = std::make_unique<RegistryKey>();
	rootExtensionKey->Key->SetValue(TEXT(""), TEXT("VeiM.ProjectFile"));
	rootedKeys.push_back(rootExtensionKey);

	RegistryRootedKey* rootFileTypeKey = new RegistryRootedKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Classes\\VeiM.ProjectFile"));
	rootFileTypeKey->Key = std::make_unique<RegistryKey>();
	rootFileTypeKey->Key->SetValue(TEXT(""), TEXT("VeiM Engine Project File"));
	rootFileTypeKey->Key->FindOrAddKey(L"DefaultIcon")->SetValue(TEXT(""), quotedExecutableFileName);
	rootedKeys.push_back(rootFileTypeKey);

	RegistryKey* shellKey = rootFileTypeKey->Key->FindOrAddKey(TEXT("shell"));

	RegistryKey* shellOpenKey = shellKey->FindOrAddKey(TEXT("open"));
	shellOpenKey->SetValue(L"", L"Open");
	shellOpenKey->FindOrAddKey(L"command")->SetValue(L"", quotedExecutableFileName + std::wstring(TEXT(" /editor \"%1\"")));

	RegistryKey* shellRunKey = shellKey->FindOrAddKey(TEXT("run"));
	shellRunKey->SetValue(L"", L"Launch game");
	shellRunKey->SetValue(L"Icon", quotedExecutableFileName);
	shellRunKey->FindOrAddKey(L"command")->SetValue(TEXT(""), quotedExecutableFileName + std::wstring(TEXT(" /game \"%1\"")));

	RegistryKey* shellProjectKey = shellKey->FindOrAddKey(TEXT("rungenproj"));
	shellProjectKey->SetValue(L"", L"Generate Visual Studio project files");
	shellProjectKey->SetValue(L"Icon", quotedExecutableFileName);
	shellProjectKey->FindOrAddKey(L"command")->SetValue(TEXT(""), quotedExecutableFileName + std::wstring(TEXT(" /projectfiles \"%1\"")));

	const TCHAR* userChoicePath = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.vmproject\\UserChoice");

	bool bDeleteUserChoiceKey = true;
	HKEY hUserChoiceKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, userChoicePath, 0, KEY_READ, &hUserChoiceKey) == S_OK)
	{
		TCHAR progId[128];
		::DWORD progIdSize = sizeof(progId);
		::DWORD progIdType = 0;
		if (RegQueryValueEx(hUserChoiceKey, TEXT("Progid"), NULL, &progIdType, (BYTE*)progId, &progIdSize) == ERROR_SUCCESS && progIdType == REG_SZ)
		{
			bDeleteUserChoiceKey = (_tcscmp(progId, _T("VeiM.ProjectFile")) != 0);
		}
		RegCloseKey(hUserChoiceKey);
	}
	if (bDeleteUserChoiceKey)
	{
		rootedKeys.push_back(new RegistryRootedKey(HKEY_CURRENT_USER, userChoicePath));
	}
}

const TCHAR* DesktopPlatformWindows::ExecutableName(bool bRemoveExtension)
{
	static TCHAR result[512] = {};
	static TCHAR resultWithExt[512] = {};
	if (!result[0])
	{
		if (GetModuleFileName(NULL, resultWithExt, MAX_PATH) > 0)
		{
			std::filesystem::path execPath = std::filesystem::path(resultWithExt);
			std::wstring fileNameWithExt = execPath.filename().wstring();
			wcscpy_s(resultWithExt, fileNameWithExt.c_str());

			std::wstring fileNameWithoutExt = execPath.stem().wstring();
			wcscpy_s(result, fileNameWithoutExt.c_str());
		}
	}
	return (bRemoveExtension ? result : resultWithExt);
}

bool DesktopPlatformWindows::ExecElevatedProcess(const TCHAR* URL, const TCHAR* params, VeiM::int32* outReturnCode)
{
	SHELLEXECUTEINFO ShellExecuteInfo;
	ZeroMemory(&ShellExecuteInfo, sizeof(ShellExecuteInfo));
	ShellExecuteInfo.cbSize = sizeof(ShellExecuteInfo);
	ShellExecuteInfo.fMask = SEE_MASK_UNICODE | SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteInfo.lpFile = URL;
	ShellExecuteInfo.lpVerb = TEXT("runas");
	ShellExecuteInfo.nShow = SW_SHOW;
	ShellExecuteInfo.lpParameters = params;

	bool bSuccess = false;
	if (ShellExecuteEx(&ShellExecuteInfo))
	{
		::WaitForSingleObject(ShellExecuteInfo.hProcess, INFINITE);
		if (outReturnCode != NULL)
		{
			verify(::GetExitCodeProcess(ShellExecuteInfo.hProcess, (::DWORD*)outReturnCode));
		}
		verify(::CloseHandle(ShellExecuteInfo.hProcess));
		bSuccess = true;
	}
	return bSuccess;
}
