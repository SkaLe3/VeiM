#include "EditorMisc.h"
#include "HAL/PlatformService.h"
#include "Application/Application.h"

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
		const std::wstring cmd = projectName;

		const std::wstring exeFilename = PlatformService::ExecutablePath();
		bool bSuccess = PlatformService::CreateProc(exeFilename.data(), cmd.data(), nullptr, NULL);
		return bSuccess;
	}

}

