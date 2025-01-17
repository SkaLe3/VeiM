#pragma once
#include "IDesktopPlatform.h"

class __declspec(dllexport) DesktopPlatformWindows : public IDesktopPlatform
{
public:
	DesktopPlatformWindows();

	virtual bool NormalizeEngineRootDir(std::wstring& rootDir) override;
	virtual bool IsValidRootDirectory(const std::wstring& rootDir) override;

	virtual bool GetEngineIdentifierFromRootDir(const std::wstring& rootDir, std::wstring& outIdentifier) override;
	virtual bool GetEngineIdentifierForProject(const std::wstring& projectFileName, std::wstring& outIdentifier) override;
	virtual bool GetEngineRootDirFromIdentifier(const std::wstring& identifier, std::wstring& outRootDir) override;

	virtual bool RegisterEngineInstallation(const std::wstring& rootDir, std::wstring& outIdentifier) override;
	virtual void EnumerateEngineInstallations(std::unordered_map<std::wstring, std::wstring>& outInstallations) override;

	virtual bool VerifyFileAssociations() override;
	virtual bool UpdateFileAssociations() override;

	virtual void GetRequiredRegistrySetting(std::vector<RegistryRootedKey*>& rootedKeys) override;

	virtual const TCHAR* ExecutableName(bool bRemoveExtension) override;
	virtual bool ExecElevatedProcess(const TCHAR* URL, const TCHAR* params, VeiM::int32* outReturnCode) override;

	virtual const TCHAR* GetConfigurationDir() override;
	virtual const TCHAR* BaseDir() override;
};

typedef DesktopPlatformWindows DesktopPlatform;