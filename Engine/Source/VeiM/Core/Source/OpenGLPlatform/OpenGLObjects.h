#pragma once
#include "CoreDefines.h"
#include "RenderInterface/RIObjects.h"

#include <glm/glm.hpp>
#include <Glad/glad.h>

namespace VeiM
{

	struct OpenGLTextureSpecs
	{
		OpenGLTextureSpecs(const TextureRISpecs& specs);

		GLenum Target = GL_NONE;
		GLenum Attachment = GL_NONE;

		uint8 bIsPowerOfTwo : 1;
		uint8 bIsCubeMap : 1;
		uint8 bArray : 1;
		uint8 bDepthStencil : 1;

	};

	struct OpenGLTextureCreateSpecs : public TextureRISpecs, public OpenGLTextureSpecs
	{
		OpenGLTextureCreateSpecs(const TextureRISpecs& specs)
			: TextureRISpecs(specs)
			, OpenGLTextureSpecs(specs)
		{

		}
	};

	class CORE_API OpenGLTexture : public TextureRI
	{
		OpenGLTexture(const OpenGLTexture&) = delete;
		OpenGLTexture& operator=(const OpenGLTexture&) = delete;
	public:
		explicit OpenGLTexture(const OpenGLTextureCreateSpecs& specs);
		virtual ~OpenGLTexture();

		const GLuint GetRendererID() const { return m_RendererID; }

	public:
		/* Texture type GL_TEXTURE_2D, etc. */
		const GLenum const Target = 0; 
		const GLenum const Attachment = 0;
	private:
		GLuint m_RendererID = GL_NONE;

	public:
		const uint8 bIsPowerOfTwo : 1;
		const uint8 bIsCubeMap : 1;
		const uint8 bArray : 1;
		const uint8 bDepthStencil : 1;
	};


	class OpenGLRenderInterface;
	struct OpenGLContext;

	class OpenGLViewport : public ViewportRI
	{
	public:
		OpenGLViewport(OpenGLRenderInterface* ri, uint32 width, uint32 height, bool bFullscreen, EPixelFormat pformat);

		void Resize(uint32 width, uint32 height, bool bFullscreen);

		glm::ivec2 GetSizeXY() const { return glm::ivec2{ m_Width, m_Height }; }
		virtual void* GetNativeWindow() const override;
		OpenGLContext* GetContext() const;
		OpenGLRenderInterface* GetOpenGLRI() const { return m_RI; }
		const char* GetName() { return m_Name; }
	public:
		mutable bool Dirty;
	private:
		OpenGLRenderInterface* m_RI;
		OpenGLContext* m_Context;
		uint32 m_Width;
		uint32 m_Height;
		bool m_bFullscreen;
		EPixelFormat m_PixelFormat;

		SharedPtr<OpenGLTexture> m_Backbuffer;
		const char* m_Name;

		friend class OpenGLRenderInterface;
	};
}