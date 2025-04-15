#pragma once
#include "CoreDefines.h"

namespace VeiM
{
	struct FramebufferSpecs
	{
		uint32 Width = 0;
		uint32 Height = 0;
		uint32 Samples = 1;

		bool bSRGB = false;
		bool bSwapShainTarget = false;
	};


	class CORE_API FrameBuffer
	{
	public:
		FrameBuffer(const FramebufferSpecs& specs);

		void Invalidate();
		void Resize(uint32 width, uint32 height);
		void Bind();
		void UnBind();
		uint32 GetTexture();


		uint32 Handle = 0;
		FramebufferSpecs Specs;
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