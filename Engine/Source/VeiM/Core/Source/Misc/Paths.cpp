#include "Paths.h"
#include "CoreDefines.h"

#include "HAL/PlatformMisc.h"
#include "Misc/Singleton.h"

#include <Windows.h>
#include <filesystem>
#include <shlobj.h>

namespace VeiM
{

	struct Paths::StaticData
	{
		std::wstring GameProjectFilePath;
		// TODO: Add Game save dir, user folder, etc.
	};

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

	std::wstring Paths::UserDocumentsDir()
	{
		TCHAR* path = nullptr;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);

		if (result == S_OK)
		{
			std::filesystem::path  documentsPath(path);
			CoTaskMemFree(path);
			return documentsPath;
		}
		return {};
	}

	std::wstring Paths::DefaultProjectsDir()
	{
		std::filesystem::path documentsDir = UserDocumentsDir();
		if (documentsDir.empty())
			return {};
		std::filesystem::path projectsDir = documentsDir / "VeiM Projects";
		if (!std::filesystem::exists(projectsDir))
			std::filesystem::create_directory(projectsDir);

		return projectsDir.wstring();
	}

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



	bool Paths::TrimPathAt(fs::path& inPath, const std::wstring& removePart)
	{
		std::wstring pathStr = inPath.wstring();
		std::wstring removePartNormalized = fs::path(removePart);
		std::replace(pathStr.begin(), pathStr.end(), L'\\', L'/');
		std::replace(removePartNormalized.begin(), removePartNormalized.end(), L'\\', L'/');
		size_t pos = pathStr.rfind(removePartNormalized);
		if (pos != std::wstring::npos)
		{
			pathStr = pathStr.substr(0, pos);
			inPath = pathStr;
			return true;
		}
		inPath = pathStr;
		return false;
	}


	bool Paths::DirectoryExists(const std::wstring& inPath)
	{
		return std::filesystem::exists(inPath) && std::filesystem::is_directory(inPath);
	}


	bool Paths::StartsWith(const std::wstring& inPath, const std::wstring& inSubPath)
	{
		std::filesystem::path full(inPath);
		std::filesystem::path sub(inSubPath);

		return std::mismatch(sub.begin(), sub.end(), full.begin()).first == sub.end();
	}

	std::wstring Paths::Replace(const std::wstring& inName, const std::wstring& inFrom, const std::wstring& inTo)
	{
		std::wstring nameCopy = inName;
		size_t startPos = 0;
		while ((startPos = nameCopy.find(inFrom, startPos)) != std::wstring::npos) {
			nameCopy.replace(startPos, inFrom.length(), inTo);
			startPos += inTo.length();
		}
		return nameCopy;
	}

	fs::path Paths::LaunchDir()
	{
		return PlatformMisc::LaunchDir();
	}

	fs::path Paths::EngineDir()
	{
		return PlatformMisc::EngineDir();
	}

	fs::path Paths::RootDir()
	{
		return PlatformMisc::RootDir();
	}

	fs::path Paths::ProjectDir()
	{
		return PlatformMisc::ProjectDir();
	}

	fs::path Paths::EngineContentDir()
	{
		return Paths::EngineDir() / "Content";
	}

	fs::path Paths::EngineConfigDir()
	{
		return Paths::EngineDir() / "Config";
	}

	fs::path Paths::ProjectContentDir()
	{
		return Paths::ProjectDir() / "Content";
	}

	fs::path Paths::ProjectConfigDir()
	{
		return Paths::ProjectDir() / "Config";
	}

	bool Paths::IsProjectFilePathSet()
	{
		return !Singleton<StaticData>::Get().GameProjectFilePath.empty();
	}

	fs::path Paths::GetProjectFilePath()
	{
		return Singleton<StaticData>::Get().GameProjectFilePath;
	}

	void Paths::SetProjectFilePath(const fs::path& newProjectFilePath)
	{
		Singleton<StaticData>::Get().GameProjectFilePath = fs::absolute(newProjectFilePath);
	}

}

