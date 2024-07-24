#pragma once

#include "Image.h"
#include "ImGui/ImGuiContext.h"
#include "Core/Window.h"

#include <filesystem>
#include <string>
#include <functional>
#include <memory>

struct GLFWwindow;

namespace VeiM
{
	struct ApplicationSpecification
	{
		std::string Name = "VeiM Engine";
		uint32_t Width = 1600;
		uint32_t Height = 900;

		std::filesystem::path IconPath = "VeiM_Logo.png";

		bool WindowResizeable = true;
		bool CustomTitleBar = true;
		bool CenterWindow = false;
	};
	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
		~Application();

		static Application& Get();

		void Run();
		void Close() { m_Running = false; }

		float GetTime();
		Window& GetWindow() const { return *m_Window; }
		std::shared_ptr<VeiM::ImGuiContext> GetImGuiContext() { return m_ImGuiContext; }
		const ApplicationSpecification& GetSpecifications() const { return m_Specification; }

	private: 
		void Init();
		void Shutdown();


	private:
		ApplicationSpecification m_Specification;
		
		std::unique_ptr<Window> m_Window;

		bool m_Running = false;

		float m_TimeStep = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		bool show_demo_window = true;
		bool show_another_window = false;

		std::shared_ptr<VeiM::ImGuiContext> m_ImGuiContext;
	};

	Application* CreateApplication(int argc, char** argv);
}

