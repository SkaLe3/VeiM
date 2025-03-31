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
#include "Renderer/Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>



namespace VeiM
{
	Application* Application::s_Instance = nullptr;
	Application::Application(const ApplicationSpecification& applicationSpecification)
		: m_Config(applicationSpecification.AppConfig)
	{
		VM_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;


		PlatformService::SetCurrentWorkingDirectoryToBaseDir();


		m_Window = std::make_unique<Window>(applicationSpecification.WndConfig);  // TODO: Make static function Create() or smth
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

		ImGui::Begin("Test Renderer");

		ImGui::SliderFloat("x Angle", &xAngle, 90, -180, "%.0f");
		ImGui::SliderFloat("y Angle", &yAngle, 90, -180, "%.0f");
		ImGui::SliderFloat("z Angle", &zAngle, 0, -90, "%.0f");
		ImGui::SliderFloat("fow", &fow, 10, 90, "%.0f");
		ImGui::SliderFloat("distance", &distan, -10, 10, "%.0f");
		ImGui::End();

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
		std::vector<String> cmdArgs = m_Config.CommandLineArgs;
		fs::path shaderFilename;
		fs::path textureFilename;
		if (!cmdArgs.empty())
		{
			fs::path projectPath = fs::path(cmdArgs[0]);
			shaderFilename = projectPath.parent_path() / "Content" / "basic.glsl";
			textureFilename = projectPath.parent_path() / "Content" / "T_Cube.png";
		}
		else
		{
			shaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "basic.glsl";
			textureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_Cube.png";
		}

		Shader testShader = Shader(shaderFilename);
		m_Mesh = new CubeMesh();

		Texture texture0(textureFilename);
 		testShader.Bind();
 		testShader.SetUniformInt("u_Texture", 0);
 		testShader.UnBind();


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


			float real_display_h = display_h;
#ifdef VM_WITH_EDITOR
			real_display_h = display_h - 54;
#endif

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.5f));

			model = glm::rotate(glm::mat4(1.0f), glm::radians(xAngle), glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::rotate(glm::mat4(1.0f), glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * 
 					glm::rotate(glm::mat4(1.0f), glm::radians(zAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glm::mat4 view = glm::mat4(1.0f);
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, distan));

			glm::mat4 projection;
			projection = glm::perspective(glm::radians(fow), (float)display_w / real_display_h, 0.1f, 100.0f);

			glm::mat4 viewProj;
			viewProj = projection * view;

			
			TestRenderer::RenderMesh(m_Mesh, testShader, texture0, projection, view, model);
			m_Framebuffer.UnBind();
			
#ifdef VM_WITH_EDITOR
			RenderGUI();
#else
			TestRenderer::BlitFramebufferToSwapchain(m_Framebuffer);
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