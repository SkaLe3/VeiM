#pragma once

#include <string>

namespace VeiM
{
	class Paths
	{
	public:
		// TODO: Change return types to std::filesystem:path
		// TODO: add using fs = std::filesystem;
		static std::wstring GetPath(const std::wstring& inPath);
		static std::wstring EngineContentDir();
		static std::wstring UserDocumentsDir();
		static std::wstring DefaultProjectsDir();
	};
}