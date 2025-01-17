#include "VeimManagerTool.h"
#include "DesktopPlatform/DesktopPlatformModule.h"
#include "DesktopPlatform/Paths.h"
#include "PlatformInstallation.h"


#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <filesystem>

#include <cstdio>

#include <cstdlib> // For running premake

#include <locale> // For conversion to run premake
#include <codecvt>

#include <yaml-cpp/yaml.h>

using namespace VeiM;


std::string WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}


std::wstring StringToWString(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(str);
}

bool GetEngineRootDirForProject(const std::wstring& projectFileName, std::wstring& outRootDir)
{
	std::wstring identifier;
	return  DesktopPlatformModule::Get()->GetEngineIdentifierForProject(projectFileName, identifier) && DesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(identifier, outRootDir);
}

bool RegisterCurrentEngineDirectory(bool bAskFileAssociation)
{
	std::wstring baseDir = Paths::GetPath(DesktopPlatformModule::Get()->BaseDir());
	if (!DesktopPlatformModule::Get()->NormalizeEngineRootDir(baseDir))
	{
		MessageBoxW(NULL, TEXT("The current folder does not contain an engine installation."), TEXT("Error"), MB_OK);
		return false;
	}

	std::wstring identifier;
	if (!DesktopPlatformModule::Get()->GetEngineIdentifierFromRootDir(baseDir, identifier))
	{
		MessageBoxW(NULL, TEXT("Couldn't add engine installation."), TEXT("Error"), MB_OK);
		return false;
	}

	if (!DesktopPlatformModule::Get()->VerifyFileAssociations())
	{
		if (!bAskFileAssociation || MessageBoxW(NULL, TEXT("Register VeiM Engine file types?"), TEXT("File Types"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			std::wstring executableFileName = std::wstring(std::filesystem::path(Paths::GetPath(DesktopPlatformModule::Get()->BaseDir())) / std::wstring(DesktopPlatformModule::Get()->ExecutableName(false)));

			VeiM::int32 exitCode;
			if (!DesktopPlatformModule::Get()->ExecElevatedProcess(executableFileName.c_str(), TEXT("/fileassociations"), &exitCode) || exitCode != 0)
			{
				return false;
			}
		}
	}

	return true;
}

bool RegisterCurrentEngineDirectoryWithDialog()
{
	if (MessageBoxW(NULL, TEXT("Register this directory as an VeiM Engine installation?"), TEXT("Question"), MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		return false;
	}

	if (!RegisterCurrentEngineDirectory(false))
	{
		return false;
	}

	MessageBoxW(NULL, TEXT("Registration successful."), TEXT("Success"), MB_OK);
	return true;
}

bool UpdateFileAssosiations()
{
	if (!DesktopPlatformModule::Get()->UpdateFileAssociations())
	{
		MessageBoxW(NULL, TEXT("Couldn't update file associations."), TEXT("Error"), MB_OK);
		return false;
	}
	return true;
}

bool ReadLaunchPathFromTargetFile(const std::wstring& targetFileName, std::wstring& outLaunchPath)
{
	std::string targetFileNameAnsi = WStringToString(targetFileName);

	YAML::Node config = YAML::LoadFile(targetFileNameAnsi);
	if (!config["TargetType"] || StringToWString(config["TargetType"].as<std::string>()) != TEXT("Editor"))
	{
		return false;
	}

	if (!config["Configuration"] || StringToWString(config["Configuration"].as<std::string>()) != TEXT("Development"))
	{
		return false;
	}

	if (!config["Launch"])
	{
		return false;
	}
	outLaunchPath.clear();
	outLaunchPath = StringToWString(config["Launch"].as<std::string>());
	return 	true;
}

void ReplaceSubstring(std::wstring& originString, const std::wstring& oldSubstring, const std::wstring& newSubstring)
{
	size_t pos = originString.find(oldSubstring);
	while (pos != std::wstring::npos)
	{
		originString.replace(pos, oldSubstring.length(), newSubstring);
		pos = originString.find(oldSubstring, pos + newSubstring.length());
	}
}

bool TryGetEditorFileName(const std::wstring& engineDir, const std::wstring& projectFileName, std::wstring& outEditorFileName)
{
	std::wstring projectDir = Paths::GetPath(projectFileName);
	std::wstring binariesDir = (std::filesystem::path(projectDir) / TEXT("Binaries") / TEXT("Win64")).wstring();
	if (std::filesystem::exists(binariesDir) && std::filesystem::is_directory(binariesDir))
	{
		std::vector<std::pair<std::wstring, std::filesystem::file_time_type>> files;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(binariesDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".target")
			{
				files.push_back({ entry.path().wstring(), std::filesystem::last_write_time(entry) });
			}
		}

		std::sort(files.begin(), files.end(),
				  [](const std::pair<std::wstring, std::filesystem::file_time_type>& a,
					 const std::pair<std::wstring, std::filesystem::file_time_type>& b)
				  {
					  return a.second > b.second;
				  });
		for (const std::pair<std::wstring, std::filesystem::file_time_type>& record : files)
		{
			std::wstring launchPath;
			if (ReadLaunchPathFromTargetFile(record.first, launchPath))
			{
				outEditorFileName = std::move(launchPath);
				ReplaceSubstring(outEditorFileName, TEXT("$(EngineDir)"), engineDir.c_str());
				ReplaceSubstring(outEditorFileName, TEXT("$(ProjectDir)"), projectDir.c_str());
				return true;
			}
		}
	}
	return true;
}

bool LaunchEditor()
{
	std::wstring identifier;

	if (!PlatformInstallation::SelectEngineInstallation(identifier))
	{
		return false;
	}

	std::wstring rootDir;
	DesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(identifier, rootDir);

	if (!PlatformInstallation::LaunchEditor(rootDir, std::wstring(), std::wstring()))
	{
		MessageBoxW(NULL, TEXT("Failed to launch editor. Probably Editor Development build doesn't exist"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

bool LaunchEditor(const std::wstring& projectFileName, const std::wstring& arguments)
{
	std::wstring rootDir;
	if (!GetEngineRootDirForProject(projectFileName, rootDir))
	{
		return false;
	}

	std::wstring editorFileName;
	TryGetEditorFileName((std::filesystem::path(rootDir) / TEXT("Engine")).wstring(), projectFileName, editorFileName);

	if (!PlatformInstallation::LaunchEditor(rootDir, editorFileName, std::wstring(TEXT("\"")) + projectFileName + TEXT("\" ") + arguments))
	{
		MessageBoxW(NULL, TEXT("Failed to launch editor. Probably the corresponding editor configuration build does not exist"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool GenerateProjectFiles(const std::wstring& projectFileName)
{
	IDesktopPlatform* desktopPlatform = DesktopPlatformModule::Get();

	std::wstring sourceDir = std::filesystem::path(Paths::GetPath(projectFileName)) / TEXT("Source");
	if (!Paths::DirectoryExists(sourceDir))
	{
		MessageBoxW(NULL, TEXT("No C++ source files are currently included in this project. Please add them in the Editor before attempting to generate project files."), TEXT("Error"), MB_OK | MB_ICONWARNING);
		return false;
	}

	std::wstring rootDir;
	if (!GetEngineRootDirForProject(projectFileName, rootDir))
	{
		MessageBoxW(NULL, TEXT("Error retrieving project root directory"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	// TODO: Make premake5.lua regeneration for project
	// those parts that are dependent of paths


	// May be add reading from config Engine.ini file for finding path of premake
	Paths::MakeWindowsFileName(rootDir);
	std::wstring premakeDir = (std::filesystem::path(rootDir) / TEXT("Engine\\Programs\\premake\\premake5.exe")).wstring();
	std::wstring action = TEXT("vs2022");
	std::wstring commandStart = TEXT("echo Generating project files... && echo.");
	std::wstring commandEnd = TEXT("@echo. && pause");
	std::wstring changeDir = std::wstring(L"cd ") + std::filesystem::path(Paths::GetPath(projectFileName)).wstring() + TEXT("\\");

	std::wstring command = commandStart + TEXT(" && ") + changeDir + TEXT(" && ") + TEXT("call ") + premakeDir + TEXT(" ") + action + TEXT(" && ") + commandEnd;
	std::wstring cmdCommand = TEXT("cmd.exe /C \"") + command + TEXT("\"");
	std::wstring cmdCommandFail = TEXT("cmd.exe /K \"") + command + TEXT("\"");


	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string cmdCommandA = converter.to_bytes(cmdCommand);
	std::string cmdCommandFailA = converter.to_bytes(cmdCommandFail);

	// TODO: Consider creating process
	int32 bResult = std::system(cmdCommandA.c_str());
	if (bResult != 0)
	{
		// Run again with /K flag to keep console open 
		std::system(cmdCommandFailA.c_str());
		return false;
	}

	return true;
}


int Main(const std::vector<std::wstring>& arguments)
{
	bool bResult = false;
	if (arguments.size() == 0)
	{
		bResult = RegisterCurrentEngineDirectoryWithDialog();
	}
	else if (arguments.size() == 1 && arguments[0] == TEXT("-register"))
	{
		bResult = RegisterCurrentEngineDirectory(true);
	}
	else if (arguments.size() == 2 && arguments[0] == TEXT("-register") && arguments[1] == TEXT("-unattended"))
	{
		bResult = RegisterCurrentEngineDirectory(false);
	}
	else if (arguments.size() == 1 && arguments[0] == TEXT("-fileassociations"))
	{
		bResult = UpdateFileAssosiations();
	}
	else if (arguments.size() == 2 && arguments[0] == TEXT("-editor"))
	{
		bResult = LaunchEditor(arguments[1], TEXT(""));
	}
	else if (arguments[0] == TEXT("-projectlist"))
	{
		bResult = LaunchEditor();
	}
	else if (arguments.size() == 2 && arguments[0] == TEXT("-game"))
	{
		bResult = LaunchEditor(arguments[1], TEXT("-game"));
	}
	else if (arguments.size() == 2 && arguments[0] == TEXT("-projectfiles"))
	{
		bResult = GenerateProjectFiles(arguments[1]);
	}
	else
	{
		MessageBoxW(NULL, TEXT("Invalid command line"), TEXT("Error"), MB_OK);
	}
	return bResult ? 0 : 1;
}

#if 1

int WINAPI WinMain(HINSTANCE hCurrInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int SHowCmd)
{
	int argC;
	LPWSTR* argV = ::CommandLineToArgvW(GetCommandLine(), &argC);

	std::vector<std::wstring> arguments;
	for (int idx = 1; idx < argC; idx++)
	{
		std::wstring argument = argV[idx];
		if (argument.length() > 0 && argument[0] == '/')
		{
			argument[0] = '-';
		}
		arguments.push_back(argument);
	}

	return Main(arguments);
}
#else
int main(int argC, const char* argV[])
{
	std::vector<std::wstring> arguments;
	for (int idx = 1; idx < argC; idx++)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		arguments.emplace_back(converter.from_bytes(argV[idx]));
	}
	return Main(arguments);
}
#endif

