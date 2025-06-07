#include "RIObjects.h"

namespace VeiM
{

	TextureRI::TextureRI(const TextureRISpecs& specs)
		: m_Specs(specs)
	{

	}

	bool TextureRISpecs::HasFlag(ETexCreateFlags flag) const
	{
		return ((uint32)Flags & (uint32)flag) != 0;
	}

}

