#pragma once
#include "CoreDefines.h"

#include <filesystem>

namespace VeiM
{
	class GameProjectUtils
	{
	public:
		static bool IsProjectFileValid(const std::filesystem::path& projectFile, String& outErrorMessage);
		static bool ProjecFileExists(const std::filesystem::path& projectFile);

		static bool OpenProject(const std::filesystem::path& projectFile, String& outErrorMessage);
	};
}