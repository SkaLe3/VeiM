#pragma once

#include "VeiM/IO/Log.h"

#include <string>
#include <functional>
#include <filesystem>

struct GLFWwindow;


namespace VeiM
{
	static uint8 s_WindowCount = 0;

	// TODO: change name to EventCallbackFN
	using WindowEventCallback = std::function<void(const std::string&)>;
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


	class Window
	{
	public:
		Window(const WindowConfig& config);
		~Window() { Shutdown(); }

		void SwapBuffers();
		void PollEvents();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }

		void SetVSync(bool enabled);
		bool IsVSyncEnabled() const;

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

		void SetWindowMode(EWindowMode mode);
		void UpdateCachedMode();
		EWindowMode GetChachedMode() const { return m_Data.CachedOnMinimizeMode; }
		EWindowMode GetWindowMode() const { return m_Data.Mode; }

	private:
		void Init(const WindowConfig& config);
		void Shutdown();

		void SetIcon(std::filesystem::path iconPath);
		void SetEventCallbacks(GLFWwindow* windowHandle);



	private:
		struct WindowData
		{
			std::string Title = "VeiM Engine";
			uint32_t Width = 1280;
			uint32_t Height = 720;
			bool VSync = true;
			bool CustomTitlebar = true;

			EWindowMode Mode = EWindowMode::Windowed;
			EWindowMode CachedOnMinimizeMode = EWindowMode::Windowed;

			WindowEventCallback EventCallback = [](const std::string&) {};
			TitlebarHitTestCallback TitlebarHitTest = []() { return false; };
		};
		WindowData m_Data;
		static WindowData& GetUserPointer(GLFWwindow* hndl);

		GLFWwindow* m_Window;
		// TODO: Graphics context

		static inline uint8 s_WindowCount = 0;
	};
}