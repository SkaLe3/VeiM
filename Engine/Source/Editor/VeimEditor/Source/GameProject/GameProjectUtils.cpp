#include "GameProject/GameProjectUtils.h"
#include "Settings/EditorMisc.h"
#include "Misc/Paths.h"
#include "DesktopPlatformModule.h"

#include <fstream>
#include <sstream>

#include <imgui.h>

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

	bool GameProjectUtils::CreateProject(const ProjectCreateInfo& projectInfo, String& outErrorMessage)
	{
		// TODO: Make progressbar

		if (!IsProjectFileValid(projectInfo.ProjectFilename, outErrorMessage))
		{
			return false;
		}
		if (projectInfo.TemplateFilename.empty())
		{
			outErrorMessage = "Invalid template filename selected";
			return false;
		}
		if (!GenerateProject(projectInfo, outErrorMessage))
		{
			fs::remove_all(projectInfo.ProjectFilename.parent_path());
		}
		return true;
	}

	bool GameProjectUtils::OpenProjectInIDE(const fs::path& projectFile, String& outErrorMessage)
	{
		return DesktopPlatformModule::Get()->OpenIDE(projectFile);
	}

	bool GameProjectUtils::AssociateProjectWithEngine(const fs::path& projectFile, String& outErrorMessage)
	{
		return DesktopPlatformModule::Get()->SetEngineIdentifierForProject(projectFile.wstring(), DesktopPlatformModule::Get()->GetCurrentEngineIdentifier());
	}

	bool GameProjectUtils::CompileGameProject(const fs::path& projectFile)
	{
		bool bSuccess = DesktopPlatformModule::Get()->CompileGameProject(projectFile);


		// TODO: Add dialog about fail
		return true;
	}

	bool GameProjectUtils::GenerateProject(const ProjectCreateInfo& projectInfo, String& outErrorMessage)
	{
		bool bSuccessfullyGenerated = true;

		std::wstring projectName = projectInfo.ProjectFilename.stem().wstring();
		std::wstring templateName = projectInfo.TemplateFilename.stem().wstring();
		fs::path sourceDirectory = projectInfo.TemplateFilename.parent_path();
		fs::path destinationDirectory = projectInfo.ProjectFilename.parent_path();

		std::vector<std::wstring> ignoredDirectories = { TEXT("Media") };
		std::vector<std::pair<std::wstring, std::wstring>> renameDirectories =
		{
			{std::wstring(TEXT("Source\\")) + templateName, std::wstring(TEXT("Source\\")) + projectName}
		};
		std::vector<std::pair<std::wstring, std::wstring>> renameInFiles =
		{
			{templateName, projectName}
		};

		std::vector<fs::path> filesToReplaceInside;

		VM_TRACE("Started project generation: '{0}'. Tempalte: '{1}'", String(projectName.begin(), projectName.end()), String(templateName.begin(), templateName.end()));

		if (!fs::exists(projectInfo.TemplateFilename) || !fs::is_regular_file(projectInfo.TemplateFilename))
		{
			outErrorMessage = "Template project does not exist";
			return false;
		}

		if (!fs::create_directory(destinationDirectory)) // TODO: Only create if doesnt exist
		{
			outErrorMessage = "Failed to create project directory";
			return false;
		}
		const std::wstring contentDirectory = destinationDirectory / TEXT("Content");
		if (!fs::create_directory(contentDirectory))
		{
			outErrorMessage = "Failed to create Content directory";
			return false;
		}

		std::vector<fs::path> allSourceFiles;
		for (const auto& entry : fs::recursive_directory_iterator(sourceDirectory))
		{
			if (entry.is_regular_file())
			{
				allSourceFiles.push_back(fs::absolute(entry.path()));
			}
		}

		for (const fs::path& sourceFilename : allSourceFiles)
		{
			const fs::path relativeFilename = sourceFilename.lexically_relative(sourceDirectory);
			bool bSkip = false;
			for (const auto& ignoredDirectory : ignoredDirectories)
			{
				if (Paths::StartsWith(relativeFilename, ignoredDirectory))
				{
					VM_CORE_TRACE("Skipping ignored directory '{0}'. File: '{1}'", String(ignoredDirectory.begin(), ignoredDirectory.end()), sourceFilename.string());
					bSkip = true;
					break;
				}
			}
			if (bSkip) continue;

			fs::path destinationRelativeFilePath = relativeFilename.parent_path();
			for (const std::pair<std::wstring, std::wstring>& renameDirectory : renameDirectories)
			{
				if (Paths::StartsWith(relativeFilename, renameDirectory.first))
				{
					fs::path filename = destinationRelativeFilePath.lexically_relative(renameDirectory.first);
					filename.wstring() == TEXT(".") ? filename = TEXT("") : filename;
					destinationRelativeFilePath = fs::path(renameDirectory.second) / filename;
					VM_CORE_TRACE("Moving file '{0}' to {1}", relativeFilename.string(), destinationRelativeFilePath.make_preferred().string());
				}
			}
			fs::path destFileName = relativeFilename.stem();
			const fs::path fileExtension = relativeFilename.extension();
			for (const std::pair<std::wstring, std::wstring>& renameFile : renameInFiles)
			{
				fs::path savedDestinationFilename = destFileName;
				destFileName = Paths::Replace(destFileName, renameFile.first, renameFile.second);
				if (savedDestinationFilename != destFileName)
				{
					VM_CORE_TRACE("Renaming file '{0}' to '{1}'", savedDestinationFilename.string() + fileExtension.string(), destFileName.string() + fileExtension.string());
				}
			}

			const fs::path destinationFilename = destinationDirectory / destinationRelativeFilePath / destFileName.replace_extension(fileExtension);

			fs::path destinationDir = destinationFilename.parent_path();
			if (!fs::exists(destinationDir)) 
			{
				if (!fs::create_directories(destinationDir)) 
				{
					outErrorMessage = "Failed to create directories";
					VM_CORE_ERROR("Failed to create directories for file '{0}'", destinationFilename.string());
					return false;
				}
			}
			std::error_code ec;
			bool bSuccess = fs::copy_file(sourceFilename, destinationFilename, std::filesystem::copy_options::overwrite_existing, ec);
			if (bSuccess)
			{
				filesToReplaceInside.push_back(destinationFilename);
			}
			else
			{
				if (ec)
				{
					VM_CORE_ERROR("Failed to copy file '{0}' to '{1}'\n Error message: {2}", sourceFilename.string(), destinationFilename.string(), ec.message().c_str());
				}
				else
				{
					VM_CORE_ERROR("Failed to copy file '{0}' to '{1}'", sourceFilename.string(), destinationFilename.string());
				}
				bSuccessfullyGenerated = false;
			}
		}

		for (const fs::path& replaceInFile : filesToReplaceInside)
		{
			std::wstring fileContent;

			{
				std::wifstream fileHandle(replaceInFile);
				if (!fileHandle.is_open())
				{
					VM_CORE_ERROR("Failed to open file for reading: '{0}'", replaceInFile.string());
					VM_CORE_TRACE("Skipping '{0}'", replaceInFile.string());
					bSuccessfullyGenerated = false;
					continue;
				}
				std::wstringstream buffer;
				buffer << fileHandle.rdbuf();
				fileContent = buffer.str();
			}

			for (const std::pair<std::wstring, std::wstring> replaceStr : renameInFiles)
			{
				size_t startPos = 0;
				while ((startPos = fileContent.find(replaceStr.first, startPos)) != std::wstring::npos)
				{
					fileContent.replace(startPos, replaceStr.first.length(), replaceStr.second);
					startPos += replaceStr.second.length();
				}
			}

			{
				std::wofstream fileHandle(replaceInFile);
				if (!fileHandle.is_open())
				{
					VM_CORE_ERROR("Failed to open file for writing: '{0}'", replaceInFile.string());
					VM_CORE_TRACE("Skipping '{0}'", replaceInFile.string());
					bSuccessfullyGenerated = false;
					continue;
				}
				fileHandle << fileContent;
			}

		}
		if (!AssociateProjectWithEngine(projectInfo.ProjectFilename, outErrorMessage))
		{
			VM_CORE_ERROR("Failed to create engine association for project file");
			return false;
		}
		return bSuccessfullyGenerated;
	}

}