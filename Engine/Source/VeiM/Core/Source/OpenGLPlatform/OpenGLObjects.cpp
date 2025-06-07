#include "OpenGLObjects.h"
#include "OpenGLPlatform/OpenGLInterface.h"

#include "Utils/Math.h"

#include <GLFW/glfw3.h>

namespace VeiM
{

	OpenGLViewport::OpenGLViewport(OpenGLRenderInterface* ri, uint32 width, uint32 height, bool bFullscreen, EPixelFormat pformat)
		: m_RI(ri)
		, m_Width(width)
		, m_Height(height)
		, Dirty(true)
		, m_bFullscreen(false)
		, m_PixelFormat(pformat)
	{
		m_RI->m_Viewport = this;
		m_Context = CreateOpenGLContext(this);
		Resize(width, height, bFullscreen);
	}

	void OpenGLViewport::Resize(uint32 width, uint32 height, bool bFullScreen)
	{
		if (width == m_Width || height == m_Height)
		{
			return;
		}
		if (width == 0 || height == 0)
		{
			VM_CORE_WARN("Attemted to resize viewport to {}, {}", width, height);
			return;
		}
		m_Width = width;
		m_Height = height;
		bool bWasFullscreen = m_bFullscreen;
		m_bFullscreen = bFullScreen;

		const TextureRISpecs specs = TextureRISpecs::Create2D(width, height, EPixelFormat::RGBA8)
			.SetClarValue({ 0.0f, 0.0f, 0.0f, 0.0f })
			.SetFlags(ETexCreateFlags::Render | ETexCreateFlags::Resolve);
		m_Backbuffer = MakeShared<OpenGLTexture>(specs);

		ResizeOpenGLContext(*this, m_Context, width, height, bFullScreen, bWasFullscreen, m_Backbuffer->Target, m_Backbuffer->GetRendererID());
	}

	void* OpenGLViewport::GetNativeWindow() const
	{
		return (void*)&m_Context->WindowHandle;
	}

	OpenGLContext* OpenGLViewport::GetContext() const
	{
		return m_Context;
	}



	OpenGLTextureSpecs::OpenGLTextureSpecs(const TextureRISpecs& specs)
		: bIsCubeMap(specs.IsCube())
		, bArray(specs.IsArray())
		, bDepthStencil(specs.HasFlag(ETexCreateFlags::DepthStencil))
		, bIsPowerOfTwo(false)
	{
		if (specs.HasFlag(ETexCreateFlags::Present))
		{
			Target = GL_RENDERBUFFER;
		}
		else
		{
			switch (specs.TextureType)
			{
			case ETextureType::Texture2D: Target = (specs.NumSamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; break;
			case ETextureType::Texture2DArray: Target = (specs.NumSamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D; break;
			case ETextureType::TextureCubeArray: Target = GL_TEXTURE_CUBE_MAP_ARRAY; break;
			case ETextureType::TextureCube: Target = GL_TEXTURE_CUBE_MAP; break;
			}
		}
		bIsPowerOfTwo = Math::Utils::IsPowerOfTwo(specs.Size.x) && Math::Utils::IsPowerOfTwo(specs.Size.y) && Math::Utils::IsPowerOfTwo(specs.Depth);

		if (specs.HasFlag(ETexCreateFlags::Render))
		{
			Attachment = GL_COLOR_ATTACHMENT0;
		}
		else if (specs.HasFlag(ETexCreateFlags::DepthStencil))
		{
			Attachment = (specs.Format == EPixelFormat::DEPTH24STENCIL8) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
		}
		else if (specs.HasFlag(ETexCreateFlags::Resolve))
		{
			Attachment = (specs.Format == EPixelFormat::DEPTH24STENCIL8) ? GL_DEPTH_STENCIL_ATTACHMENT
				: ((specs.Format == EPixelFormat::DEPTH24) ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0);
		}
		else
		{
			Attachment = GL_NONE;
		}
	}

	OpenGLTexture::~OpenGLTexture()
	{
		// IMPLEMENT
	}

}

