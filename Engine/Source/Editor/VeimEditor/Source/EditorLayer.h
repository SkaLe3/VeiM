#pragma once
#include "CoreDefines.h"
#include "Application/Layer.h"
#include "Widgets/Titlebar.h"
#include "ThirdParty/EditorWindows.h"
#include "GameProject/ProjectBrowser.h"
#include "LevelEditor/LevelEditor.h"

#include <glm/glm.hpp>



namespace VeiM
{
	extern class EditorLayer* g_EditorLayer;

	class EditorLayer : public Layer, public Widget
	{
	public:
		EditorLayer();
		void OnAttach() override;
		void OnDetach() override;
		virtual void OnUpdateGUI() override;
		virtual bool OnGUI() override;

		virtual bool OnKeyType(const InputKeyTypeEvent& keyTypeEvent) override;
		virtual bool OnKeyDown(const InputKeyEvent& keyEvent)  override;
		virtual bool OnKeyUp(const InputKeyEvent& keyEvent)  override;
		virtual bool OnMouseUp(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseDown(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseDoubleClick(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseMove(const InputMouseEvent& mouseEvent)  override;
		virtual bool OnMouseWheel(const InputMouseEvent& mouseEvent)  override;

		virtual void OnFinishInput() override;


		void ImGuiWindowMenu();
		void ImGuiWindowsRender();

	public:
		void ThemeEditorRender(); // TODO: Move somewhere
		void CreateTitleBar();

		void Close();
		static EditorLayer& Get() { return *g_EditorLayer; }
	private:
		void RenderClassRegistry();
		void RenderGCInfo();

	private:
		SharedPtr<UI::TitleBar> m_TitleBar;
		float m_WindowBorderSize = 1.0f; // TODO: Put in private struct called EditorStyle or something similar
		float m_MinWinSizeX = 100.0f;
		bool m_bOpenThemeEditor = false;
		bool m_bOpenGCInfo = true;
		bool m_bOpenClassRegistry = false;
		ImGuiWindows m_ImGuiWindows;

		UniquePtr<ProjectBrowser> m_ProjectBrowser;
		UniquePtr<LevelEditor> m_LevelEditor;

	};
}