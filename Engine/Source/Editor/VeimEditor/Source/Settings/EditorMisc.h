#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class EditorMisc
	{
	public:
		static EditorMisc& Get();

		void OnInit();

		void OnShutdown();

		const std::wstring& GetPendingProjectName() const;
		void SetPendingProjectName(const std::wstring& projectName);
		void ClearPendingProjectName();

		void SwitchProject(const std::wstring& projectFilename);
		bool SpawnEditor(const std::wstring& projectName);

	private:
		std::wstring m_PendingProjectName;
		std::wstring m_PendingCommandLine;
	};
}