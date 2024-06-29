#pragma once

#include "VeiM/Image.h"
#include "VeiM/UI/Titlebar.h"
#include "VeiM/UI/UI.h"

#include <string>

struct GLFWwindow;


namespace VeiM
{
	struct ApplicationSpecification;

	class Window
	{
	public:
		Window(ApplicationSpecification& specs);
		~Window() { Shutdown(); }

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }

		void SetVSync(bool enabled);
		bool IsVSync() const;

		bool IsMaximized() const;

		GLFWwindow* GetNativeWindow() const;

		UI::TitleBar& GetTitlebar() { return *m_TitleBar; }
		ApplicationSpecification& GetSpecifications() { return m_Specification; }

		void DrawBase();

	private:
		void Init();
		void Shutdown();

	private:
		ApplicationSpecification& m_Specification;
		GLFWwindow* m_Window;
		// Graphics context
		struct WindowData
		{
			std::string Title = "";
			uint32_t Width = 0, Height = 0;

			bool VSync;
		};

		WindowData m_Data;

	private:
		std::shared_ptr<UI::TitleBar> m_TitleBar;

	};
}