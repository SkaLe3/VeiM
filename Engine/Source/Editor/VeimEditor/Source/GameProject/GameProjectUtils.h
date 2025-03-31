#pragma once
#include "CoreDefines.h"

#include <filesystem>

namespace VeiM
{
	struct ProjectCreateInfo
	{
		fs::path ProjectFilename;
		fs::path TemplateFilename;
	};

	class GameProjectUtils
	{
	public:
		static bool IsProjectFileValid(const fs::path& projectFile, String& outErrorMessage);
		static bool ProjecFileExists(const fs::path& projectFile);

		static bool OpenProject(const fs::path& projectFile, String& outErrorMessage);
		static bool CreateProject(const ProjectCreateInfo& projectInfo, String& outErrorMessage);
		static bool OpenProjectInIDE(const fs::path& projectFile, String& outErrorMessage);

		static bool AssociateProjectWithEngine(const fs::path& projectFile, String& outErrorMessage);
		static bool CompileGameProject(const fs::path& projectFile);
	private:
		static bool GenerateProject(const ProjectCreateInfo& projectInfo, String& outErrorMessage);
	};
}