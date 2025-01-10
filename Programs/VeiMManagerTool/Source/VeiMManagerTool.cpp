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
	std::string str1;
	std::string str2;
	DesktopPlatformModule::Get()->GetEngineIdentifierFromRootDir(str1, str2);


	std::filesystem::path baseDir = std::filesystem::current_path();
	std::cout << "Base Directory: " << baseDir << std::endl;
	system("pause");
	return true;
}

bool RegisterCurrentEngineDirectoryWithDialog()
{
	// TODO: Use PlatformMisc for Dialog for confirmation
	// 
	// Register after confirmation
	if (!RegisterCurrentEngineDirectory(false))
	{
		return false;
	}

	// TODO: Use PlatformMics for notify
	return true;
}

bool UpdateFileAssosiations()
{
	// Update
	return true;
}

bool LaunchEditor()
{
	String identifier;
	// Select editor to launch
	String rootDir;
	// Get engine root dir from identifier
	// Launch editor
	return true;
}

bool LaunchEditor(const std::wstring& projectFileName, const std::wstring& arguments)
{
	String rootDit;

	String editorFileName;
	// Launch editor

	return true;
}

bool GenerateProjectFiles(const std::wstring& projectFileName)
{
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
		// TODO: Dialog
	}
	return bResult ? 0 : 1;
}

#if 0 

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

