#pragma once
#include "CoreDefines.h"
#include "Types/PixelFormat.h"
#include "RenderInterface/Types.h"

#include <glm/glm.hpp>

namespace VeiM
{
	struct CORE_API TextureRISpecs
	{
		TextureRISpecs(ETextureType ttype) : TextureType(ttype) {}

		static TextureRISpecs Create2D()
		{
			return TextureRISpecs(ETextureType::Texture2D);
		}
		static TextureRISpecs Create2D(int32 width, int32 height, EPixelFormat format)
		{
			return Create2D().SetSize(width, height).SetFormat(format);
		}

		TextureRISpecs SetFlags(ETexCreateFlags flags) { Flags = flags; return *this; }
		TextureRISpecs SetSize(int32 width, int32 height) { Size = { width ,height }; return *this; }
		TextureRISpecs SetFormat(EPixelFormat format) { Format = format; return *this; }
		TextureRISpecs SetClarValue(const glm::vec4& value) { ClearValue = value; return *this; }

		bool HasFlag(ETexCreateFlags flag) const;
		bool IsCube() const { return TextureType == ETextureType::TextureCube || TextureType == ETextureType::TextureCubeArray; }
		bool IsArray() const { return TextureType == ETextureType::Texture2DArray || TextureType == ETextureType::TextureCubeArray; }

		ETexCreateFlags Flags = ETexCreateFlags::None;
		glm::vec4 ClearValue = glm::vec4{ 0.f, 0.f, 0.f, 0.f };
		glm::ivec2 Size = glm::ivec2(1, 1);
		uint32 Depth = 1;
		ETextureType TextureType = ETextureType::Texture2D;
		EPixelFormat Format = EPixelFormat::None;
		uint8 NumSamples = 1;
	};

	class CORE_API TextureRI
	{
	protected:
		TextureRI(const TextureRISpecs& specs);
		const TextureRISpecs& GetSpecs() const { return m_Specs; }
	private:
		TextureRISpecs m_Specs;
	};


	class ViewportRI
	{
	public:

		virtual void* GetNativeWindow() const { return nullptr; }
	};
}