#pragma once
#include "CoreDefines.h"


namespace VeiM
{
	class Texture
	{
	public:
		Texture(const fs::path& filepath);
		uint32 GetTexID() const { return m_TextureID; }

	private:

		uint32 m_TextureID;
	};
}