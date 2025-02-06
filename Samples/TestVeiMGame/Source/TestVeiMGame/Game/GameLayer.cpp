#include "GameLayer.h"
#ifdef IS_UNIFIED

#include "Application/Application.h"
#include "Windows/WindowsUtils.h"

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
		TestRenderer::Init();
		// Compile shaders
		TriangleShader shader;
		uint32 shaderID = ShaderStatics::CreateProgram(shader.GetVertex().c_str(), shader.GetFragment().c_str());

		m_Mesh = new TriangleMesh();
		m_Mesh->SetShader(shaderID);
		m_Framebuffer.Invalidate(1280, 720);

	}

	void GameLayer::OnUpdate(float deltaTime)
	{
		GetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), display_w, display_h);
		m_Framebuffer.Invalidate(display_w, display_h);
		m_Framebuffer.Bind();
		TestRenderer::Clear();

		TestRenderer::RenderMesh(m_Mesh);
		m_Framebuffer.UnBind();
		TestRenderer::DisplayFramebufferToScreen(m_Framebuffer);
	}


}

#endif