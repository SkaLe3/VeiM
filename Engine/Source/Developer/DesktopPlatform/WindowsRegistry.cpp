#include "WindowsRegistry.h"


using namespace VeiM;

RegistryValue::RegistryValue()
{
	Type = 0;
}

void RegistryValue::Set(const std::wstring& newValue)
{
	Type = REG_SZ;
	Data.clear();
	std::vector<uint8> newValueVector((const uint8*)newValue.c_str(), (const uint8*)newValue.c_str() + (newValue.length() + 1) * sizeof(TCHAR));
	Data.insert(Data.end(), newValueVector.begin(), newValueVector.end());
}

void RegistryValue::Set(uint32 newValue)
{
	Type = REG_DWORD;
	Data.clear();
	std::vector<uint8> newValueVector((const uint8*)&newValue, (const uint8*)&newValue + sizeof(uint32));
	Data.insert(Data.end(), newValueVector.begin(), newValueVector.end());
}

bool RegistryValue::Read(HKEY hKey, const std::wstring& name)
{
	DWORD ValueType;
	DWORD ValueDataLength = 0;
	if (RegQueryValueEx(hKey, name.c_str(), NULL, &ValueType, NULL, &ValueDataLength) != ERROR_SUCCESS)
	{
		return false;
	}
	Type = (uint32)ValueType;

	Data.resize(ValueDataLength);
	if (RegQueryValueEx(hKey, name.c_str(), NULL, NULL, Data.data(), &ValueDataLength) != ERROR_SUCCESS || ValueDataLength != Data.size())
	{
		return false;
	}

	return true;
}

bool RegistryValue::Write(HKEY hKey, const std::wstring& name) const
{
	return (RegSetValueEx(hKey, name.c_str(), 0, Type, Data.data(), (DWORD)Data.size()) == ERROR_SUCCESS);
}

bool RegistryValue::IsUpToDate(HKEY hKey, const std::wstring& name) const
{
	RegistryValue other;
	return other.Read(hKey, name) && other.Type == Type && other.Data == Data;
}


RegistryKey::~RegistryKey()
{
	for (const auto& key : Keys)
	{
		delete key.second;
	}
	for (const auto& value : Values)
	{
		delete value.second;
	}
}

RegistryKey* RegistryKey::FindOrAddKey(const std::wstring& name)
{
	RegistryKey*& Key = Keys[name];
	if (Key == NULL)
	{
		Key = new RegistryKey();
	}
	return Key;
}

RegistryValue* RegistryKey::FindOrAddValue(const std::wstring& name)
{
	RegistryValue*& Value = Values[name];
	if (Value == NULL)
	{
		Value = new RegistryValue();
	}
	return Value;
}

void RegistryKey::SetValue(const std::wstring& name, const std::wstring& value)
{
	FindOrAddValue(name)->Set(value);
}

void RegistryKey::SetValue(const std::wstring& name, uint32 value)
{
	FindOrAddValue(name)->Set(value);
}

bool RegistryKey::Read(HKEY hKey)
{
	std::vector<std::wstring> keyNames;
	EnumerateRegistryKeys(hKey, keyNames);

	for (int32 idx = 0; idx < keyNames.size(); idx++)
	{
		RegistryKey* key = FindOrAddKey(keyNames[idx]);
		if (!key->Read(hKey, keyNames[idx]));
		{
			return false;
		}
	}

	std::vector<std::wstring> valueNames;
	EnumerateRegistryValues(hKey, valueNames);
	for (int32 idx = 0; idx < valueNames.size(); idx++)
	{
		RegistryValue* value = FindOrAddValue(valueNames[idx]);
		if (!value->Read(hKey, valueNames[idx]));
		{
			return false;
		}
	}
	return true;
}

bool RegistryKey::Read(HKEY hRootKey, const std::wstring& path)
{
	bool bRes = false;
	HKEY hKey;
	if (RegOpenKeyEx(hRootKey, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		bRes = Read(hKey);
		RegCloseKey(hKey);
	}
	return bRes;
}

bool RegistryKey::Write(HKEY hKey, bool bRemoveDifferences) const
{
	if (bRemoveDifferences)
	{
		std::vector<std::wstring> valueNames;
		EnumerateRegistryValues(hKey, valueNames);

		for (int32 idx = 0; idx < valueNames.size(); idx++)
		{
			if (Values.find(valueNames[idx]) == Values.end() && RegDeleteValue(hKey, valueNames[idx].c_str()) != ERROR_SUCCESS)
			{
				return false;
			}
		}

		std::vector<std::wstring> keyNames;
		EnumerateRegistryKeys(hKey, keyNames);

		for (int32 idx = 0; idx < keyNames.size(); idx++)
		{
			if (Keys.find(keyNames[idx]) == Keys.end() && RegDeleteTree(hKey, keyNames[idx].c_str()) != ERROR_SUCCESS)
			{
				return false;
			}
		}
	}

	for (const std::pair<std::wstring, RegistryKey*>& childKey : Keys)
	{
		if (!childKey.second->Write(hKey, childKey.first, bRemoveDifferences))
		{
			return false;
		}
	}

	for (const std::pair<std::wstring, RegistryValue*>& childValue : Values)
	{
		if (RegSetValueEx(hKey, childValue.first.c_str(), 0, childValue.second->Type, childValue.second->Data.data(), (DWORD)childValue.second->Data.size()) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	return true;
}

bool RegistryKey::Write(HKEY hRootKey, const std::wstring& path, bool bRemoveDifferences) const
{
	bool bRes = false;
	HKEY hKey;
	if (RegCreateKeyEx(hRootKey, path.c_str(), 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		bRes = Write(hKey, bRemoveDifferences);
		RegCloseKey(hKey);
	}
	return bRes;
}

bool RegistryKey::IsUpToDate(HKEY hKey, bool bRemoveDifferences) const
{
	if (bRemoveDifferences)
	{
		std::vector<std::wstring> valueNames;
		EnumerateRegistryValues(hKey, valueNames);

		for (int32 idx = 0; idx < valueNames.size(); idx++)
		{
			if (Values.find(valueNames[idx]) == Values.end())
			{
				return false;
			}
		}

		std::vector<std::wstring> keyNames;
		EnumerateRegistryKeys(hKey, keyNames);

		for (int32 idx = 0; idx < keyNames.size(); idx++)
		{
			if (Keys.find(keyNames[idx]) == Keys.end())
			{
				return false;
			}
		}
	}

	for (const auto& key : Keys)
	{
		if (!key.second->IsUpToDate(hKey, key.first, bRemoveDifferences))
		{
			return false;
		}
	}
	for (const auto& value : Values)
	{
		if (!value.second->IsUpToDate(hKey, value.first))
		{
			return false;
		}
	}
	return true;

}

bool RegistryKey::IsUpToDate(HKEY hRootKey, const std::wstring& path, bool bRemoveDifferences) const
{
	bool bRes = false;

	HKEY hKey;
	if (RegOpenKeyEx(hRootKey, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		bRes = IsUpToDate(hKey, bRemoveDifferences);
		RegCloseKey(hKey);
	}
	return bRes;
}

RegistryRootedKey::RegistryRootedKey(HKEY hInKeyRoot, const std::wstring& inPath)
{
	hRootKey = hInKeyRoot;
	Path = inPath;
}

bool RegistryRootedKey::Exists() const
{
	bool bRes = false;

	HKEY hKey;
	if (RegOpenKeyEx(hRootKey, Path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		bRes = true;
	}

	return bRes;
}

bool RegistryRootedKey::Read()
{
	return false;
}

bool RegistryRootedKey::Write(bool bRemoveDifferences) const
{
	bool bRes = false;
	if (Key)
	{
		bRes = Key->Write(hRootKey, Path, bRemoveDifferences);
	}
	else
	{
		bRes = (!Exists() || RegDeleteKeyEx(hRootKey, Path.c_str(), 0, 0) == ERROR_SUCCESS);
	}
	return bRes;
}

bool RegistryRootedKey::IsUpToDate(bool bRemoveDifferences) const
{
	bool bRes = false;
	if (Key)
	{
		bRes = Key->IsUpToDate(hRootKey, Path.c_str(), bRemoveDifferences);
	}
	else
	{
		bRes = !Exists();
	}
	return bRes;
}

bool EnumerateRegistryKeys(HKEY hKey, std::vector<std::wstring>& outNames)
{
	for (DWORD index = 0;; index++)
	{
		TCHAR keyName[256];
		DWORD keyNameLength = sizeof(keyName) / sizeof(keyName[0]);

		LONG result = RegEnumKeyEx(hKey, index, keyName, &keyNameLength, NULL, NULL, NULL, NULL);
		if (result == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else if (result != ERROR_SUCCESS)
		{
			return false;
		}

		outNames.push_back(keyName);
	}
	return true;
}

bool EnumerateRegistryValues(HKEY hKey, std::vector<std::wstring>& outNames)
{
	for (DWORD index = 0;; index++)
	{
		wchar_t valueName[256];
		DWORD valueNameLength = sizeof(valueName) / sizeof(valueName[0]);

		LONG result = RegEnumValue(hKey, index, valueName, &valueNameLength, NULL, NULL, NULL, NULL);
		if (result == ERROR_NO_MORE_ITEMS)
		{
			break;
		}
		else if (result != ERROR_SUCCESS)
		{
			return false;
		}

		outNames.push_back(valueName);
	}
	return true;
}
