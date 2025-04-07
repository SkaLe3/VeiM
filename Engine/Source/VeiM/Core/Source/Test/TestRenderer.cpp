#include "TestRenderer.h"
#include "Application/Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace VeiM
{

	void TestRenderer::Init()
	{
		// enable gl debug output
		// set opengl message callback
		// enable blend
		// set blend func

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		// Enabgle texture 2d

	}

	CORE_API void TestRenderer::RenderMesh(IMesh* mesh, Shader& shader, Texture& tex, const glm::mat4& viewProj, const glm::mat4& view, const glm::mat4& transform)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex.GetTexID());

		shader.Bind();
		shader.SetMat4("u_ViewProjection", viewProj);
		shader.SetMat4("u_View", view);
		shader.SetMat4("u_Transform", transform);
		shader.SetFloat4("tintColor", { 1.0f, 1.0f, 1.0f, 1.0f });
		mesh->Render();
	}


	CORE_API void TestRenderer::SetClearColor(const float x, const float y, const float z, const float a)
	{
		glClearColor(x, y, z, a);
	}

	void TestRenderer::Clear()
	{
		// TODO: Put in RHI
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void TestRenderer::BlitFramebufferToSwapchain(FrameBuffer& framebuffer)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.Handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, framebuffer.swidth, framebuffer.sheight, 0, 0, framebuffer.swidth, framebuffer.sheight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}



}