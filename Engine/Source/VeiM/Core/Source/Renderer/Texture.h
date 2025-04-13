#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"

namespace VeiM
{

	struct Texture
	{
		uint32 Id = 0;
		StringID Type;
		fs::path Path;
	};

	Texture TextureFromFile(const fs::path& filename);
}