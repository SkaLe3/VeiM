#include "WindowsUtils.h"
#include <GLFW/glfw3.h>
#include <windows.h>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <sstream>

namespace VeiM
{
	float Time::GetTime()
	{
		return glfwGetTime();
	}

	bool FileDialogs::OpenFileDialog(const void* parentWindowHandle, const String& dialogTitle, const String& defaultPath, const String& defaultFile, const String& fileTypes, String& outFilename)
	{
		// TODO: Add flags with None and Multiple to select multiple files
		bool bSuccess = false;

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)glfwGetWin32Window((GLFWwindow*)parentWindowHandle);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		std::string combined;

		if (fileTypes.empty())
		{
			ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
		}
		else
		{
			std::vector<std::string> values;
			std::stringstream ss(fileTypes);
			std::string item;

			while (std::getline(ss, item, '|'))
			{
				values.push_back(item);
			}

			combined = values[0] + " (" + values[1] + ")" + '\0' + values[1] + '\0';
			ofn.lpstrFilter = combined.data();
		}

		ofn.lpstrFileTitle = nullptr;
		ofn.nFilterIndex = 1;
		ofn.nMaxFileTitle = 0;
		if (defaultPath.empty())
		{
			if (GetCurrentDirectoryA(256, currentDir))
				ofn.lpstrInitialDir = currentDir;
			else
				ofn.lpstrInitialDir = nullptr;
		}
		else
		{
			ofn.lpstrInitialDir = defaultPath.c_str();
		}
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		ofn.lpstrTitle = dialogTitle.empty() ? nullptr : dialogTitle.c_str();

		if (!defaultFile.empty())
		{
			strncpy_s(szFile, defaultFile.c_str(), sizeof(szFile));
		}

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			outFilename = ofn.lpstrFile;
			return true;
		}

		DWORD error = CommDlgExtendedError();
		if (error == 0)
		{
			// User canceled the dialog
			outFilename.clear();
		}
		else
		{
			// An error occurred
			outFilename.clear();
		}

		return false;

	}

}

