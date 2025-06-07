#pragma once

#include "Logging/Log.h"
#include "Types/Delegate.h"

#include <glm/glm.hpp>

#include <string>
#include <functional>
#include <filesystem>



struct GLFWwindow;


namespace VeiM
{
	DECLARE_EVENT_2p(ForwardInput_WindowFocusDelegate, void*, int);
	DECLARE_EVENT_2p(ForwardInput_CursorEnterDelegate, void*, int);
	DECLARE_EVENT_3p(ForwardInput_CursorPosDelegate, void*, double, double);
	DECLARE_EVENT_4p(ForwardInput_MouseButtonDelegate, void*, int, int, int);
	DECLARE_EVENT_3p(ForwardInput_ScrollDelegate, void*, double, double);
	DECLARE_EVENT_5p(ForwardInput_KeyDelegate, void*, int, int, int, int);
	DECLARE_EVENT_2p(ForwardInput_CharDelegate, void*, unsigned int);
	DECLARE_EVENT_2p(ForwardInput_MonitorDelegate, void*, int);
	
	class InputEventInternal;

	static uint8 s_WindowCount = 0;

	// TODO: change name to EventCallbackFN
	using WindowEventCallback = std::function<void(InputEventInternal*)>;
	using TitlebarHitTestCallback = std::function<bool()>;

	// TODO: Make enum helper class and macros to convert enums to strings	
	enum class EWindowMode : uint8_t
	{
		Windowed = 0,
		WindowedFullscreen,
		Minimized,
		Fullscreen
	};

	FORCEINLINE const char* LexToString(EWindowMode windowMode)
	{
		switch (windowMode)
		{
		case EWindowMode::Fullscreen: return "Fullscreen";
		case EWindowMode::WindowedFullscreen: return "WindowedFullscreen";
		case EWindowMode::Windowed: return "Windowed";
		case EWindowMode::Minimized: return "Minimized";
		default: return "Unknown";
		}
	}

	// Window creation configuration
	struct WindowConfig
	{
		std::string Title = "VeiM Engine";
		uint32_t Width = 1280;
		uint32_t Height = 720;
		bool VSync = true;
		bool CustomTitlebar = true;
		bool WindowResizeable = true;
		std::filesystem::path IconPath;

		EWindowMode Mode = EWindowMode::Windowed;

		// TODO: WindowEventCallback
	};


	class CORE_API Window
	{
	public:
		Window(const WindowConfig& config);
		~Window() { Shutdown(); }

		void SwapBuffers();
		void PollEvents();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		glm::vec2 GetScreenSize() const;

		void SetVSync(bool enabled);
		bool IsVSyncEnabled() const;
		void Hide();
		void Show();

		void SetRawInput(bool enabled);	// TODO: Always enable for editor, make as option for games
		bool IsRawInputEnabled() const;

		inline void SetEventCallback(const WindowEventCallback& callback)
		{
			m_Data.EventCallback = callback;
		}

		inline void SetTitlebarHitTestCallback(const TitlebarHitTestCallback& callback)
		{
			m_Data.TitlebarHitTest = callback;
		}

		GLFWwindow* GetNativeWindow() const;
		bool HasCustomTitlebar() const { return m_Data.CustomTitlebar; }

		void SetSize(uint32 width, uint32 height);
		void SetWindowMode(EWindowMode mode);
		void UpdateCachedMode();
		void SetCachedMode();
		EWindowMode GetChachedMode() const { return m_Data.CachedOnMinimizeMode; }
		EWindowMode GetWindowMode() const { return m_Data.Mode; }


	private:
		void Init(const WindowConfig& config);
		void Shutdown();

		void SetIcon(std::filesystem::path iconPath);
		void SetEventCallbacks(GLFWwindow* windowHandle);

	public:
		struct ForwardInputDelegates
		{
			ForwardInput_WindowFocusDelegate WindowFocusCallback;
			ForwardInput_CursorEnterDelegate CursorEnterCallback;
			ForwardInput_CursorPosDelegate CursorPosCallback;
			ForwardInput_MouseButtonDelegate MouseButtonCallback;
			ForwardInput_ScrollDelegate ScrollCallback;
			ForwardInput_KeyDelegate KeyCallback;
			ForwardInput_CharDelegate CharCallback;
			ForwardInput_MonitorDelegate MonitorCallback;
		};
		Window::ForwardInputDelegates& GetForwardInputDelegates() { return m_Data.InputDelegates; }
	private:
		struct WindowData
		{
			String Title = "VeiM Engine";
			uint32 Width = 1280;
			uint32 Height = 720;
			bool VSync = true;
			bool CustomTitlebar = true;
			bool RawInputEnabled = false;

			EWindowMode Mode = EWindowMode::Windowed;
			EWindowMode CachedOnMinimizeMode = EWindowMode::Windowed;

			WindowEventCallback EventCallback = [](InputEventInternal*) {};
			TitlebarHitTestCallback TitlebarHitTest = []() { return false; };

			ForwardInputDelegates InputDelegates;
		};
		WindowData m_Data;
		static WindowData& GetUserPointer(GLFWwindow* hndl);

		GLFWwindow* m_Window;

		static inline uint8 s_WindowCount = 0;


	};
}