#pragma once
#include "CoreDefines.h"
#include "Types/StringID.h"

namespace VeiM
{

	enum class ETextureColorSpace
	{
		Linear,
		sRGB
	};

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

	Texture TextureFromFile(const fs::path& filename, ETextureColorSpace colorSpace);
	Texture GenerateTexture(uint8* data, uint32 width, uint32 height);
	CubeMap loadCubemap(const std::vector<fs::path>& texturesFaces);
}