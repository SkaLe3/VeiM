#include "Paths.h"
#include <Windows.h>
#include <filesystem>
#include <shlobj.h>

namespace VeiM
{

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

}

