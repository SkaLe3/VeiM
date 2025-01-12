#pragma once

#include <string>

class Paths
{
public:
	static void NormalizeDirectoryName(std::wstring& inPath);
	static void CollapseRelativeDirectories(std::wstring& inPath);
	static void MakeWindowsFileName(std::wstring& inPath);
	static std::wstring Absolute(const std::wstring& inPath);
	static std::wstring EngineDir();

};