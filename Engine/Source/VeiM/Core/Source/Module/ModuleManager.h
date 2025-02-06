#pragma once
#include "CoreDefines.h"

#include <unordered_map>

namespace VeiM
{
	class ModuleManager;

	ModuleManager& GetModuleManager();

	typedef void (*ModuleFunction)();

	struct CORE_API GameModule
	{
		ModuleFunction moduleFunction;
	};

	// Used to load game dll for editor
	class CORE_API ModuleManager
	{
	public:
		static ModuleManager& Get();

		void LoadModule(const std::wstring moduleName, const fs::path& projectPath);

		GameModule* LoadedModule = nullptr;
	};

	// Delete ModuleManager. Instead use Project Manager to load both project itself and its module(s)
	// Remove Module interface functions from DesktopPlatform. Make it just as plain singleton
}