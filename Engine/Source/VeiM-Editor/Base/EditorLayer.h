#pragma once
#include "VeiM/Core/CoreDefines.h"
#include "VeiM/Core/Layer.h"
#include "Titlebar.h"
#include "EditorWindows.h"
#include <glm/glm.hpp>


// TEMPORARY
#include "VeiM/Test/FrameBuffer.h"

#include "ProjectBrowser.h"


namespace VeiM
{
	class EditorLayer : public Layer
	{
	public: 
		EditorLayer();
		void OnAttach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI() override;

		void ImGuiWindowMenu();
		void ImGuiWindowsRender();

	public:
		void ThemeEditorRender(); // TODO: Move somewhere
		void CreateTitleBar();


	private:
		std::shared_ptr<UI::TitleBar> m_TitleBar;
		float m_WindowBorderSize = 1.0f; // TODO: Put in private struct called EditorStyle or something similar
		float m_MinWinSizeX = 100.0f;
		bool show_demo_window = false;
		bool show_another_window = false;
		bool m_ThemeEditor = false;
		ImGuiWindows m_ImGuiWindows;


		// Temp
		ProjectBrowserWindow m_ProjectBrowser;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_ViewportSize;


		// Temp
		FrameBuffer m_Framebuffer;
		int display_w, display_h;
		uint32 triangleShader;
		uint32 quadShader;
		uint32 triangleVAO, triangleVBO;
		uint32 quadVAO, quadVBO;

	};
}