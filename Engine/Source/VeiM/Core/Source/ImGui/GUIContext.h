#pragma once

#include "Application/Window.h"
#include "Application/Layer.h"

#include "UI/Theme.h"


extern void(*g_CustomMouseButtonCallback)(void*, int, int, int);
extern void(*g_CustomMouseScrollCallback)(void*, double, double);
extern void(*g_CustomKeyCallback)(void*, int, int, int, int);
extern void(*g_CustomWindowFocusCallback)(void*, int);
extern void(*g_CustomCursorPosCallback)(void*, double, double);
extern void(*g_CustomCursorEnterCallback)(void*, int);
extern void(*g_CustomCharCallback)(void*, unsigned int);

namespace VeiM
{
	struct GUIDebug
	{
		float FlashColorTime = 0.0f;
		ImVec4* FlashColor4 = nullptr;
		ImVec4 FlashColor4Backup = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImU32* FlashColorU32 = nullptr;
		ImU32 FlashColorU32Backup = IM_COL32(255, 255, 255, 255);
	};


	class CORE_API GUIContext : public Layer
	{
	public:
		GUIContext();

		// Layer interface override
	public:
		void OnAttach() override;
		void OnDetach() override;

	public:
		void EnableDocking(Window&);
		void RenderDockspace();
		void BeginFrame();
		void EndFrame();
		void BlockEvents(bool block) { m_bBlockEvents = block; }
		bool IsBlockingEvents() const { return m_bBlockEvents; }

		static ImGuiContext* GetImGuiContext();

	public:
		UI::Theme& GetTheme() { return m_Theme; }
		inline void SetTheme(const UI::Theme& theme) { m_Theme = theme; }
		void UpdateTheme();

		GUIDebug& GetDebug() { return m_Debug; }

	private:
		UI::Theme m_Theme;
		GUIDebug m_Debug;
		String m_ConfigFilename;

		bool m_bBlockEvents = true;

	public:
		inline void SetGUICustomCallbacks(
			void(*mouseButton)(void*, int, int, int) = nullptr,
			void(*scroll)(void*, double, double) = nullptr,
			void(*key)(void*, int, int, int, int) = nullptr,
			void(*focus)(void*, int) = nullptr,
			void(*cursorPos) (void*, double, double) = nullptr,
			void(*cursorEnter)(void*, int) = nullptr,
			void(*charCallback) (void*, unsigned int) = nullptr
		) {
			g_CustomMouseButtonCallback = mouseButton;
			g_CustomMouseScrollCallback = scroll;
			g_CustomKeyCallback = key;
			g_CustomWindowFocusCallback = focus;
			g_CustomCursorPosCallback = cursorPos;
			g_CustomCursorEnterCallback = cursorEnter;
			g_CustomCharCallback = charCallback;
		}
	};
}

