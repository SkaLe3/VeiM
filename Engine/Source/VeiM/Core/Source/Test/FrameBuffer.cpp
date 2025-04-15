#include "Test/FrameBuffer.h"

#include <glad/glad.h>

namespace VeiM
{
	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}
	static void CreateTexture(bool multisampled, uint32* outID)
	{
		glCreateTextures(TextureTarget(multisampled), 1, outID);
	}

	VeiM::FrameBuffer::FrameBuffer(const FramebufferSpecs& specs) : Specs(specs)
	{
		Invalidate();
	}

	void VeiM::FrameBuffer::Invalidate()
	{
		if (Handle)
		{
			glDeleteFramebuffers(1, &Handle);
			glDeleteTextures(1, &textureColorBuffer);
			glDeleteRenderbuffers(1, &rbo);
			Handle = 0;
			textureColorBuffer = 0;
			rbo = 0;
		}
		glCreateFramebuffers(1, &Handle);

		bool multisample = Specs.Samples > 1;

		// Color texture
		CreateTexture(multisample, &textureColorBuffer);
		if (multisample)
		{
			glTextureStorage2DMultisample(textureColorBuffer, Specs.Samples, GL_RGB8, Specs.Width, Specs.Height, GL_TRUE);
		}
		else
		{
			glTextureStorage2D(textureColorBuffer, 1, GL_RGB8, Specs.Width, Specs.Height);
			glTextureParameteri(textureColorBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(textureColorBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(textureColorBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(textureColorBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}	

		// Attach
		glNamedFramebufferTexture(Handle, GL_COLOR_ATTACHMENT0, textureColorBuffer, 0);

		// Renderbuffer for depth+stencil
		glCreateRenderbuffers(1, &rbo);
		if (multisample)
			glNamedRenderbufferStorageMultisample(rbo, Specs.Samples, GL_DEPTH24_STENCIL8, Specs.Width, Specs.Height);
		else
			glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, Specs.Width, Specs.Height);

		// Attach
		glNamedFramebufferRenderbuffer(Handle, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckNamedFramebufferStatus(Handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			VM_CORE_ERROR(" Framebuffer is not complete!");
	}

	void VeiM::FrameBuffer::Resize(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0)
		{
			VM_CORE_WARN("Attempt to resize framebuffer to {0}, {1}", width, height);
			return;
		}
		Specs.Width = width;
		Specs.Height = height;
		Invalidate();
	}

	void VeiM::FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Handle);
		glViewport(0, 0, Specs.Width, Specs.Height);
	}

	void VeiM::FrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	VeiM::uint32 VeiM::FrameBuffer::GetTexture()
	{
		return textureColorBuffer;
	}

	VeiM::uint32 VeiM::ShaderStatics::CompileShader(uint32 type, const char* source)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			VM_CORE_ERROR("Shader Compilation Failed {0}", infoLog);
		}
		return shader;
	}

	VeiM::uint32 VeiM::ShaderStatics::CreateProgram(const char* vertexSource, const char* fragmentSource)
	{
		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

		GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		int success;
		char infoLog[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, nullptr, infoLog);
			VM_CORE_ERROR("Program Linking Failed {0}", infoLog);
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}

}