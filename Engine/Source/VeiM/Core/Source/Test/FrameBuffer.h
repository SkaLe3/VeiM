#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	class CORE_API FrameBuffer
	{
	public:
		void Invalidate(uint32 width, uint32 height);
		void Bind();
		void UnBind();
		uint32 GetTexture();


		uint32 Handle = 0;
		uint32 swidth, sheight;
	private:
		uint32 textureColorBuffer = 0;
		uint32 rbo = 0;

	};

	class CORE_API ShaderStatics
	{
	public:
		static uint32 CompileShader(uint32 type, const char* source);
		static uint32 CreateProgram(const char* vertexSource, const char* fragmentSource);
	};
}