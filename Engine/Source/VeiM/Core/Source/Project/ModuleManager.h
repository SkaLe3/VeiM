#pragma once
#include "CoreDefines.h"

#include <unordered_map>

#if IS_UNIFIED
#define IMPLEMENT_MAIN_GAME_MODULE(Name) static VeiM::String s_AppName = #Name; static VeiM::String s_AppTitle = #Name;
#define VM_GAME_MODULE_IMPLEMENTATION <Application/EntryPoint.h>
#else
#define IMPLEMENT_MAIN_GAME_MODULE(Name)
#define VM_GAME_MODULE_IMPLEMENTATION "CoreDefines.h"
#endif

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
}



	// Remove Module interface functions from DesktopPlatform. Make it just as plain singleton