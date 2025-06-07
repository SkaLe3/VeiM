#include "Application/Window.h"

#include "Application/Application.h"
#include "Logging/Log.h"
#include "Misc/Paths.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 
#include <stb_image.h>

namespace VeiM
{

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


	void Window::SetSize(uint32 width, uint32 height)
	{
		m_Data.Width = width;
		m_Data.Height = height;
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

	void Window::SetCachedMode()
	{
		m_Data.Mode = m_Data.CachedOnMinimizeMode;
	}

	Window::WindowData& Window::GetUserPointer(GLFWwindow* hndl)
	{
		return *(WindowData*)glfwGetWindowUserPointer(hndl);
	}

	// TODO: Make Create function
	void Window::Init(const WindowConfig& config)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_WindowCount;

		VM_CORE_ASSERT(m_Window, "[GLFW] Failed to create window!");

		glfwShowWindow(m_Window);
		// TODO : Move to graphics context class
		glfwMakeContextCurrent(m_Window);

		SetVSync(config.VSync);

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
			icon.pixels = stbi_load((Paths::EngineContentDir() / iconPath).string().c_str(), &icon.width, &icon.height, &channels, 4);
			glfwSetWindowIcon(m_Window, 1, &icon);
			stbi_image_free(icon.pixels);
		}
	}

	void Window::SetEventCallbacks(GLFWwindow* windowHandle)
	{
		glfwSetMonitorCallback([](GLFWmonitor* monitor, int event)
			{
				Window::WindowData& data = GetUserPointer(Application::Get().GetWindow().GetNativeWindow());
				data.InputDelegates.MonitorCallback(monitor, event);
			});
		glfwSetWindowSizeCallback(windowHandle, [](GLFWwindow* window, int width, int height)
			{
				Window::WindowData& data = GetUserPointer(window);
				WindowSizeEventInternal* newSizeEvent = new WindowSizeEventInternal();
				newSizeEvent->NativeWindow = window;
				newSizeEvent->Width = width;
				newSizeEvent->Height = height;
				data.EventCallback(newSizeEvent);
			});

		glfwSetWindowCloseCallback(windowHandle, [](GLFWwindow* window)
			{
				Window::WindowData& data = GetUserPointer(window);
				WindowCloseEventInternal* newCloseEvent = new WindowCloseEventInternal();
				newCloseEvent->NativeWindow = window;
				data.EventCallback(newCloseEvent);
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
				Window::WindowData& data = GetUserPointer(window);
				data.Mode;
				WindowIconifyEventInternal* newIconEvent = new WindowIconifyEventInternal();
				newIconEvent->NativeWindow = window;
				newIconEvent->Iconified = iconified;
				data.EventCallback(newIconEvent);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Window::WindowData& data = GetUserPointer(window);
				KeyEventInternal* newKeyEvent = new KeyEventInternal();
				newKeyEvent->NativeWindow = window;
				newKeyEvent->Action = action;
				newKeyEvent->KeyCode = key;
				newKeyEvent->ScanCode = scancode;
				data.EventCallback(newKeyEvent);
				data.InputDelegates.KeyCallback(window, key, scancode, action, mods);
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32 keycode)
			{
				Window::WindowData& data = GetUserPointer(window);
				KeyTypeEventInternal* newCharEvent = new KeyTypeEventInternal();
				newCharEvent->NativeWindow = window;
				newCharEvent->TypeKeyCode = keycode;
				data.EventCallback(newCharEvent);
				data.InputDelegates.CharCallback(window, keycode);
			});
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int32 button, int32 action, int32 mods)
			{
				Window::WindowData& data = GetUserPointer(window);
				MouseButtonEventInternal* newButtonEvent = new MouseButtonEventInternal();
				newButtonEvent->NativeWindow = window;
				newButtonEvent->Action = action;
				newButtonEvent->ButtonCode = button;
				data.EventCallback(newButtonEvent);
				data.InputDelegates.MouseButtonCallback(window, button, action, mods);
			});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				Window::WindowData& data = GetUserPointer(window);
				MouseScrollEventInternal* newScrollEvent = new MouseScrollEventInternal();
				newScrollEvent->NativeWindow = window;
				newScrollEvent->XOffset = xOffset;
				newScrollEvent->YOffset = yOffset;
				data.EventCallback(newScrollEvent);
				data.InputDelegates.ScrollCallback(window, xOffset, yOffset);
			});
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				Window::WindowData& data = GetUserPointer(window);
				MouseMoveEventInternal* newMoveEvent = new MouseMoveEventInternal();
				newMoveEvent->NativeWindow = window;
				newMoveEvent->XPos = xPos;
				newMoveEvent->YPos = yPos;
				data.EventCallback(newMoveEvent);
				data.InputDelegates.CursorPosCallback(window, xPos, yPos);
			});
		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				Window::WindowData& data = GetUserPointer(window);
				data.InputDelegates.WindowFocusCallback(window, focused);
			});
		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered)
		{
			Window::WindowData& data = GetUserPointer(window);
			data.InputDelegates.CursorEnterCallback(window, entered);
		});

		// Use refresh callback to draw while resizing
	}

}