#include "Paths.h"
#include "DesktopPlatformModule.h"
#include "VeiM/Core/CoreDefines.h"

#include <algorithm>
#include <filesystem>


using namespace VeiM;

void Paths::NormalizeDirectoryName(std::wstring& inPath)
{
	std::replace(inPath.begin(), inPath.end(), L'\\', L'/');
	if (inPath.ends_with(L"/") && !inPath.ends_with(L"//") && !inPath.ends_with(L":/"))
	{
		inPath[inPath.size() - 1] = L'\0';
		inPath.resize(inPath.find(L'\0'));
	}
}

void Paths::CollapseRelativeDirectories(std::wstring& inPath)
{
	inPath = std::filesystem::canonical(std::filesystem::path(inPath)).wstring();
}

void Paths::MakeWindowsFileName(std::wstring& inPath)
{
	std::replace(inPath.begin(), inPath.end(), L'/', L'\\');
}

std::wstring Paths::Absolute(const std::wstring& inPath)
{
	return std::filesystem::absolute(std::filesystem::path(inPath)).wstring();
}

std::wstring Paths::EngineDir()
{
	std::wstring dir = GetPath(DesktopPlatformModule::Get()->BaseDir());
	DesktopPlatformModule::Get()->NormalizeEngineRootDir(dir);
	dir += TEXT("/Engine");
	return dir;
}

std::wstring Paths::GetPath(const std::wstring& inPath)
{
	size_t found = inPath.find_last_of(L"/\\");

	std::wstring result;
	if (found != std::wstring::npos)
	{
		result = inPath.substr(0, found);
	}
	return result;
}

bool Paths::DirectoryExists(const std::wstring& inPath)
{
	return std::filesystem::exists(inPath) && std::filesystem::is_directory(inPath);
}
