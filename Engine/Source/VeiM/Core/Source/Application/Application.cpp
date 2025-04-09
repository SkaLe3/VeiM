#include "Application.h"

#ifdef VM_WITH_EDITOR
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#include "HAL/PlatformService.h"
#include "Windows/WindowsUtils.h"
#include "Misc/Paths.h"
#include "Misc/Timer.h"
#include "Logging/Log.h"
#include "Project/ModuleManager.h"
#include "Types/StringID.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 


#include "Test/Base.h" // TEmporary
#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"
#include "Renderer/SphereMesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <sstream>


namespace VeiM
{
	UniquePtr<EditorCamera> m_Camera;

	bool firstMouse = true;
	float lastX = 0;
	float lastY = 0;



	bool cursorHidden = false;
	bool switched = false;
	bool pan = false;

	void process_input(GLFWwindow* window)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			if (!cursorHidden)
				switched = true;
			cursorHidden = true;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
		{
			if (cursorHidden)
				switched = true;
			cursorHidden = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
		{
			pan = true;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_RELEASE)
		{
			pan = false;
		}
		if (switched)
		{
			if (cursorHidden)
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			switched = false;
			firstMouse = true;
		}

		if (!cursorHidden)
			return;
		glm::vec3 direction = { 0.f, 0.f, 0.f };

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			direction += m_Camera->GetForwardVector();
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			direction += -m_Camera->GetForwardVector();
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			direction += -m_Camera->GetRightVector();
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			direction += m_Camera->GetRightVector();
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			direction += m_Camera->GetUpVector();
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			direction -= m_Camera->GetUpVector();

		if (direction != glm::vec3{0.f, 0.f, 0.f})
			direction = glm::normalize(direction);
		m_Camera->InputKey(Application::Get().GetDeltaTime(), direction);
	}

	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{

		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		if (pan)
		{
			m_Camera->InputPan({ xoffset, yoffset });
		}
		else
		if (cursorHidden)
			m_Camera->InputMouse({ xoffset, yoffset });
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		m_Camera->InputScroll({ xoffset, yoffset });
	}

}


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
		// Temp
		glfwSetCursorPosCallback(m_Window->GetNativeWindow(), mouse_callback);
		glfwSetScrollCallback(m_Window->GetNativeWindow(), scroll_callback);


#ifdef VM_WITH_EDITOR

		std::vector<String> cmdArgs = m_Config.CommandLineArgs;
		if (!cmdArgs.empty())
		{
			Paths::SetProjectFilePath(cmdArgs[0]);
			ModuleManager::Get().SetGameBianariesDir(Paths::ProjectDir() / "Binaries" / "Win64");
			ModuleManager::Get().LoadModule(Paths::GetProjectFilePath().stem().wstring());
			ModuleManager::Get().LoadModule(Paths::GetProjectFilePath().stem().wstring());
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

		if (bHasGame)
		{

			ImGui::Begin("Test Renderer");

			ImGui::SeparatorText("Cube Controls");
			ImGui::SliderFloat("Cube Pitch", &cubePitch, 0, 360, "%.0f");
			ImGui::SliderFloat("Cube Yaw", &cubeYaw, 0, 360, "%.0f");
			ImGui::SliderFloat("Cube Roll", &cubeRoll, 0, 360, "%.0f");

			ImGui::SeparatorText("Camera");
			const char* projectionTypes[] = { "Perspective", "Orthographic" };
			int currentProjectionIndex = static_cast<int>(m_Camera->GetProjectionType());

			if (ImGui::Combo("Projection", &currentProjectionIndex, projectionTypes, IM_ARRAYSIZE(projectionTypes)))
			{
				m_Camera->SetProjectionType(static_cast<Camera::ProjectionType>(currentProjectionIndex));
				m_Camera->SetOrthoHeight(size);
			}

			const char* fovAxis[] = { "Horizontal", "Vertical" };
			int currentAxis = static_cast<int>(m_Camera->GetFovAxis());

			if (ImGui::Combo("FOV Axis", &currentAxis, fovAxis, IM_ARRAYSIZE(fovAxis)))
			{
				m_Camera->SetFovAxis(static_cast<Camera::FOVAxis>(currentAxis));
			}

			if (ImGui::SliderFloat("FOV", &fov, 10, 100, "%.0f"))
			{
				m_Camera->SetFov(fov);
			}
			if (ImGui::SliderFloat("Ortho Size", &size, 1, 20, "%.0f"))
			{
				m_Camera->SetOrthoHeight(size);
			}
			ImGui::SliderFloat("Camera Speed", &m_Camera->MovementSpeed(), 1.f, 25.f, "%.0f");
			ImGui::SliderFloat("Camera Sensitivity", &m_Camera->Sensitivity(), 0.05f, 1.f, "%.2f");
			ImGui::End();
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
		std::vector<String> cmdArgs = m_Config.CommandLineArgs;
		fs::path cubeShaderFilename;
		fs::path sphereShaderFilename;
		fs::path cubeTextureFilename;
		fs::path sphereTextureFilename;
		fs::path grayTextureFilename;

		if (IsRunningGame())
		{
			// TODO: User ProjectContentDir, but make sure to get correct path for unified build
			cubeShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "basic.glsl";
			cubeTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_Cube.png";
			sphereShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "sun.glsl";
			sphereTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_Sun.jpg";
			grayTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_Gray.png";
			bHasGame = true;
		}
		else
		{
			if (!cmdArgs.empty())
			{
				cubeShaderFilename = Paths::ProjectContentDir() / "basic.glsl";
				cubeTextureFilename = Paths::ProjectContentDir() / "T_Cube.png";
				sphereShaderFilename = Paths::ProjectContentDir() / "sun.glsl";
				sphereTextureFilename = Paths::ProjectContentDir() / "T_Sun.jpg";
				grayTextureFilename = Paths::ProjectContentDir() / "T_Gray.png";
				bHasGame = true;
			}
		}

		Shader* cubeShader;
		Shader* sphereShader;
		Texture* cubeTexture;
		Texture* sphereTexture;
		Texture* grayTexture;

		if (bHasGame)
		{

			cubeShader = new Shader(cubeShaderFilename);
			sphereShader = new Shader(sphereShaderFilename);
			m_CubeMesh = new CubeMesh();
			m_SphereMesh = new SphereMesh(24, 24);

			cubeTexture = new Texture(cubeTextureFilename);
			sphereTexture = new Texture(sphereTextureFilename);
			grayTexture = new Texture(grayTextureFilename);
			cubeShader->Bind();
			cubeShader->SetUniformInt("u_Texture", 0);
			cubeShader->UnBind();
			sphereShader->Bind();
			sphereShader->SetUniformInt("u_Texture", 0);
			sphereShader->UnBind();

			m_Framebuffer.Invalidate(1280, 720);
		}

		m_Camera = MakeUnique<EditorCamera>();
		m_Camera->SetPerspective(45.f, 1280.f / 720.f, 0.05f, 4000.f);

		while (!glfwWindowShouldClose(m_Window->GetNativeWindow()) && m_Running)
		{
			m_Window->PollEvents();

			float time = Time::GetTime();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			process_input(m_Window->GetNativeWindow());

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(m_DeltaTime);

			if (bHasGame)
			{

				if (character)
				{
					character->Update(m_DeltaTime);
				}
				GetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), display_w, display_h);
				float real_display_h = display_h;
#ifdef VM_WITH_EDITOR
				real_display_h = display_h - 54;
#endif

				m_Framebuffer.Invalidate(display_w, real_display_h);
				m_Camera->SetViewport(display_w, real_display_h);
				m_Camera->Update(m_DeltaTime);
				m_Framebuffer.Bind();
				TestRenderer::SetClearColor(0.2f, 0.3f, 0.3f, 1.f);
				//TestRenderer::SetClearColor(0.05f, 0.0f, 0.1f, 1.f);
				TestRenderer::Clear();


				glm::mat4 cubeModel = glm::mat4(1.0f);
				cubeModel = glm::translate(cubeModel, glm::vec3(-2.0f, 0.0f, 0)) 
					* glm::toMat4(glm::quat(glm::vec3(glm::radians(-cubePitch), glm::radians(-cubeYaw), glm::radians(-cubeRoll))));

				glm::mat4 sphereModel = glm::mat4(1.0f);
				sphereModel = glm::translate(sphereModel, glm::vec3(2.f, 0.0f, 0)) * glm::scale(glm::mat4(1.0f), { 1.5f, 1.5f, 1.5f });

				glm::mat4 view = m_Camera->GetViewMatrix();

				glm::mat4 projection = m_Camera->GetProjectionMatrix();

				TestRenderer::RenderMesh(m_SphereMesh, *sphereShader, *sphereTexture, projection, view, sphereModel);
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *cubeTexture, projection, view, cubeModel);
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *cubeTexture, projection, view, glm::translate(glm::mat4(1.0f), glm::vec3(1.f, 4.f, -2.f)));
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *cubeTexture, projection, view, glm::translate(glm::mat4(1.0f), glm::vec3(4.f, 5.f, -5.f)));
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *cubeTexture, projection, view, glm::translate(glm::mat4(1.0f), glm::vec3(-5.f, 0.f, -3.f)));
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *cubeTexture, projection, view, glm::translate(glm::mat4(1.0f), glm::vec3(-3.f, 3.f, -2.f)));
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *cubeTexture, projection, view, glm::translate(glm::mat4(1.0f), glm::vec3(1.f, 4.f, -4.f)));
				// Floor
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *grayTexture, projection, view, glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -4.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(20.f, 1.f, 20.f)));
				m_Framebuffer.UnBind();
			}

#ifdef VM_WITH_EDITOR
			RenderGUI();
#else
			if (bHasGame)
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