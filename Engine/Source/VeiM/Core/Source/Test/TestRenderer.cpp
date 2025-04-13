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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_CULL_FACE); // TODO: Set in render command to be able to choose between 1 or 2 sided material in material properties
		glCullFace(GL_BACK);
		// Enable texture 2d

	}

	CORE_API void TestRenderer::RenderMesh(IMesh* mesh, Shader& shader)
	{

		if (mesh->Tdiffuse.Id != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			shader.SetUniformInt("u_Material.diffuse", 0);
			glBindTexture(GL_TEXTURE_2D, mesh->Tdiffuse.Id);
		}
		if (mesh->Tspecualr.Id != 0)
		{
			glActiveTexture(GL_TEXTURE1);
			shader.SetUniformInt("u_Material.specular", 1);
			glBindTexture(GL_TEXTURE_2D, mesh->Tspecualr.Id);
		}

		glBindVertexArray(mesh->GetVAO());
		if (mesh->Indices.size() > 0)
		{
			glDrawElements(mesh->Topology == ETopology::TriangleStrip ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(mesh->Topology == ETopology::TriangleStrip ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, mesh->Positions.size());
		}
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

	CORE_API void TestRenderer::SetClearColor(const float x, const float y, const float z, const float a)
	{
		glClearColor(x, y, z, a);
	}

	void TestRenderer::Clear()
	{
		// TODO: Put in RHI

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void TestRenderer::BlitFramebufferToSwapchain(FrameBuffer& framebuffer)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.Handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, framebuffer.swidth, framebuffer.sheight, 0, 0, framebuffer.swidth, framebuffer.sheight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}



}