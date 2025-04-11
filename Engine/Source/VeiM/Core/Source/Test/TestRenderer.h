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
		CORE_API static void Init();

		CORE_API static void RenderMesh(IMesh* mesh, Shader& shader, Texture& tex, const glm::mat4& viewProj, const glm::mat4& transform);
		CORE_API static void SetClearColor(const float x, const float y, const float z, const float a);
		CORE_API static void Clear();
		CORE_API static void BlitFramebufferToSwapchain(FrameBuffer& framebuffer);

	};
}