#include "Application/CommandLine.h"
#include "Windows.h"

namespace VeiM
{
	bool CommandLine::bIsInitialized = false;
	TCHAR CommandLine::CmdLine[CommandLine::MaxCommandLineSize] = {};

	const TCHAR* CommandLine::Get()
	{
		return CmdLine;
	}

	bool CommandLine::Set(const TCHAR* newCommandLine)
	{
		lstrcpy(CmdLine, newCommandLine);
		bIsInitialized = true;
		return true;

	}

}

