#pragma once
#include "CoreDefines.h"
#include <string>

namespace VeiM
{
	class CORE_API Paths
	{
	public:
		// TODO: Change return types to std::filesystem:path
		// TODO: add using fs = std::filesystem;
		static std::wstring GetPath(const std::wstring& inPath);
		static std::wstring EngineContentDir();
		static std::wstring UserDocumentsDir();
		static std::wstring DefaultProjectsDir();


		static void NormalizeDirectoryName(std::wstring& inPath);
		static void CollapseRelativeDirectories(std::wstring& inPath);
		static void MakeWindowsFileName(std::wstring& inPath);
		static std::wstring Absolute(const std::wstring& inPath);
		static std::wstring EngineDir();

		static bool DirectoryExists(const std::wstring& inPath);

	private:
		struct StaticData;
	};
}