#pragma once

#include <VeiM/Core/CoreDefines.h>

#include <string>
#include <unordered_map>
#include <tchar.h>

class RegistryRootedKey;

class __declspec(dllexport) IDesktopPlatform
{
public:
	virtual ~IDesktopPlatform() {}

	// TODO: Make file dialogs functions
	virtual bool NormalizeEngineRootDir(std::wstring& rootDir) = 0;
	virtual bool IsValidRootDirectory(const std::wstring& rootDir) = 0;

	virtual bool GetEngineIdentifierFromRootDir(const std::wstring& rootDir, std::wstring& outIdentifier) = 0;
	virtual bool RegisterEngineInstallation(const std::wstring& rootDir, std::wstring& outIdentifier) = 0;
	virtual void EnumerateEngineInstallations(std::unordered_map<std::wstring, std::wstring>& outInstallations) = 0;

	virtual bool VerifyFileAssociations() = 0;
	virtual bool UpdateFileAssociations() = 0;

	virtual void GetRequiredRegistrySetting(std::vector<RegistryRootedKey*>& rootedKeys) = 0;

	virtual const TCHAR* ExecutableName(bool bRemoveExtension) = 0;
	virtual bool ExecElevatedProcess(const TCHAR* URL, const TCHAR* params, VeiM::int32* outReturnCode) = 0;
};


