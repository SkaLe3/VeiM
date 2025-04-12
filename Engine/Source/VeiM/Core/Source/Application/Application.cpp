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
			direction += glm::vec3(0.0f, 1.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			direction -= glm::vec3(0.0f, 1.0f, 0.0f);

		if (direction != glm::vec3{ 0.f, 0.f, 0.f })
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

			ImGui::SliderFloat("Shininess", &m_Shininess, 1.f, 128.f, "%.0f");

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

			ImGui::SeparatorText("Directional Lighting");
			ImGui::SliderFloat3("Ambient Color D", glm::value_ptr(m_DirLightAmbient), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Diffuse Color D", glm::value_ptr(m_DirLightDiffuse), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Specular Color D", glm::value_ptr(m_DirLightSpecular), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Direction D", glm::value_ptr(m_DirLightDirection), -1.0f, 1.0f, "%.2f");

			ImGui::SeparatorText("Point Lights");
			ImGui::SliderFloat3("Ambient Color P", glm::value_ptr(m_PointLightAmbient), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Diffuse Color P", glm::value_ptr(m_PointLightDiffuse), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Specular Color P", glm::value_ptr(m_PointLightSpecular), 0.0f, 1.0f, "%.3f");
			ImGui::DragFloat3("Light 1 Position", glm::value_ptr(m_PointLightPos1), 0.1f);
			ImGui::DragFloat3("Light 2 Position", glm::value_ptr(m_PointLightPos2), 0.1f);
			ImGui::DragFloat3("Light 3 Position", glm::value_ptr(m_PointLightPos3), 0.1f);
			ImGui::DragFloat3("Light 4 Position", glm::value_ptr(m_PointLightPos4), 0.1f);

			ImGui::SeparatorText("Spot Lights");
			ImGui::SliderFloat3("Ambient Color S", glm::value_ptr(m_SpotLightAmbient), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Diffuse Color S", glm::value_ptr(m_SpotLightDiffuse), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Specular Color S", glm::value_ptr(m_SpotLightSpecular), 0.0f, 1.0f, "%.3f");
			ImGui::DragFloat3("Position S", glm::value_ptr(m_SpotLightPos), 0.1f);
			ImGui::SliderFloat3("Direction S", glm::value_ptr(m_SpotLightDir), -1.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Inner", &m_SpotLightInner, 1.f, 180.f, "%.0f");
			ImGui::SliderFloat("Outer", &m_SpotLightOuter, 1.f, 180.f, "%.0f");

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
		fs::path cubeTextureFilename;
		fs::path sphereTextureFilename;
		fs::path grayTextureFilename;
		fs::path lightSourceShaderFilename;
		fs::path cubeShaderFilename;

		fs::path directionalLightShaderFilename;
		fs::path pointLightShaderFilename;
		fs::path spotLightShaderFilename;
		fs::path LitShaderFilename;

		fs::path boxDiffuseTextureFilename;
		fs::path boxSpecularTextureFilename;
		fs::path boxEmissionTextureFilename;

		VM_CORE_TRACE("VAlue: {0}", VM_GAME);
		if (IsRunningGame())
		{
			// TODO: User ProjectContentDir, but make sure to get correct path for unified build
			cubeShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "basic.glsl";
			cubeTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_Cube.png";
			pointLightShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "sun.glsl";
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
				sphereTextureFilename = Paths::ProjectContentDir() / "T_Sun.jpg";
				grayTextureFilename = Paths::ProjectContentDir() / "T_Gray.png";
				lightSourceShaderFilename = Paths::ProjectContentDir() / "lightsource.glsl";

				directionalLightShaderFilename = Paths::ProjectContentDir() / "directionallight.glsl";
				pointLightShaderFilename = Paths::ProjectContentDir() / "pointlight.glsl";
				spotLightShaderFilename = Paths::ProjectContentDir() / "spotlight.glsl";
				LitShaderFilename = Paths::ProjectContentDir() / "Lit.glsl";

				boxDiffuseTextureFilename = Paths::ProjectContentDir() / "T_BoxDiffuse.png";
				boxSpecularTextureFilename = Paths::ProjectContentDir() / "T_BoxSpecular.png";
				boxEmissionTextureFilename = Paths::ProjectContentDir() / "T_BoxEmission.png";

				bHasGame = true;
			}
		}

		Shader* cubeShader;
		Shader* lightSourceShader;

		Shader* directionalLightShader;
		Shader* pointLightShader;
		Shader* spotLightShader;
		Shader* litShader;


		Texture* cubeTexture;
		Texture* sphereTexture;
		Texture* grayTexture;

		Texture* boxDiffuse;
		Texture* boxSpecular;
		Texture* boxEmission;

		unsigned int lightVAO;
		if (bHasGame)
		{

			cubeShader = new Shader(cubeShaderFilename);
			lightSourceShader = new Shader(lightSourceShaderFilename);

			directionalLightShader = new Shader(directionalLightShaderFilename);
			pointLightShader = new Shader(pointLightShaderFilename);
			spotLightShader = new Shader(spotLightShaderFilename);
			litShader = new Shader(LitShaderFilename);

			m_CubeMesh = new CubeMesh();
			m_SphereMesh = new SphereMesh(8, 8);

			cubeTexture = new Texture(cubeTextureFilename);
			sphereTexture = new Texture(sphereTextureFilename);
			grayTexture = new Texture(grayTextureFilename);

			boxDiffuse = new Texture(boxDiffuseTextureFilename);
			boxSpecular = new Texture(boxSpecularTextureFilename);
			boxEmission = new Texture(boxEmissionTextureFilename);

			cubeShader->Bind();
			cubeShader->SetUniformInt("u_Texture", 0);
			cubeShader->UnBind();


			m_Framebuffer.Invalidate(1280, 720);
		}

		m_Camera = MakeUnique<EditorCamera>();
		m_Camera->SetPerspective(45.f, 1280.f / 720.f, 0.05f, 4000.f);
		m_Camera->SetPosition({ -2.f, 1.f, 4.f });




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
#ifndef VM_WITH_EDITOR
				int d_width;
				int d_height;
				GetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), d_width, d_height);
				m_Width = d_width;
				m_Height = d_height;
#endif
				m_Framebuffer.Invalidate(m_Width, m_Height);
				m_Camera->SetViewport(m_Width, m_Height);
				m_Camera->Update(m_DeltaTime);
				m_Framebuffer.Bind();
				//TestRenderer::SetClearColor(0.2f, 0.3f, 0.3f, 1.f);
				TestRenderer::SetClearColor(0.05f, 0.0f, 0.1f, 1.f);
				TestRenderer::Clear();


				glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f))
					* glm::toMat4(glm::quat(glm::vec3(glm::radians(-cubePitch), glm::radians(-cubeYaw), glm::radians(-cubeRoll))));

				glm::mat4 sphereModel = glm::mat4(1.0f);

				glm::mat4 viewProjection = m_Camera->GetViewProjection();

				glm::mat4 pointLightModel1 = glm::translate(glm::mat4(1.0f), m_PointLightPos1) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel2 = glm::translate(glm::mat4(1.0f), m_PointLightPos2) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel3 = glm::translate(glm::mat4(1.0f), m_PointLightPos3) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel4 = glm::translate(glm::mat4(1.0f), m_PointLightPos4) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

				glm::vec3 spotLigthDirection = glm::normalize(m_SpotLightDir);
				glm::quat spotLightRotation = glm::rotation(glm::vec3(0.0f, 0.0f, -1.0f), spotLigthDirection);

				glm::mat4 spotLightModel = glm::translate(glm::mat4(1.0f), m_SpotLightPos) * glm::toMat4(spotLightRotation) * glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.1f));




				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, boxDiffuse->GetTexID());

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, boxSpecular->GetTexID());

				litShader->Bind();
				litShader->SetMat4("u_ViewProjection", viewProjection);
				litShader->SetMat4("u_Transform", cubeModel);

				litShader->SetFloat3("u_ViewPos", m_Camera->GetPosition());

				litShader->SetUniformInt("u_Material.diffuse", 0);
				litShader->SetUniformInt("u_Material.specular", 1);
				litShader->SetFloat("u_Material.shininess", m_Shininess);

				litShader->SetFloat3("u_DirLight.direction", m_DirLightDirection);
				litShader->SetFloat3("u_DirLight.ambient", m_DirLightAmbient);
				litShader->SetFloat3("u_DirLight.diffuse", m_DirLightDiffuse);
				litShader->SetFloat3("u_DirLight.specular", m_DirLightSpecular);

				litShader->SetFloat3("u_PointLights[0].position", m_PointLightPos1);
				litShader->SetFloat3("u_PointLights[1].position", m_PointLightPos2);
				litShader->SetFloat3("u_PointLights[2].position", m_PointLightPos3);
				litShader->SetFloat3("u_PointLights[3].position", m_PointLightPos4);
				litShader->SetFloat("u_PointLights[0].constant", 1.0f);
				litShader->SetFloat("u_PointLights[1].constant", 1.0f);
				litShader->SetFloat("u_PointLights[2].constant", 1.0f);
				litShader->SetFloat("u_PointLights[3].constant", 1.0f);
				litShader->SetFloat("u_PointLights[0].linear", 0.09f);
				litShader->SetFloat("u_PointLights[1].linear", 0.09f);
				litShader->SetFloat("u_PointLights[2].linear", 0.09f);
				litShader->SetFloat("u_PointLights[3].linear", 0.09f);
				litShader->SetFloat("u_PointLights[0].quadratic", 0.032f);
				litShader->SetFloat("u_PointLights[1].quadratic", 0.032f);
				litShader->SetFloat("u_PointLights[2].quadratic", 0.032f);
				litShader->SetFloat("u_PointLights[3].quadratic", 0.032f);
				litShader->SetFloat3("u_PointLights[0].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[1].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[2].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[3].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[0].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[1].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[2].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[3].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[0].specular", m_PointLightSpecular);
				litShader->SetFloat3("u_PointLights[1].specular", m_PointLightSpecular);
				litShader->SetFloat3("u_PointLights[2].specular", m_PointLightSpecular);
				litShader->SetFloat3("u_PointLights[3].specular", m_PointLightSpecular);

				litShader->SetFloat3("u_SpotLights[0].position", m_SpotLightPos);
				litShader->SetFloat3("u_SpotLights[0].direction", m_SpotLightDir);
				litShader->SetFloat("u_SpotLights[0].innerCutOff", glm::cos(glm::radians(m_SpotLightInner)));
				litShader->SetFloat("u_SpotLights[0].outerCutOff", glm::cos(glm::radians(m_SpotLightOuter)));
				litShader->SetFloat("u_SpotLights[0].constant", 1.0f);
				litShader->SetFloat("u_SpotLights[0].linear", 0.09f);
				litShader->SetFloat("u_SpotLights[0].quadratic", 0.032f);

				litShader->SetFloat3("u_SpotLights[0].ambient", m_SpotLightAmbient);
				litShader->SetFloat3("u_SpotLights[0].diffuse", m_SpotLightDiffuse);
				litShader->SetFloat3("u_SpotLights[0].specular", m_SpotLightSpecular);


				glBindVertexArray(m_CubeMesh->GetVAO());
				glDrawElements(GL_TRIANGLES, m_CubeMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(5, 3, 5)));
				glDrawElements(GL_TRIANGLES, m_CubeMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 8)));
				glDrawElements(GL_TRIANGLES, m_CubeMesh->Indices.size(), GL_UNSIGNED_INT, 0);


				lightSourceShader->Bind();
				lightSourceShader->SetMat4("u_ViewProjection", viewProjection);
				lightSourceShader->SetMat4("u_Transform", pointLightModel1);
				lightSourceShader->SetFloat3("u_LightColor", m_PointLightDiffuse);
				glBindVertexArray(m_SphereMesh->GetVAO());
				glDrawElements(GL_TRIANGLES, m_SphereMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				lightSourceShader->SetMat4("u_Transform", pointLightModel2);
				glDrawElements(GL_TRIANGLES, m_SphereMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				lightSourceShader->SetMat4("u_Transform", pointLightModel3);
				glDrawElements(GL_TRIANGLES, m_SphereMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				lightSourceShader->SetMat4("u_Transform", pointLightModel4);
				glDrawElements(GL_TRIANGLES, m_SphereMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				lightSourceShader->SetMat4("u_Transform", spotLightModel);
				lightSourceShader->SetFloat3("u_LightColor", m_SpotLightDiffuse);
				glDrawElements(GL_TRIANGLES, m_SphereMesh->Indices.size(), GL_UNSIGNED_INT, 0);

				// Floor
				TestRenderer::RenderMesh(m_CubeMesh, *cubeShader, *grayTexture, viewProjection, glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -4.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(20.f, 1.f, 20.f)));
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