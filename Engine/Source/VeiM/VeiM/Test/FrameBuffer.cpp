#include "FrameBuffer.h"

#include <glad/glad.h>



void VeiM::FrameBuffer::Invalidate(uint32 widht, uint32 height)
{
	swidth = widht;
	sheight = height;
	if (framebuffer)
	{
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &textureColorBuffer);
		glDeleteRenderbuffers(1, &rbo);
		framebuffer = 0;
		textureColorBuffer = 0;
		rbo = 0;
	}
	glCreateFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);



	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);



	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		VM_CORE_ERROR(" Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
}

void VeiM::FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, swidth, sheight);
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
