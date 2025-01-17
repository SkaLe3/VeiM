#include "Paths.h"

namespace VeiM
{

	std::wstring Paths::GetPath(std::wstring& inPath)
	{
		size_t found = inPath.find_last_of(L"/\\");

		std::wstring result;
		if (found != std::wstring::npos)
		{
			result = inPath.substr(0, found);
		}
		return result;
	}

	std::wstring Paths::EngineContentDir()
	{
		return L"";
	}

}

