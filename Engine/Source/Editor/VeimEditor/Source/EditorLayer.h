#pragma once
#include "CoreDefines.h"
#include "Application/Layer.h"
#include "Widgets/Titlebar.h"
#include "ThirdParty/EditorWindows.h"
#include "GameProject/ProjectBrowser.h"

#include <glm/glm.hpp>



namespace VeiM
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI() override;

		void ImGuiWindowMenu();
		void ImGuiWindowsRender();

	public:
		void ThemeEditorRender(); // TODO: Move somewhere
		void CreateTitleBar();

	private:
		void TestClassMetadataDisplay();

	private:
		std::shared_ptr<UI::TitleBar> m_TitleBar;
		float m_WindowBorderSize = 1.0f; // TODO: Put in private struct called EditorStyle or something similar
		float m_MinWinSizeX = 100.0f;
		bool show_demo_window = false;
		bool show_another_window = false;
		bool m_ThemeEditor = false;
		ImGuiWindows m_ImGuiWindows;

		// Temp
		UniquePtr<ProjectBrowser> m_ProjectBrowser;
		glm::vec2 m_ViewportBounds[2];
		glm::vec2 m_ViewportSize;

		bool m_bViewportFocused = false;
		bool m_bViewportHovered = false;


		// Temp


	};
}