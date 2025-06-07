#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	enum class ETextureType : uint8
	{
		Texture2D,
		Texture2DArray,
		TextureCube,
		TextureCubeArray
	};


	enum class ETexCreateFlags : uint32
	{
		None = 0,
		Render = 1 << 0,
		Resolve = 1 << 1,
		DepthStencil = 1 << 2,
		Present = 1 << 3

	};
	inline constexpr ETexCreateFlags  operator| (ETexCreateFlags  lhs, ETexCreateFlags rhs) { return (ETexCreateFlags)((uint32)lhs | (uint32)rhs); } 
}