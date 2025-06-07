#include "EditorMisc.h"
#include "HAL/PlatformService.h"
#include "Application/Application.h"
#include "Misc/Paths.h"
#include "DesktopPlatformModule.h"
#include "EditorLayer.h"

namespace VeiM
{

	EditorMisc& EditorMisc::Get()
	{
		static EditorMisc editorMisc;
		return editorMisc;
	}

	void EditorMisc::OnInit()
	{

	}

	void EditorMisc::OnShutdown()
	{
		if (!m_PendingProjectName.empty())
		{
			bool bSuccess = SpawnEditor(m_PendingProjectName);
			if (!bSuccess)
			{
				VM_CORE_WARN("Couldn't restart editor");
				ClearPendingProjectName();
				return;
			}
		}
	}

	const std::wstring& EditorMisc::GetPendingProjectName() const
	{
		return m_PendingProjectName;
	}

	void EditorMisc::SetPendingProjectName(const std::wstring& projectName)
	{
		m_PendingProjectName = projectName;
	}

	void EditorMisc::ClearPendingProjectName()
	{
		m_PendingProjectName.clear();
		m_PendingCommandLine.clear();
	}

	void EditorMisc::SwitchProject(const std::wstring& projectFilename)
	{
		SetPendingProjectName(projectFilename);
		Application::Get().Close();
	}

	bool EditorMisc::SpawnEditor(const std::wstring& projectName)
	{
		const std::wstring cmd = TEXT("\"") + projectName + TEXT("\"");
		std::wstring rootDir = Paths::GetPath(PlatformService::ExecutablePath());
		DesktopPlatformModule::Get()->NormalizeEngineRootDir(rootDir);
		std::wstring binariesDir = (fs::path(rootDir) / "Engine" / "Binaries" / "Win64" / "Development_Editor").wstring();
		std::wstring editorFilePath = binariesDir + TEXT("\\") + TEXT("VeimEditor.exe");

		ProcessHandle editorProcessHandle = PlatformService::CreateProc(editorFilePath.data(), cmd.data(), nullptr, NULL, nullptr);
		if (editorProcessHandle.IsValid())
		{
			PlatformService::CloseProcess(editorProcessHandle);
			return true;
		}
		return false;
	}
}

