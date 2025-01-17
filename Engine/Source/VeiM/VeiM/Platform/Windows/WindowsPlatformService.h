#pragma once

#include "VeiM/Core/GenericPlatformService.h"

namespace VeiM
{
	struct WindowsPlatformService : public GenericPlatformService
	{
	public:
		static const TCHAR* BaseDir();
		static void SetCurrentWorkingDirectoryToBaseDir();
	};

	typedef WindowsPlatformService PlatformService;
}