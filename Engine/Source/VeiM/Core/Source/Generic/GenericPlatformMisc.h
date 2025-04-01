#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	struct GenericPlatformMisc
	{
	public:
		static fs::path LaunchDir();
		static fs::path EngineDir();
		static fs::path RootDir();
		static fs::path ProjectDir();



		static void MakeEngineDir(fs::path& outEngineDir);
	private:
		struct StaticData;
	};
}