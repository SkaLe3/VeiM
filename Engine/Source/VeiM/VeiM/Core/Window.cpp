#include "VeiM/Core/Window.h"

#include "VeiM/Core/Application.h"
#include "VeiM/IO/Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 
#include <stb_image.h>

namespace VeiM
{
	static void glfw_error_callback(int error, const char* description)
	{
		VM_CORE_ERROR("[GLFW] Error({0}) : {1}", error, description);
	}

	Window::Window(const WindowConfig& config)
	{
		WindowData WD;
		WD.Title = config.Title;
		WD.Width = config.Width;
		WD.Height = config.Height;
		WD.VSync = config.VSync;
		WD.CustomTitlebar = config.CustomTitlebar;
		WD.Mode = config.Mode;
		m_Data = WD;
		Init(config);
	}


	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}

	void Window::PollEvents()
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
	}

	glm::vec2 Window::GetScreenSize() const
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		return { mode->width, mode->height };
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
		{
			VM_CORE_TRACE("[GLFW] VSync enabled");
			glfwSwapInterval(1);
		}
		else
		{
			VM_CORE_TRACE("[GLFW] VSync disabled");
			glfwSwapInterval(0);
		}
		m_Data.VSync = enabled;
	}

	bool Window::IsVSyncEnabled() const
	{
		return m_Data.VSync;
	}

	void Window::Hide()
	{
		glfwSetWindowOpacity(m_Window, 0.0f);
	}

	void Window::Show()
	{
		glfwSetWindowOpacity(m_Window, 1.0f);
	}

	void Window::SetRawInput(bool enabled)
	{
		if (glfwRawMouseMotionSupported())
		{
			if (enabled)
			{
				VM_CORE_TRACE("[GLFW] Raw mouse motion enabled");
				glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

			}
			else
			{
				VM_CORE_TRACE("[GLFW] Raw mouse motion enabled");
				glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
			}
		}
		else
		{
			VM_CORE_WARN("[GLFW] Raw mouse motion not supported on this platform!");
		}
	}

	bool Window::IsRawInputEnabled() const
	{
		return glfwGetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION) == GLFW_TRUE;
	}

	GLFWwindow* Window::GetNativeWindow() const
	{
		return m_Window;
	}


	void Window::SetWindowMode(EWindowMode mode)
	{
		EWindowMode currentMode = GetWindowMode();
		GLFWwindow* hWnd = reinterpret_cast<GLFWwindow*>(m_Window);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		if (currentMode == mode)
			return;

		// Remove fullscreen
		if (currentMode == EWindowMode::Fullscreen)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(hWnd, nullptr, 0, 0, m_Data.Width, m_Data.Width, mode->refreshRate);
		}

		switch (mode)
		{
		case EWindowMode::Windowed:
		{
			glfwRestoreWindow(hWnd);
			break;
		}
		case EWindowMode::WindowedFullscreen:
		{
			glfwMaximizeWindow(hWnd);
			break;
		}
		case EWindowMode::Minimized:
		{
			glfwIconifyWindow(hWnd);
			break;
		}
		case EWindowMode::Fullscreen:
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(hWnd, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			break;
		}
		}

		m_Data.Mode = mode;
	}

	void Window::UpdateCachedMode()
	{
		m_Data.CachedOnMinimizeMode = GetWindowMode();
	}

	Window::WindowData& Window::GetUserPointer(GLFWwindow* hndl)
	{
		return *(WindowData*)glfwGetWindowUserPointer(hndl);
	}

	// TODO: Make Create function
	void Window::Init(const WindowConfig& config)
	{
		VM_CORE_INFO("Creating Window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

		if (s_WindowCount == 0)
		{
			int succeed = glfwInit();
			VM_CORE_ASSERT(succeed, "Could not initialize GLFW"); // TODO: Make verify
			VM_CORE_INFO("[GLFW] GLFW Init");

			glfwSetErrorCallback(glfw_error_callback);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		if (m_Data.CustomTitlebar)
		{
			glfwWindowHint(GLFW_TITLEBAR, false);
			//glfwWindowHint(GLFW_DECORATED, false);
			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
		}

		// Can be used later
		//GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		//const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
		//int monitorX, monitorY;
		//glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#if defined(VM_DEBUG)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_WindowCount;

		VM_CORE_ASSERT(m_Window, "[GLFW] Failed to create window!");

		glfwShowWindow(m_Window);

		// TODO : Move to graphics context class
		glfwMakeContextCurrent(m_Window);
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VM_CORE_ASSERT(gladStatus, "Failed to initialize Glad!");

		VM_CORE_INFO("OpenGL Info:");
		VM_CORE_INFO("    Vendor: {0}", (char*)glGetString(GL_VENDOR));
		VM_CORE_INFO("    Renderer: {0}", (char*)glGetString(GL_RENDERER));
		VM_CORE_INFO("    Version: {0}", (char*)glGetString(GL_VERSION));

		VM_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "VeiM requires OpenGL version 4.5 or higher");

		SetVSync(true);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, config.WindowResizeable ? GLFW_TRUE : GLFW_FALSE);

		SetEventCallbacks(m_Window);
		SetIcon(config.IconPath);
		SetWindowMode(config.Mode);

		SetRawInput(true);
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::SetIcon(std::filesystem::path iconPath)
	{
		GLFWimage icon;
		int channels;
		if (!iconPath.empty())
		{
			String iconPathStr = iconPath.string();
			icon.pixels = stbi_load((Application::Get().GetEngineResourcePath() / iconPathStr).string().c_str(), &icon.width, &icon.height, &channels, 4);
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(icon.pixels);
		}
	}

	void Window::SetEventCallbacks(GLFWwindow* windowHandle)
	{
		glfwSetWindowSizeCallback(windowHandle, [](GLFWwindow* window, int width, int height)
								  {
									  Window::WindowData& data = GetUserPointer(window);
									  data.Width = width;
									  data.Height = height;
									  data.EventCallback(std::string("Window resize: ") + std::to_string(data.Width) + " | " + std::to_string(data.Height));

									  // Handling the situation when window mode should changed, but no other callbacks were called
									  if (glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
										  Application::Get().GetWindow().SetWindowMode(EWindowMode::WindowedFullscreen);
									  else if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
										  Application::Get().GetWindow().SetWindowMode(EWindowMode::Windowed);
								  });

		glfwSetTitlebarHitTestCallback(m_Window, [](GLFWwindow* window, int x, int y, int* hit)
									   {
										   Window::WindowData& data = GetUserPointer(window);
										   if (data.CustomTitlebar && data.TitlebarHitTest)
										   {
											   *hit = data.TitlebarHitTest();
										   }
									   });
		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
									 {
										 if (iconified)
										 {
											 Window::WindowData& data = GetUserPointer(window);
											 data.CachedOnMinimizeMode = data.Mode;
										 }
										 else
										 {
											 Window::WindowData& data = GetUserPointer(window);
											 Application::Get().GetWindow().SetWindowMode(data.CachedOnMinimizeMode);
										 }
									 });
	}

}