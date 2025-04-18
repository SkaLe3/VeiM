#pragma once

#ifdef VM_WITH_EDITOR
#include "ImGui/GUIContext.h"
#include "UI/Image.h"
#endif

#include "CoreDefines.h"
#include "Application/Window.h"
#include "LayerStack.h"

#include <filesystem>
#include <string>
#include <functional>
#include <memory>


// TEMPORARY
#include "Test/FrameBuffer.h"
#include "Test/TestRenderer.h"
#include "Renderer/EditorCamera.h"

struct GLFWwindow;

namespace VeiM
{
	struct CORE_API ApplicationConfig
	{
		std::string Name = "VeiM Engine";
		bool EnableConsole = true;
		std::vector<String> CommandLineArgs;
	};


	struct CORE_API ApplicationSpecification
	{
		ApplicationConfig AppConfig;
		WindowConfig WndConfig;
	};
	// TODO: Refactor Application class. Use Application only for Editor build
	class CORE_API Application
	{
	public:
		FrameBuffer* DebugGetFramebuffer() { return m_PostProcessFramebuffer; }
		uint32 DebugGetFramebufferTexture() { return depthMap; }
	public:
		Application(const ApplicationSpecification& applicationSpecification);
		~Application();
		static Application& Get();

		void Run();


		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close() { m_Running = false; }

		inline float GetDeltaTime() { return m_DeltaTime; }
		Window& GetWindow() const { return *m_Window; }
		ApplicationConfig GetConfig() { return m_Config; }
		std::wstring GetConfiguration(); // TODO: Create enum class and put EnumToString in some file like PlatformMisc
#ifdef VM_WITH_EDITOR
		GUIContext* GetGUIContext() { return m_GUIContext; }
		void ViewportResize(float vwidth, float vheight) { m_Width = vwidth; m_Height = vheight; }
	private:
		void InitGUI();
		void RenderGUI();
#endif
	private:
		void Shutdown();

		void OnEvent(const std::string& inf);
	private:
#ifdef VM_WITH_EDITOR
		GUIContext* m_GUIContext;
#endif
	private:
		ApplicationConfig m_Config;
		std::unique_ptr<Window> m_Window;

		LayerStack m_LayerStack;

		bool m_Running = true;
		bool m_Minimized = false;

		float m_DeltaTime = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;


		//Temp 
		FrameBuffer* m_Framebuffer;
		FrameBuffer* m_IntermediateFramebuffer;
		FrameBuffer* m_PostProcessFramebuffer;
		uint32 samplesNumber = 1;

		IMesh* m_CubeMesh;
		IMesh* m_SphereMesh;
		float cubePitch = 0;
		float cubeYaw = 0;
		float cubeRoll = 0;
		float fov = 45.f;
		float size = 10;
		bool bHasGame = false;

		bool bDrawDepthBuffer = false;
		bool bUseNatureSkybox = true;
		bool bExplode = false;
		bool bUseInstancing = true;
		bool bCastGlobalShadows = true;
		bool bCastPointShadows = true;
		bool bUseSRGB = false;
		bool bUseNormalMaps = true;
		int32 m_ShadowMapLevel = 3;
		uint32 SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
		uint32 pSHADOW_WIDTH = 1024, pSHADOW_HEIGHT = 1024;
		float m_ShadowBiasMin = 0.001f;
		float m_ShadowBiasMax = 0.005f;

		float m_Shininess = 64;

		glm::vec3 m_DirLightAmbient = { 0.2f, 0.2f, 0.2f };
		glm::vec3 m_DirLightDiffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_DirLightSpecular = { 1.f, 1.f, 1.f };
		glm::vec3 m_DirLightDirection = { -0.2f, -1.f, -0.3f };

		glm::vec3 m_PointLightAmbient = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_PointLightDiffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_PointLightSpecular = { 1.f, 1.f, 1.f };

		glm::vec3 m_SpotLightAmbient = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_SpotLightDiffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_SpotLightSpecular = { 1.f, 1.f, 1.f };


		glm::vec3 m_PointLightPos1 = { -1.0f, -20.2f,  0.0f };
		glm::vec3 m_PointLightPos2 = {  1.0f, -20.2f,  0.0f };
		glm::vec3 m_PointLightPos3 = {  0.0f, -20.2f, -1.0f };
		glm::vec3 m_PointLightPos4 = {  0.0f, -20.2f,  1.0f };

		glm::vec3 m_SpotLightPos = { 5.f, 4.0f, 5.0f };
		glm::vec3 m_SpotLightDir = { 0.f, -1.f, 0.f };
		float m_SpotLightInner = 12.5f;
		float m_SpotLightOuter = 15.f;


		uint32 m_Width = 600;
		uint32 m_Height = 400;

		// Shadows
		unsigned int depthMapFBO;
		unsigned int depthMap; // Depth Map for shadow maps
		unsigned int depthCubeMapFBO;
		uint32 depthCubemap;

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication(const std::vector<String>& arguments);
}

