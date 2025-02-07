#include "ModuleManager.h"
#include "Application/Application.h"
#include "HAL/PlatformService.h"
#include <windows.h>

namespace VeiM
{

	ModuleManager::~ModuleManager()
	{
		std::vector<std::wstring> moduleNames;
		for (const auto& [name, handle] : m_Modules)
		{
			moduleNames.push_back(name);
		}
		for (const auto& name : moduleNames)
		{
			UnloadModule(name);
		}
	}

	ModuleManager& ModuleManager::Get()
	{
		static ModuleManager singleton;
		return singleton;
	}

	void ModuleManager::LoadModule(const std::wstring moduleName)
	{
		String moduleNameA = String(moduleName.begin(), moduleName.end());
		if (m_Modules.find(moduleName) != m_Modules.end())
		{
			VM_CORE_WARN("Attempt to load module. Module {0} is already loaded", moduleNameA);
			return;
		}

		std::wstring librarySufix = TEXT("");
		std::wstring platformName = TEXT("Win64");
		std::wstring configurationName = Application::Get().GetConfiguration();
		if (configurationName != TEXT("Development"))
		{
			librarySufix = TEXT("-") + platformName + TEXT("-") + configurationName;
		}
		std::wstring libraryName = (moduleName + librarySufix + TEXT(".dll"));

		HMODULE moduleHandle = LoadLibraryW(libraryName.c_str());
		if (!moduleHandle)
		{
			VM_CORE_ERROR("Failed to load {0} module", moduleNameA);
			return;
		}

		m_Modules[moduleName] = moduleHandle;
		VM_CORE_INFO("Module {0} loaded successfully",moduleNameA);
	}

	void ModuleManager::UnloadModule(const std::wstring moduleName)
	{
		String moduleNameA = String(moduleName.begin(), moduleName.end());
		auto it = m_Modules.find(moduleName);
		if (it == m_Modules.end())
		{
			VM_CORE_WARN("Attempt to unload module. Module {0} is not loaded", moduleNameA);
			return;
		}
		if (!FreeLibrary((HMODULE)it->second))
		{
			VM_CORE_WARN("Failed to unload module {0}. Reason: {1}", moduleNameA, (uint64)GetLastError());
			return;
		}
		m_Modules.erase(it);
		VM_CORE_INFO("Module {0} unloaded successfullly", moduleNameA);
	}

	void ModuleManager::SetGameBianariesDir(const fs::path& gameDir)
	{
		PlatformService::AddDllDirPath(gameDir.wstring().c_str());
	}


}

