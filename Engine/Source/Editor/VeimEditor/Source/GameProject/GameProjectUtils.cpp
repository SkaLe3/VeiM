#include "GameProject/GameProjectUtils.h"
#include "Settings/EditorMisc.h"


#define MAX_PROJECT_NAME_LENGTH 20

namespace VeiM
{

	bool GameProjectUtils::IsProjectFileValid(const std::filesystem::path& projectFile, String& outErrorMessage)
	{
		const String baseProjectFile = projectFile.stem().string();
		if (projectFile.parent_path().empty())
		{
			outErrorMessage = "You must specify a path";
			return false;
		}
		if (baseProjectFile.empty())
		{
			outErrorMessage = "You must specify a project name";
			return false;
		}
		if (baseProjectFile.find(' ') != String::npos)
		{
			outErrorMessage = "Project name may not contain a space";
			return false;
		}
		if (!std::isalpha(baseProjectFile[0]))
		{
			outErrorMessage = "Project name must begin with an alphabetic character";
			return false;
		}
		if (baseProjectFile.length() > MAX_PROJECT_NAME_LENGTH)
		{
			outErrorMessage = String("Project name must must not be longer that ") + std::to_string(MAX_PROJECT_NAME_LENGTH) + " characters";
			return false;
		}
		if (projectFile.extension().string() != ".vmproject")
		{
			outErrorMessage = String("Invalid file extension ") + projectFile.extension().string();
			return false;
		}
		if (ProjecFileExists(projectFile))
		{
			outErrorMessage = "This project already exists";
			return false;
		}
		if (fs::absolute(projectFile.parent_path()).string().rfind(fs::absolute(fs::current_path()).string()) != String::npos)
		{
			outErrorMessage = "Project cannot be saved under Engine folder";
			return false;
		}
		bool folderIsEmpty = true;
		if (fs::is_directory(projectFile.parent_path())) {
			for (const auto& entry : fs::recursive_directory_iterator(projectFile.parent_path())) {
				if (fs::is_regular_file(entry)) {
					folderIsEmpty = false;
				}
			}
		}
		if (!folderIsEmpty)
		{
			outErrorMessage = "Project can be saved only in empty folder";
			return false;
		}
		return true;

	}

	bool GameProjectUtils::ProjecFileExists(const std::filesystem::path& projectFile)
	{
		return fs::exists(projectFile);
	}

	bool GameProjectUtils::OpenProject(const std::filesystem::path& projectFile, String& outErrorMessage)
	{
		const String baseProjectFile = projectFile.stem().string();
		if (projectFile.empty())
		{
			outErrorMessage = "You must specify a project file";
			return false;
		}
		if (baseProjectFile.find(" ") != String::npos)
		{
			outErrorMessage = "Project may not contain a space";
			return false;
		}
		if (!std::isalpha(baseProjectFile[0]))
		{
			outErrorMessage = "Project name must begin with an alphabetic character";
			return false;
		}
		if (projectFile.extension().string() != ".vmproject")
		{
			outErrorMessage = String("Invalid file extension ") + projectFile.extension().string();
			return false;
		}
		if (!fs::exists(projectFile))
		{
			outErrorMessage = projectFile.string() + " does not exist";
			return false;
		}

		EditorMisc::Get().SwitchProject(projectFile);
		return true;
	}

}