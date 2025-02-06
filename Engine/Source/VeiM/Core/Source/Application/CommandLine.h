#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	struct CommandLine
	{
		static constexpr inline uint32 MaxCommandLineSize = 16384;
		static const TCHAR* Get();
		static bool Set(const TCHAR* newCommandLine);

	private:
		static bool bIsInitialized;
		static TCHAR CmdLine[MaxCommandLineSize];

	};
}