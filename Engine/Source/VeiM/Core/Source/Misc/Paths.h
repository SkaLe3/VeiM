#pragma once
#include "CoreDefines.h"
#include <string>
#include <filesystem>

namespace VeiM
{
	class CORE_API Paths
	{
	public:
		// TODO: Change return types to std::filesystem:path

		static std::wstring GetPath(const std::wstring& inPath);
		static std::wstring EngineContentDir();
		static std::wstring UserDocumentsDir();
		static std::wstring DefaultProjectsDir();


		static void NormalizeDirectoryName(std::wstring& inPath);
		static void CollapseRelativeDirectories(std::wstring& inPath);
		static void MakeWindowsFileName(std::wstring& inPath);
		static std::wstring Absolute(const std::wstring& inPath);

		static bool DirectoryExists(const std::wstring& inPath);
		static bool StartsWith(const std::wstring& inPath, const std::wstring& inSubPath);
		static std::wstring Replace(const std::wstring& inName, const std::wstring& inFrom, const std::wstring& inTo);

		static fs::path LaunchDir();
		static fs::path EngineDir();
		static fs::path RootDir();
		static fs::path ProjectDir();

		static bool IsProjectFilePathSet();
		static fs::path GetProjectFilePath();
		static void SetProjectFilePath(const fs::path& newProjectFilePath);

		static bool TrimPathAt(fs::path& inPath, const std::wstring& removePart);
	private:
		struct StaticData;
		// TODO: Add paths for project location, engine dir location, config location, content location, etc.
	};
}