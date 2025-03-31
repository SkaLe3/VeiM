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


	void TestRenderer::Clear()
	{
		// TODO: Put in RHI
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void TestRenderer::BlitFramebufferToSwapchain(FrameBuffer& framebuffer)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.Handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, framebuffer.swidth, framebuffer.sheight, 0, 0, framebuffer.swidth, framebuffer.sheight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	IMesh::IMesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices) : m_IndexCount(indices.size())
	{
		glGenVertexArrays(1, &m_VAO);	
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // TexCoords
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);

	}

	IMesh::~IMesh()
	{
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		glDeleteVertexArrays(1, &m_VAO);
	}

	void IMesh::Render()
	{
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	


}