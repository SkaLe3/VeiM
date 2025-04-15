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
#include "Renderer/Model.h"

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

			ImGui::SeparatorText("General");
			ImGui::Checkbox("Draw Depth Buffer", &bDrawDepthBuffer);
			ImGui::Checkbox("Nature Skybox", &bUseNatureSkybox);
			ImGui::Checkbox("Explode", &bExplode);
			ImGui::Checkbox("Use Instancing", &bUseInstancing);
			ImGui::Checkbox("Use SRGB", &bUseSRGB);

			/* AA OPTIONS */
			static const char* aaOptions[] = {
				"No AA",
				"MSAA 2x",
				"MSAA 4x",
				"MSAA 8x",
				"MSAA 16x"
			};
			static uint32 aaSamples[] = {
				1,
				2,
				4,
				8,
				16
			};
			static int currentAAIndex = 2; // Default to "MSAA 4x"

			if (ImGui::BeginCombo("Anti-Aliasing Mode", aaOptions[currentAAIndex]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(aaOptions); n++)
				{
					bool isSelected = (currentAAIndex == n);
					if (ImGui::Selectable(aaOptions[n], isSelected))
						currentAAIndex = n;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			samplesNumber = aaSamples[currentAAIndex];

			/* END AA OPTIONS*/


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

		fs::path lightSourceShaderFilename;
		fs::path LitShaderFilename;
		fs::path borderShaderFilename;
		fs::path screenShaderFilename;
		fs::path skyboxShaderFilename;
		fs::path instanceShaderFilename;

		fs::path grayTextureFilename;
		fs::path boxDiffuseTextureFilename;
		fs::path boxSpecularTextureFilename;
		fs::path boxEmissionTextureFilename;

		fs::path backpackModelFilename;
		fs::path coinMeshFilename;

		std::vector<fs::path> skyboxFilenames;
		std::vector<fs::path> skyboxSFilenames;

		std::vector<String> faces
		{
			"right.jpg",
				"left.jpg",
				"top.jpg",
				"bottom.jpg",
				"front.jpg",
				"back.jpg"
		};

		std::vector<String> facesstylized
		{
			"px.png",
				"nx.png",
				"py.png",
				"ny.png",
				"pz.png",
				"nz.png"
		};

		VM_CORE_TRACE("VAlue: {0}", VM_GAME);
		if (IsRunningGame())
		{
			// TODO: User ProjectContentDir, but make sure to get correct path for unified build
			grayTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_Gray.png";
			lightSourceShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "lightsource.glsl";
			LitShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "Lit.glsl";
			borderShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "border.glsl";
			screenShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "screen.glsl";
			skyboxShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "skybox.glsl";
			instanceShaderFilename = fs::current_path().parent_path().parent_path() / "Content" / "instancing.glsl";

			boxDiffuseTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_BoxDiffuse.png";
			boxSpecularTextureFilename = fs::current_path().parent_path().parent_path() / "Content" / "T_BoxSpecular.png";
			backpackModelFilename = fs::current_path().parent_path().parent_path() / "Content" / "backpack" / "backpack.obj";
			coinMeshFilename = fs::current_path().parent_path().parent_path() / "Content" / "coin" / "Coin.obj";

			for (int i = 0; i < 6; i++)
				skyboxFilenames.push_back(fs::current_path().parent_path().parent_path() / "Content" / "skybox" / faces[i]);

			for (int i = 0; i < 6; i++)
				skyboxSFilenames.push_back(fs::current_path().parent_path().parent_path() / "Content" / "skybox2" / facesstylized[i]);

			bHasGame = true;
		}
		else
		{
			if (!cmdArgs.empty())
			{
				grayTextureFilename = Paths::ProjectContentDir() / "T_Gray.png";
				lightSourceShaderFilename = Paths::ProjectContentDir() / "lightsource.glsl";
				LitShaderFilename = Paths::ProjectContentDir() / "Lit.glsl";
				borderShaderFilename = Paths::ProjectContentDir() / "border.glsl";
				screenShaderFilename = Paths::ProjectContentDir() / "screen.glsl";
				skyboxShaderFilename = Paths::ProjectContentDir() / "skybox.glsl";
				instanceShaderFilename = Paths::ProjectContentDir() / "instancing.glsl";

				boxDiffuseTextureFilename = Paths::ProjectContentDir() / "T_BoxDiffuse.png";
				boxSpecularTextureFilename = Paths::ProjectContentDir() / "T_BoxSpecular.png";
				backpackModelFilename = Paths::ProjectContentDir() / "backpack" / "backpack.obj";
				coinMeshFilename = Paths::ProjectContentDir() / "coin" / "Coin.obj";

				for (int i = 0; i < 6; i++)
					skyboxFilenames.push_back(Paths::ProjectContentDir() / "skybox" / faces[i]);

				for (int i = 0; i < 6; i++)
					skyboxSFilenames.push_back(Paths::ProjectContentDir() / "skybox2" / facesstylized[i]);


				bHasGame = true;
			}
		}

		Shader* lightSourceShader;
		Shader* litShader;
		Shader* borderShader;
		Shader* screenShader;
		Shader* skyboxShader;
		Shader* instanceShader;

		Texture grayTexture;

		Texture boxDiffuse;
		Texture boxSpecular;
		CubeMap skybox;
		CubeMap skybox2;


		Model* backpackMesh;
		Model* coinMesh;

		SkyBoxCube* skyboxMesh;
		unsigned int uboMatrices;
		uint32 screenVAO, screenVBO;

		uint32 coinsCount = 3000;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[coinsCount];
		srand(glfwGetTime());
		float radius = 50.0;
		float offset = 15.0f;


		for (unsigned int i = 0; i < coinsCount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)coinsCount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y + 10.f, z));

			// 2. scale: scale between 0.05 and 0.25f
			float scale = (rand() % 40) / 100.0f + 0.4;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
		}
		// vertex buffer object
		unsigned int coinMatrixBuffer;


		if (bHasGame)
		{

			lightSourceShader = new Shader(lightSourceShaderFilename);
			litShader = new Shader(LitShaderFilename);
			borderShader = new Shader(borderShaderFilename);
			screenShader = new Shader(screenShaderFilename);
			skyboxShader = new Shader(skyboxShaderFilename);
			instanceShader = new Shader(instanceShaderFilename);

			m_CubeMesh = new CubeMesh();
			m_SphereMesh = new SphereMesh(8, 8);

			backpackMesh = new Model(backpackModelFilename);
			coinMesh = new Model(coinMeshFilename);
			skyboxMesh = new SkyBoxCube();
			skyboxMesh->Finilize();

			grayTexture = TextureFromFile(grayTextureFilename, ETextureColorSpace::sRGB);
			boxDiffuse = TextureFromFile(boxDiffuseTextureFilename, ETextureColorSpace::sRGB);
			boxSpecular = TextureFromFile(boxSpecularTextureFilename, ETextureColorSpace::Linear);

			m_CubeMesh->Tdiffuse = boxDiffuse;
			m_CubeMesh->Tspecualr = boxSpecular;

			skybox = loadCubemap(skyboxFilenames);
			skybox2 = loadCubemap(skyboxSFilenames);

			FramebufferSpecs fbspecs;
			fbspecs.Width = 1280;
			fbspecs.Height = 720;
			fbspecs.Samples = samplesNumber;
			m_Framebuffer = new FrameBuffer(fbspecs);
			fbspecs.Samples = 1;
			m_IntermediateFramebuffer = new FrameBuffer(fbspecs);
			m_PostProcessFramebuffer = new FrameBuffer(fbspecs);



			m_Camera = MakeUnique<EditorCamera>();
			m_Camera->SetPerspective(45.f, 1280.f / 720.f, 0.2f, 200.f);
			m_Camera->SetPosition({ 0.f, 5.f, 0.f });


			// Post Processing Quad
			float quadVertices[] = {
				// positions   // texcoords
				-1.0f, -1.0f,  0.0f, 0.0f,  // bottom left
				 1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
				-1.0f,  1.0f,  0.0f, 1.0f,  // top left
				-1.0f,  1.0f,  0.0f, 1.0f,  // top left
				 1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
				 1.0f,  1.0f,  1.0f, 1.0f   // top right
			};


			glGenVertexArrays(1, &screenVAO);
			glGenBuffers(1, &screenVBO);
			glBindVertexArray(screenVAO);
			glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

			glBindVertexArray(0);

			// Uniform Buffer
			/* Set uniform block equal to binding point 0*/
			unsigned int uniformBlockIndexLit = glGetUniformBlockIndex(litShader->GetId(), "Matrices");
			unsigned int uniformBlockIndexLightSource = glGetUniformBlockIndex(lightSourceShader->GetId(), "Matrices");
			unsigned int uniformBlockinstanced = glGetUniformBlockIndex(instanceShader->GetId(), "Matrices");
			glUniformBlockBinding(litShader->GetId(), uniformBlockIndexLit, 0);
			glUniformBlockBinding(lightSourceShader->GetId(), uniformBlockIndexLightSource, 0);
			glUniformBlockBinding(instanceShader->GetId(), uniformBlockinstanced, 0);

			/* Create UBO and allocate memory */

			glGenBuffers(1, &uboMatrices);
			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			/* Link entire range of the buffer to binding point 0*/
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(glm::mat4));



			/* Coint instancing */
			glGenBuffers(1, &coinMatrixBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, coinMatrixBuffer);
			glBufferData(GL_ARRAY_BUFFER, coinsCount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

			for (unsigned int i = 0; i < coinMesh->GetMeshesCount(); i++)
			{
				unsigned int VAO = coinMesh->GetMesh(i)->GetVAO();
				glBindVertexArray(VAO);
				// vertex attributes
				std::size_t vec4Size = sizeof(glm::vec4);
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

				glVertexAttribDivisor(3, 1);
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);

				glBindVertexArray(0);
			}

		}

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
				if (FramebufferSpecs spec = m_Framebuffer->Specs;
					m_Width > 0.0f && m_Height > 0.0 &&
					(spec.Width != m_Width || spec.Height != m_Height))
				{
					m_Framebuffer->Resize((uint32)m_Width, (uint32)m_Height);
					m_IntermediateFramebuffer->Resize((uint32)m_Width, (uint32)m_Height);
					m_PostProcessFramebuffer->Resize((uint32)m_Width, (uint32)m_Height);
					m_Camera->SetViewport(m_Width, m_Height);
				}
				if (m_Framebuffer->Specs.Samples != samplesNumber)
				{
					m_Framebuffer->Specs.Samples = samplesNumber;
					m_Framebuffer->Invalidate();
				}

				m_Camera->Update(m_DeltaTime);
				m_Framebuffer->Bind();
				TestRenderer::SetClearColor(0.2f, 0.3f, 0.3f, 1.f);
				//TestRenderer::SetClearColor(0.05f, 0.0f, 0.1f, 1.f);
				TestRenderer::Clear();


				glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f))
					* glm::toMat4(glm::quat(glm::vec3(glm::radians(-cubePitch), glm::radians(-cubeYaw), glm::radians(-cubeRoll))));

				glm::mat4 sphereModel = glm::mat4(1.0f);

				glm::mat4 viewProjection = m_Camera->GetViewProjection();
				glm::mat4 view = glm::mat4(glm::mat3(m_Camera->GetViewMatrix()));

				glm::mat4 pointLightModel1 = glm::translate(glm::mat4(1.0f), m_PointLightPos1) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel2 = glm::translate(glm::mat4(1.0f), m_PointLightPos2) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel3 = glm::translate(glm::mat4(1.0f), m_PointLightPos3) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel4 = glm::translate(glm::mat4(1.0f), m_PointLightPos4) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

				glm::vec3 spotLigthDirection = glm::normalize(m_SpotLightDir);
				glm::quat spotLightRotation = glm::rotation(glm::vec3(0.0f, 0.0f, -1.0f), spotLigthDirection);

				glm::mat4 spotLightModel = glm::translate(glm::mat4(1.0f), m_SpotLightPos) * glm::toMat4(spotLightRotation) * glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.1f));



				//				glEnable(GL_DEPTH_TEST);
				// 				glEnable(GL_STENCIL_TEST);
				// 				glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
				// 				glStencilMask(0xFF);

				float explodeTime = -1.0;
				if (bExplode)
					explodeTime = Time::GetTime();


				glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewProjection));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				m_CubeMesh->Tdiffuse = boxDiffuse;
				m_CubeMesh->Tspecualr = boxSpecular;


				litShader->Bind();
				litShader->SetFloat("u_Time", explodeTime);
				litShader->SetBool("u_UseDepth", bDrawDepthBuffer);


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

				instanceShader->Bind();
				instanceShader->SetFloat("u_Time", explodeTime);
				instanceShader->SetBool("u_UseDepth", bDrawDepthBuffer);

				if (bUseInstancing)
				{
					instanceShader->SetFloat3("u_ViewPos", m_Camera->GetPosition());

					instanceShader->SetUniformInt("u_Material.diffuse", 0);
					instanceShader->SetUniformInt("u_Material.specular", 1);
					instanceShader->SetFloat("u_Material.shininess", m_Shininess);

					instanceShader->SetFloat3("u_DirLight.direction", m_DirLightDirection);
					instanceShader->SetFloat3("u_DirLight.ambient", m_DirLightAmbient);
					instanceShader->SetFloat3("u_DirLight.diffuse", m_DirLightDiffuse);
					instanceShader->SetFloat3("u_DirLight.specular", m_DirLightSpecular);

					instanceShader->SetFloat3("u_PointLights[0].position", m_PointLightPos1);
					instanceShader->SetFloat3("u_PointLights[1].position", m_PointLightPos2);
					instanceShader->SetFloat3("u_PointLights[2].position", m_PointLightPos3);
					instanceShader->SetFloat3("u_PointLights[3].position", m_PointLightPos4);
					instanceShader->SetFloat("u_PointLights[0].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[1].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[2].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[3].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[0].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[1].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[2].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[3].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[0].quadratic", 0.032f);
					instanceShader->SetFloat("u_PointLights[1].quadratic", 0.032f);
					instanceShader->SetFloat("u_PointLights[2].quadratic", 0.032f);
					instanceShader->SetFloat("u_PointLights[3].quadratic", 0.032f);
					instanceShader->SetFloat3("u_PointLights[0].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[1].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[2].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[3].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[0].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[1].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[2].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[3].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[0].specular", m_PointLightSpecular);
					instanceShader->SetFloat3("u_PointLights[1].specular", m_PointLightSpecular);
					instanceShader->SetFloat3("u_PointLights[2].specular", m_PointLightSpecular);
					instanceShader->SetFloat3("u_PointLights[3].specular", m_PointLightSpecular);

					instanceShader->SetFloat3("u_SpotLights[0].position", m_SpotLightPos);
					instanceShader->SetFloat3("u_SpotLights[0].direction", m_SpotLightDir);
					instanceShader->SetFloat("u_SpotLights[0].innerCutOff", glm::cos(glm::radians(m_SpotLightInner)));
					instanceShader->SetFloat("u_SpotLights[0].outerCutOff", glm::cos(glm::radians(m_SpotLightOuter)));
					instanceShader->SetFloat("u_SpotLights[0].constant", 1.0f);
					instanceShader->SetFloat("u_SpotLights[0].linear", 0.09f);
					instanceShader->SetFloat("u_SpotLights[0].quadratic", 0.032f);

					instanceShader->SetFloat3("u_SpotLights[0].ambient", m_SpotLightAmbient);
					instanceShader->SetFloat3("u_SpotLights[0].diffuse", m_SpotLightDiffuse);
					instanceShader->SetFloat3("u_SpotLights[0].specular", m_SpotLightSpecular);



					instanceShader->Bind();
					for (unsigned int i = 0; i < coinMesh->GetMeshesCount(); i++)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, coinMesh->GetMesh(i)->Tdiffuse.Id);


						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, coinMesh->GetMesh(i)->Tspecualr.Id);
						glBindVertexArray(coinMesh->GetMesh(i)->GetVAO());
						glDrawElementsInstanced(
							GL_TRIANGLES, coinMesh->GetMesh(i)->Indices.size(), GL_UNSIGNED_INT, 0, coinsCount
						);
						glBindVertexArray(0);
					}
				}

				// Floor
				m_CubeMesh->Tdiffuse = grayTexture;
				m_CubeMesh->Tspecualr = boxSpecular;
				litShader->Bind();
				litShader->SetMat4("u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -4.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(20.f, 1.f, 20.f)));
				TestRenderer::RenderMesh(m_CubeMesh, *litShader);

				m_CubeMesh->Tdiffuse = boxDiffuse;
				m_CubeMesh->Tspecualr = boxSpecular;

				litShader->SetMat4("u_Transform", cubeModel);
				TestRenderer::RenderMesh(m_CubeMesh, *litShader);

				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(5, 3, 5)));
				TestRenderer::RenderMesh(m_CubeMesh, *litShader);

				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 8)));
				TestRenderer::RenderMesh(m_CubeMesh, *litShader);


				lightSourceShader->Bind();
				lightSourceShader->SetFloat3("u_LightColor", m_PointLightDiffuse);


				lightSourceShader->SetMat4("u_Transform", pointLightModel1);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetMat4("u_Transform", pointLightModel2);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetMat4("u_Transform", pointLightModel3);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetMat4("u_Transform", pointLightModel4);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetFloat3("u_LightColor", m_SpotLightDiffuse);
				lightSourceShader->SetMat4("u_Transform", spotLightModel);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				// Backpack
				litShader->Bind();
				glm::mat4 backpackModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, -5.f));
				litShader->SetMat4("u_Transform", backpackModel);
				backpackMesh->Draw(*litShader);

				// Coin
				litShader->Bind();
				glm::mat4 coinModel = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 4.0f, -5.f));
				litShader->SetMat4("u_Transform", coinModel);
				coinMesh->Draw(*litShader);

				if (!bUseInstancing)
				{
					for (uint32 i = 0; i < coinsCount; i++)
					{
						litShader->SetMat4("u_Transform", modelMatrices[i]);
						coinMesh->Draw(*litShader);
					}

				}

				/*				glStencilMask(0x00);*/
				uint32 skyboxId;
				if (bUseNatureSkybox)
					skyboxId = skybox.Id;
				else
					skyboxId = skybox2.Id;

				skyboxShader->Bind();
				skyboxShader->SetMat4("u_ViewProjection", m_Camera->GetProjectionMatrix() * view);
				skyboxShader->SetUniformInt("u_SkyBox", 0);
				glDisable(GL_CULL_FACE);
				glDepthFunc(GL_LEQUAL);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxId);
				glBindVertexArray(skyboxMesh->VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS);
				glEnable(GL_CULL_FACE);


				m_Framebuffer->UnBind();
				TestRenderer::BlitFramebufferTo(*m_Framebuffer, *m_IntermediateFramebuffer);



				m_PostProcessFramebuffer->Bind();
				TestRenderer::BlitStencil(*m_IntermediateFramebuffer, *m_PostProcessFramebuffer);

				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				screenShader->Bind();
				glBindVertexArray(screenVAO);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, m_IntermediateFramebuffer->GetTexture());
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glEnable(GL_DEPTH_TEST);
				m_PostProcessFramebuffer->UnBind();

			}

#ifdef VM_WITH_EDITOR
			RenderGUI();
#else
			if (bHasGame)
			{
				TestRenderer::BlitFramebufferToSwapchain(m_PostProcessFramebuffer);


			}
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