#pragma once
#include <VeiM/Core/CoreDefines.h>

namespace VeiM
{
	class FrameBuffer
	{
	public:
		void Invalidate(uint32 widht, uint32 height);
		void Bind();
		void UnBind();
		uint32 GetTexture();

	private:
		uint32 framebuffer = 0;
		uint32 textureColorBuffer = 0;
		uint32 rbo = 0;

		uint32 swidth, sheight;
	};

	class ShaderStatics
	{
	public:
		static uint32 CompileShader(uint32 type, const char* source);
		static uint32 CreateProgram(const char* vertexSource, const char* fragmentSource);
	};
}