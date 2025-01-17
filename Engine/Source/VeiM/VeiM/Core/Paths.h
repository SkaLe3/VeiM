#pragma once

#include <string>

namespace VeiM
{
	class Paths
	{
	public:
		static std::wstring GetPath(std::wstring& inPath); // TODO: Change wstring to String
		static std::wstring EngineContentDir();
	};
}