#include "Application.h"

#include "VeiM/UI/Titlebar.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>		 

extern bool g_ApplicationRunning;

namespace VeiM
{
#include "VeiM/UI/Embed/VeiM-Icon.embed"
#include "VeiM/UI/Embed/WindowIcons.embed"

	static VeiM::Application* s_Instance = nullptr;
	Application::Application(const ApplicationSpecification& applicationSpecification /*= applicationSpecification()*/)
		: m_Specification(applicationSpecification)
	{
		// TODO : Add logger init
		s_Instance = this;
		Init();
	}

	Application::~Application()
	{
		Shutdown();
		s_Instance = nullptr;
	}
	void Application::Init()
	{
		m_Window = std::make_unique<Window>(m_Specification);
		m_ImGuiContext = std::make_shared<ImGuiContext>();
		m_ImGuiContext->Init();
	}

	void Application::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		g_ApplicationRunning = false;
	}



	Application& Application::Get()
	{
		return *s_Instance;
	}

	void Application::Run()
	{
		m_Running = true;
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		// Main loop

		while (!glfwWindowShouldClose(m_Window->GetNativeWindow()) && m_Running)
		{
			// Poll and handle events (inputs, window resize, etc.)
			// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
			// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
			// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
			// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			glfwPollEvents();

			float time = (float)glfwGetTime();
			m_FrameTime = time - m_LastFrameTime;
			m_TimeStep = std::min<float>(m_FrameTime, 0.0333f);
			m_LastFrameTime = time;


			int display_w, display_h;
			glfwGetFramebufferSize(m_Window->GetNativeWindow(), &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);

			m_ImGuiContext->BeginFrame();
			m_ImGuiContext->EnableDocking(*m_Window);
			m_Window->DrawBase();
			m_ImGuiContext->RenderDockspace();

			if (!m_Window->GetSpecifications().CustomTitleBar)
				m_Window->GetTitlebar().DrawMenubar();


			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}

			

			ImGui::End();
			m_ImGuiContext->EndFrame();

			glfwSwapBuffers(m_Window->GetNativeWindow());
		}
	}







}