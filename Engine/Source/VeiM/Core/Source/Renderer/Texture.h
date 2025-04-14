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

	struct CubeMap
	{
		uint32 Id = 0;
	};

	Texture TextureFromFile(const fs::path& filename);
	CubeMap loadCubemap(const std::vector<fs::path>& texturesFaces);
}