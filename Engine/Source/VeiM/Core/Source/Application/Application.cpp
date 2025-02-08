#include "Application.h"

#ifdef VM_WITH_EDITOR
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#include "HAL/PlatformService.h"
#include "Windows/WindowsUtils.h"
#include "Logging/Log.h"
#include "Project/ModuleManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 


#include "Test/Base.h" // TEmporary



const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color
}
)";


namespace VeiM
{
	Application* Application::s_Instance = nullptr;
	Application::Application(const ApplicationSpecification& applicationSpecification)
		: m_Config(applicationSpecification.AppConfig)
	{
		VM_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;


		PlatformService::SetCurrentWorkingDirectoryToBaseDir();
		std::filesystem::current_path(std::filesystem::current_path().string() + "/../../../../");

		m_Window = std::make_unique<Window>(applicationSpecification.WndConfig);  // TODO: Make static function Create() or another
		m_Window->SetEventCallback([this](const std::string& inf) { Application::OnEvent(inf); });


#ifdef VM_WITH_EDITOR

		std::vector<String> cmdArgs = m_Config.CommandLineArgs;
		if (!cmdArgs.empty())
		{
			fs::path projectPath = fs::path(cmdArgs[0]);
			ModuleManager::Get().SetGameBianariesDir(projectPath.parent_path() / "Binaries" / "Win64");
			ModuleManager::Get().LoadModule(projectPath.stem().wstring());
			ModuleManager::Get().LoadModule(projectPath.stem().wstring());
		}

		InitGUI();
#endif
	}

	Application::~Application()
	{
		Shutdown();
		s_Instance = nullptr;
	}
	std::wstring Application::GetConfiguration()
	{
#if defined(VM_DEBUG)
		return TEXT("Debug");
#elif defined(VM_DEVELOPMENT)
		return TEXT("Development");
#elif defined(VM_SHIPPING)
		return TEXT("Shipping");
#endif
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

		UniquePtr<Base> character = Base::Instantiate("GameCharacter");
		if (character)
		{
			character->Start();
		}
		TestRenderer::Init();
		// Compile shaders
		uint32 myshader = ShaderStatics::CreateProgram(vertexShaderSource, fragmentShaderSource);

		m_Mesh = new QuadMesh();

		m_Mesh->SetShader(myshader);
		m_Framebuffer.Invalidate(1280, 720);

		while (!glfwWindowShouldClose(m_Window->GetNativeWindow()) && m_Running)
		{
			m_Window->PollEvents();

			float time = Time::GetTime();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(m_DeltaTime);
			if (character)
			{
				character->Update(m_DeltaTime);
			}
			GetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), display_w, display_h);
			m_Framebuffer.Invalidate(display_w, display_h);
			m_Framebuffer.Bind();
			TestRenderer::Clear();

			TestRenderer::RenderMesh(m_Mesh);
			m_Framebuffer.UnBind();

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