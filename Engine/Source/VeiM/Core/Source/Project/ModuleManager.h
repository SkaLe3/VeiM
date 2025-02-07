#pragma once
#include "CoreDefines.h"

#include <unordered_map>

namespace VeiM
{
	// Used to load game dll for editor
	class CORE_API ModuleManager
	{
	public:
		~ModuleManager();
		static ModuleManager& Get();

		void LoadModule(const std::wstring moduleName);
		void UnloadModule(const std::wstring moduleName);
		void SetGameBianariesDir(const fs::path& gameDir);

	private:
		ModuleManager() = default;
		ModuleManager(const ModuleManager&) = delete;
		ModuleManager& operator=(const ModuleManager&) = delete;
	private:
		std::unordered_map<std::wstring, void*> m_Modules;
	};
	// Remove Module interface functions from DesktopPlatform. Make it just as plain singleton
}