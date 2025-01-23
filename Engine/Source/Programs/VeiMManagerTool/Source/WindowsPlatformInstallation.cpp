#include "WindowsPlatformInstallation.h"
#include "DesktopPlatform/DesktopPlatformModule.h"
#include "DesktopPlatform/Paths.h"
#include "Resources/Resource.h"

#include <filesystem>

using namespace VeiM;


std::wstring GetInstallationDescription(const std::wstring& id, const std::wstring& rootDir)
{
	std::wstring platformRootDir = rootDir;
	Paths::MakeWindowsFileName(platformRootDir);
	return std::wstring(TEXT("Source build at ")) + platformRootDir;
}

struct SelectBuildDialog
{
public:
	SelectBuildDialog(const std::wstring& inIdentifier)
	{
		Identifier = inIdentifier;
		DesktopPlatformModule::Get()->EnumerateEngineInstallations(m_Installations);
		for (const auto& pair : m_Installations) m_SortedIdentifiers.push_back(pair.first);
		std::sort(m_SortedIdentifiers.begin(), m_SortedIdentifiers.end());
		if (m_SortedIdentifiers.size() == 1)
		{
			Identifier = m_SortedIdentifiers[0];
		}
	}

	bool DoModal(HWND hWndParrent)
	{
		if (!m_Installations.empty())
		{
			return DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECTBUILD), hWndParrent, &DialogProc, (LPARAM)this) != FALSE;
		}
		MessageBoxW(NULL, TEXT("No VeiM Engine installations found"), TEXT("Error"), MB_OK);
		return false;
	}

public:
	std::wstring Identifier;

private:
	std::vector<std::wstring> m_SortedIdentifiers;
	std::unordered_map<std::wstring, std::wstring> m_Installations;

	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		SelectBuildDialog* dialog = (SelectBuildDialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		switch (Msg)
		{
		case WM_INITDIALOG:
			dialog = (SelectBuildDialog*)lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)dialog);
			dialog->UpdateInstallations(hWnd);
			return false;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				dialog->StoreSelection(hWnd);
				EndDialog(hWnd, 1);
				return false;
			case IDCANCEL:
				EndDialog(hWnd, 0);
				return false;
			}
		}
		return FALSE;
	}

	// TODO: Add Browse

	void StoreSelection(HWND hWnd)
	{
		int32 idx = SendDlgItemMessage(hWnd, IDC_BUILDLIST, CB_GETCURSEL, 0, 0);
		Identifier = m_SortedIdentifiers.size() > idx && idx >= 0 ? m_SortedIdentifiers[idx] : TEXT("");
	}

	void UpdateInstallations(HWND hWnd)
	{
		SendDlgItemMessage(hWnd, IDC_BUILDLIST, CB_RESETCONTENT, 0, 0);

		for (int32 idx = 0; idx < m_SortedIdentifiers.size(); idx++)
		{
			const std::wstring& sortedIdentifier = m_SortedIdentifiers[idx];
			std::wstring description = GetInstallationDescription(sortedIdentifier, m_Installations[sortedIdentifier]);
			SendDlgItemMessage(hWnd, IDC_BUILDLIST, CB_ADDSTRING, 0, (LPARAM)description.data());
		}
		int32 newIdx = 0;
		auto it = std::find(m_SortedIdentifiers.begin(), m_SortedIdentifiers.end(), Identifier);
		if (it != m_SortedIdentifiers.end())
			newIdx = std::distance(m_SortedIdentifiers.begin(), it);
		SendDlgItemMessage(hWnd, IDC_BUILDLIST, CB_SETCURSEL, newIdx, 0);
	}
};







bool WindowsPlatformInstallation::LaunchEditor(const std::wstring& rootDirName, const std::wstring& explicitFileName, const std::wstring& arguments)
{
	std::wstring fileName;
	std::wstring configuration = TEXT("Developmnet");
	std::wstring exeName = std::wstring(TEXT("VeiMEditor.exe"));
	if (explicitFileName.empty())
	{
		fileName = std::filesystem::path(rootDirName) / TEXT("Engine/Binaries/Win64") / configuration / exeName;
	}
	else
	{
		fileName = explicitFileName;
	}

	std::wstring commandLine = std::wstring(TEXT("\"")) + fileName + TEXT("\" ") + arguments;
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo;
	if (!CreateProcess(NULL, commandLine.data(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
	{
		return false;
	}

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	return true;
}

bool WindowsPlatformInstallation::SelectEngineInstallation(std::wstring& identifier)
{
	SelectBuildDialog dialog(identifier);
	// If only one installation found, just use it
	/*
	if (identifier != dialog.Identifier)
	{
		identifier = dialog.Identifier;
		return true;
	}
	*/
	if (!dialog.DoModal(NULL))
	{
		return false;
	}
	identifier = dialog.Identifier;
	return true;
}

