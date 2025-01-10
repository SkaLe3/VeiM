#pragma once
#include <string>

class __declspec(dllexport) IDesktopPlatform
{
public:
	virtual ~IDesktopPlatform() {}

	// TODO: Make file dialogs functions

	virtual bool GetEngineIdentifierFromRootDir(const std::string& rootDir, std::string& outIdentifier) = 0;

};


