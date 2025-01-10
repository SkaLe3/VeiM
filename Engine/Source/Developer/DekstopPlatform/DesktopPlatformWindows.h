#pragma once
#include "IDesktopPlatform.h"

class __declspec(dllexport) DesktopPlatformWindows : public IDesktopPlatform
{
public:
	DesktopPlatformWindows();

	virtual bool GetEngineIdentifierFromRootDir(const std::string& rootDir, std::string& outIdentifier) override;
};

typedef DesktopPlatformWindows DesktopPlatform;