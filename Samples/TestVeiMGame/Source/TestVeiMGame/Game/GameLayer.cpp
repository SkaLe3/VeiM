#include "GameLayer.h"
#include "VeiM/Core/Application.h"

#include <iostream> // TODO: remove

#include <glad/glad.h> // TODO: remove
#include <GLFW/glfw3.h>


#include "GameClasses.h"





namespace VeiM
{
	GameLayer::GameLayer() : Layer("GameLayer")
	{

	}


	void GameLayer::OnAttach()
	{
		// Triangle VAO
		TriangleMesh triangleMesh;
		triangleVAO, triangleVBO;
		glGenVertexArrays(1, &triangleVAO);
		glGenBuffers(1, &triangleVBO);

		glBindVertexArray(triangleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
		glBufferData(GL_ARRAY_BUFFER, triangleMesh.GetSize(), triangleMesh.GetData(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Compile shaders
		TriangleShader shader;
		triangleShader = ShaderStatics::CreateProgram(shader.GetVertex().c_str(), shader.GetFragment().c_str());

		m_Framebuffer.Invalidate(1280, 720);

	}

	void GameLayer::OnUpdate(float deltaTime)
	{
		glfwGetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), &display_w, &display_h);
		m_Framebuffer.Invalidate(display_w, display_h);
		m_Framebuffer.Bind();

		// TODO: Put in RHI
		glClearColor(0.8, 0.8, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);


		glUseProgram(triangleShader);
		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 800, 600);
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(quadShader);
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, m_Framebuffer.GetTexture());
		glDrawArrays(GL_TRIANGLES, 0, 6);


		m_Framebuffer.UnBind();
	}


}