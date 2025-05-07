#pragma once
#include "CoreDefines.h"
#include "Test/FrameBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"

namespace VeiM
{
	class  TestRenderer
	{
	public:
		struct RSettings
		{
			uint8 bMainFramebufferDirty : 1 = 0;
			uint8 bPostProcessFramebufferDirty : 1 = 0;
			uint8 bHDREnabled : 1 = 0;
			uint8 bAntiAliasingQuality : 3 = 2;

			float GammaCorrection = 2.2f;
			float Exposure = 1.0f;

			// TODO: Add ViewMode:
			// Wireframe
			// Unlit
			// Lit
			// Light Only
			// Light Complexity
		};
		inline static RSettings Settings;

		CORE_API static void Init();

		CORE_API static void RenderMesh(IMesh* mesh, Shader& shader, uint32 shadowmapTexture = 0, uint32 shadowcubemapTexture = 0, bool noTextures = false);
		CORE_API static void SetClearColor(const float x, const float y, const float z, const float a);
		CORE_API static void Clear();
		CORE_API static void BlitFramebufferToSwapchain(FrameBuffer& framebuffer);
		CORE_API static void BlitFramebufferTo(FrameBuffer& sourceFB, FrameBuffer& targetFB);
		CORE_API static void BlitStencil(FrameBuffer& framebufferSource, FrameBuffer& framebufferDestination);

		CORE_API static void UpdatePostprocessShader(Shader& shader);

	};
}