#pragma once
#include "VeiM/Core/CoreDefines.h"

#include <windows.h>
#include <string>
#include <unordered_map>
#include <memory>


struct RegistryValue
{
	VeiM::uint32 Type;
	std::vector<VeiM::uint8> Data;

	RegistryValue();

	void Set(const std::wstring& newValue);
	void Set(VeiM::uint32 newValue);

	bool Read(HKEY hKey, const std::wstring& name);
	bool Write(HKEY hKey, const std::wstring& name) const;
	bool IsUpToDate(HKEY hKey, const std::wstring& name) const;
};

struct RegistryKey
{

	RegistryKey() {}
	~RegistryKey();
private:
	RegistryKey(const RegistryKey&) = delete;
	RegistryKey& operator=(const RegistryKey&) = delete;


public:
	std::unordered_map<std::wstring, RegistryKey*> Keys;
	std::unordered_map<std::wstring, RegistryValue*> Values;

	RegistryKey* FindOrAddKey(const std::wstring& name);
	RegistryValue* FindOrAddValue(const std::wstring& name);

	void SetValue(const std::wstring& name, const std::wstring& value);
	void SetValue(const std::wstring& name, VeiM::uint32 value);

	bool Read(HKEY hKey);
	bool Read(HKEY hKey, const std::wstring& path);

	bool Write(HKEY hKey, bool bRemoveDifferences) const;
	bool Write(HKEY hKey, const std::wstring& path, bool bRemoveDifferences) const;

	bool IsUpToDate(HKEY hKey, bool bRemoveDifferences) const;
	bool IsUpToDate(HKEY hRootKey, const std::wstring& path, bool bRemoveDifferences) const;
};

struct RegistryRootedKey
{
	HKEY hRootKey;
	std::wstring Path;
	std::unique_ptr<RegistryKey> Key;

	RegistryRootedKey(HKEY hInKeyRoot, const std::wstring& inPath);

	bool Exists() const;

	bool Read();
	bool Write(bool bRemoveDifferences) const;
	bool IsUpToDate(bool bRemoveDifferences) const;
};

bool EnumerateRegistryKeys(HKEY hKey, std::vector<std::wstring>& outNames);
bool EnumerateRegistryValues(HKEY hKey, std::vector<std::wstring>& outNames);