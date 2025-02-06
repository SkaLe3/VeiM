#include "TestRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VeiM
{

	void TestRenderer::Init()
	{
		glGenVertexArrays(1, &fullscreenQuadVAO);
		glGenBuffers(1, &fullscreenQuadVBO);
		glGenBuffers(1, &fullscreenQuadEBO);
		glBindVertexArray(fullscreenQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenQuadVertices), fullscreenQuadVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fullscreenQuadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		displayShader = ShaderStatics::CreateProgram(vertexShaderSource, fragmentShaderSource);
	}

	CORE_API void TestRenderer::RenderMesh(IMesh* mesh)
	{
		mesh->Render();
	}

	void TestRenderer::Clear()
	{
		// TODO: Put in RHI
		glClearColor(0.8, 0.8, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void TestRenderer::DisplayFramebufferToScreen(FrameBuffer& frameBufferA)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the default framebuffer (the screen)
		glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

		// Bind the framebuffer texture and render a full-screen quad
		glUseProgram(displayShader); // Use a simple shader to display the texture
		glBindVertexArray(fullscreenQuadVAO); // VAO for a fullscreen quad
		glBindTexture(GL_TEXTURE_2D, frameBufferA.GetTexture()); // Bind the framebuffer texture
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw the fullscreen quad
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

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float))); // TexCoords
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
		glUseProgram(m_Shader);
		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

}