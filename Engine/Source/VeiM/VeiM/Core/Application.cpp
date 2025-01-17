#include "Application.h"

#ifdef VM_WITH_EDITOR
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 


#include "VeiM/Platform/Windows/WindowsUtils.h"
#include "VeiM/IO/Log.h"

#include "VeiM/Core/PlatformService.h"

extern bool g_ApplicationRunning;

namespace VeiM
{
	Application* Application::s_Instance = nullptr;
	Application::Application(const ApplicationSpecification& applicationSpecification)
		: m_Config(applicationSpecification.AppConfig)
	{
		VM_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		
		PlatformService::SetCurrentWorkingDirectoryToBaseDir();
		std::filesystem::current_path( std::filesystem::current_path().string() + "/../../../../");

		m_Window = std::make_unique<Window>(applicationSpecification.WndConfig);  // TODO: Make static function Create() or another
		m_Window->SetEventCallback([this](const std::string& inf) { Application::OnEvent(inf); });
#ifdef VM_WITH_EDITOR
		InitGUI();
#endif
	}

	Application::~Application()
	{
		Shutdown();
		s_Instance = nullptr;
	}

#ifdef VM_WITH_EDITOR
	void Application::InitGUI()
	{
		m_GUIContext = new GUIContext();
		PushOverlay(m_GUIContext);
	}

	void Application::RenderGUI()
	{
		m_GUIContext->BeginFrame();
		m_GUIContext->EnableDocking(*m_Window);
		{
			for (Layer* layer : m_LayerStack)
				layer->OnGUI();
		}


		m_GUIContext->EndFrame();
	}
#endif
	void Application::Shutdown()
	{

		g_ApplicationRunning = false;
	}



	Application& Application::Get()
	{
		return *s_Instance;
	}

	void Application::OnEvent(const std::string& inf)
	{
		// TODO: Dispatch events, change string to event type
		VM_CORE_INFO(inf);
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window->GetNativeWindow()) && m_Running)
		{
			m_Window->PollEvents();

			float time = Time::GetTime();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;


			// TODO: Put in RHI
			int display_w, display_h;
			glfwGetFramebufferSize(m_Window->GetNativeWindow(), &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(0.8, 0.8, 0.8, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);

#ifdef VM_WITH_EDITOR
			RenderGUI();
#endif

			m_Window->SwapBuffers();
		}
	}



	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushLayer(overlay);
		overlay->OnAttach();
	}

}