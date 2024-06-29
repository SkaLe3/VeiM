#include "Window.h"

#include "VeiM/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>		 
#include <stb_image.h>

#include <iostream>

static void glfw_error_callback(int error, const char* description)
{
	std::cout << "GLFW Error (" << error << "): " << description;
}

namespace VeiM
{

	Window::Window(ApplicationSpecification& specs) : m_Specification(specs)
	{
		Init();
		m_TitleBar = std::make_shared<UI::TitleBar>(this);
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		// SwapBuffers();
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
		{
			std::cout << "VSync enabled\n";
			glfwSwapInterval(1);
		}
		else
		{
			std::cout << "VSync disabled\n";
			glfwSwapInterval(0);
		}
	}

	bool Window::IsVSync() const
	{
		return m_Data.VSync;
	}

	bool Window::IsMaximized() const
	{
		return (bool)glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	GLFWwindow* Window::GetNativeWindow() const
	{
		return m_Window;
	}

	void Window::DrawBase()
	{
		const bool isMaximized = IsMaximized();
		{
			ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 50, 50, 255));
			// Draw window border if the window is not maximized
			if (!isMaximized)
				UI::RenderWindowOuterBorders(ImGui::GetCurrentWindow());
			ImGui::PopStyleColor(); // ImGuiCol_Border
		}
		if (GetSpecifications().CustomTitleBar)
		{
			float titleBarHeight;
			m_TitleBar->Draw(titleBarHeight);
			ImGui::SetCursorPosY(titleBarHeight);
		}
	}

	void Window::Init()
	{
		glfwSetErrorCallback(glfw_error_callback);

		std::cout << "Creating Window " << m_Specification.Name << " ( " << m_Specification.Width << " x " << m_Specification.Height << " )\n";
		int succeed = glfwInit();
		// TODO : Make assert
		if (!succeed)
		{
			std::cout << "Could not initialize GLFW\n";
			return;
		}
		// TODO : check somewhere
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		// ----------------------

		if (m_Specification.CustomTitleBar)
		{
			glfwWindowHint(GLFW_TITLEBAR, false);
			//glfwWindowHint(GLFW_DECORATED, false);
			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
		}

		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

		int monitorX, monitorY;
		glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		// Create window with graphics context
		m_Window = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Name.c_str(), nullptr, nullptr);

		// TODO : Make assert
		if (m_Window == nullptr)
		{
			std::cout << "Failed to create window\n";
			return;
		}

		glfwShowWindow(m_Window);
		// TODO : Move to graphics context class
		glfwMakeContextCurrent(m_Window);
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!gladStatus)
			// TODO : make assert
			std::cout << "Failed to initialize Glad\n";
		// TODO : Output info
		
		SetVSync(true);

		// Set icon
		GLFWimage icon;
		int channels;
		if (!m_Specification.IconPath.empty())
		{
			std::string iconPathStr = m_Specification.IconPath.string();
			icon.pixels = stbi_load(iconPathStr.c_str(), &icon.width, &icon.height, &channels, 4);
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(icon.pixels);
		}

		glfwSetWindowUserPointer(m_Window, this);
		glfwSetTitlebarHitTestCallback(m_Window, [](GLFWwindow* window, int x, int y, int* hit)
									   {
										   Window* appWindow = (Window*)glfwGetWindowUserPointer(window);
										   *hit = appWindow->m_TitleBar->IsHovered();
									   });
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

}