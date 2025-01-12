#include "VeimManagerTool.h"
#include "DekstopPlatform/DesktopPlatformModule.h"


#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <filesystem>
#include <locale>
#include <codecvt>

#include <iostream>
#include <cstdio>

using namespace VeiM;

bool RegisterCurrentEngineDirectory(bool bAskFileAssociation)
{
	std::wstring baseDir = std::filesystem::current_path().wstring();
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
			std::wstring executableFileName = std::wstring(std::filesystem::current_path() / std::wstring(DesktopPlatformModule::Get()->ExecutableName(false)));

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

bool LaunchEditor()
{
	MessageBoxW(NULL, TEXT("Launching Editor project selection"), TEXT("Launching Editor"), MB_OK);
	String identifier;
	// Select editor to launch
	String rootDir;
	// Get engine root dir from identifier
	// Launch editor
	return true;
}

bool LaunchEditor(const std::wstring& projectFileName, const std::wstring& arguments)
{
	std::wstring msg = std::wstring(L"Launching ") + projectFileName + L" in Editor";
	MessageBoxW(NULL, msg.c_str(), TEXT("Launching Game"), MB_OK);
	String rootDit;

	String editorFileName;
	// Launch editor

	return true;
}

bool GenerateProjectFiles(const std::wstring& projectFileName)
{
	MessageBoxW(NULL, TEXT("Generating project files"), TEXT("Project Files"), MB_OK);

	

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

