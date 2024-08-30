#pragma once
#include "VeiM/Core/Layer.h"
#include "Titlebar.h"
#include "EditorWindows.h"

namespace VeiM
{
	class EditorLayer : public Layer
	{
	public: 
		EditorLayer();
		void OnAttach() override;
		void OnGUI() override;

		void ImGuiWindowMenu();
		void ImGuiWindowsRender();

	public:
		void ThemeEditorRender(); // TODO: Move somewhere


	private:
		std::shared_ptr<UI::TitleBar> m_TitleBar;
		float m_WindowBorderSize = 1.0f; // TODO: Put in private struct called EditorStyle or something similar
		float m_MinWinSizeX = 100.0f;
		bool show_demo_window = false;
		bool show_another_window = false;
		bool m_ThemeEditor = false;
		ImGuiWindows m_ImGuiWindows;
	};
}