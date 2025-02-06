#include "Paths.h"
#include "CoreDefines.h"

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

	std::wstring Paths::EngineContentDir()
	{
		return L"";
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

	std::wstring Paths::EngineDir()
	{
		std::wstring dir = TEXT("../../../../Engine");
		return dir;
	}
	bool Paths::DirectoryExists(const std::wstring& inPath)
	{
		return std::filesystem::exists(inPath) && std::filesystem::is_directory(inPath);
	}


}

