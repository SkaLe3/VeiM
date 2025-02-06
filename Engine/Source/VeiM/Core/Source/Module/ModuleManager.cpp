#include "ModuleManager.h"
#include <windows.h>

namespace VeiM
{

	ModuleManager& GetModuleManager()
	{
		static ModuleManager singleton;
		return singleton;
	}

	VeiM::ModuleManager& ModuleManager::Get()
	{
		return GetModuleManager();
	}

	void ModuleManager::LoadModule(const std::wstring moduleName, const fs::path& projectPath)
	{
		std::wstring libraryName = (moduleName + TEXT("-Win64-Debug") + TEXT(".dll"));
		fs::path libraryPath = projectPath / "Binaries" / "Win64" / libraryName;
		HMODULE hDll = LoadLibraryW(libraryPath.wstring().c_str());
		if (!hDll)
			VM_CORE_ERROR("Failed to load {0} module", String(moduleName.begin(), moduleName.end()));

		if (!LoadedModule)
		{
			LoadedModule = new GameModule;
		}
		LoadedModule->moduleFunction = (ModuleFunction)GetProcAddress(hDll, "gameLog");

		if (!LoadedModule->moduleFunction)
		{
			VM_CORE_WARN("Failed to find function gameLog: {0}", GetLastError());
			FreeLibrary(hDll);
		}
	}

}

