#include "Paths.h"
#include "VeiM/Core/CoreDefines.h"

#include <algorithm>
#include <filesystem>


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
   return TEXT("../../../../Engine/");
}
