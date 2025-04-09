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
		FrameBuffer* DebugGetFramebuffer() { return &m_Framebuffer; }
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
		FrameBuffer m_Framebuffer;
		int display_w, display_h;
		IMesh* m_CubeMesh;
		IMesh* m_SphereMesh;
		float cubePitch = -45;
		float cubeYaw = -30;
		float cubeRoll = 0;
		float fov = 45.f;
		float size = 10;
		bool bHasGame = false;

	public:
		FrameBuffer& GetFramebuffer() { return m_Framebuffer; };

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication(const std::vector<String>& arguments);
}

