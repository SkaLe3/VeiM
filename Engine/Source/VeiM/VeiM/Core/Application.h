#pragma once

#ifdef VM_WITH_EDITOR
#include "VeiM/ImGui/GUIContext.h"
#include "Image.h"
#endif

#include "VeiM/Core/Window.h"
#include "LayerStack.h"

#include <filesystem>
#include <string>
#include <functional>
#include <memory>

struct GLFWwindow;

namespace VeiM
{

	

	struct ApplicationConfig
	{
		std::string Name = "VeiM Engine";
		bool EnableConsole = true;
		std::filesystem::path WorkingDirectory;
		std::filesystem::path EngineContentPath;
		std::vector<String> CommandLineArgs;
	};


	struct ApplicationSpecification		  
	{
		ApplicationConfig AppConfig;
		WindowConfig WndConfig;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification);
		~Application();
		static Application& Get();

		void Run();


		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close() { m_Running = false; }

		inline std::filesystem::path GetEngineResourcePath() { return m_Config.EngineContentPath; }
		inline float GetDeltaTime() { return m_DeltaTime; }
		Window& GetWindow() const { return *m_Window; }
#ifdef VM_WITH_EDITOR
		VeiM::GUIContext* GetGUIContext() { return m_GUIContext; }
	private: 
		void InitGUI();
		void RenderGUI();
#endif
	private:
		void Shutdown();

		void OnEvent(const std::string& inf);
	private:
#ifdef VM_WITH_EDITOR
		VeiM::GUIContext* m_GUIContext;
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

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication(const std::vector<String>& arguments);
}

