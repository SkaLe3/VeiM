#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	enum class EPixelFormat : uint8
	{
		None = 0,
		R8,
		R8_SRGB,
		RG8,
		RG8_SRG,
		RGB8,
		RGB8_SRGB,
		RGBA8,
		RGBA8_SRGB,

		R32F,
		RG16F,
		RGB16F,
		R11G11B10F,
		RGB32F,
		RGBA16F,
		RGBA32F,

		DEPTH16,
		DEPTH24STENCIL8,
		DEPTH24,
		DEPTH32F
	};
}