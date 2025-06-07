#pragma once

#ifdef VM_WITH_EDITOR
#include "ImGui/GUIContext.h"
#include "UI/Image.h"
#endif

#include "CoreDefines.h"
#include "Application/Window.h"
#include "LayerStack.h"

#include "Windows/WindowsInputInterface.h"
#include "Input/Input.h"

#include <filesystem>
#include <string>
#include <functional>
#include <memory>


// TEMPORARY
#include "Test/FrameBuffer.h"
#include "Test/TestRenderer.h"
#include "Renderer/EditorCamera.h"
#include "Engine/CoreObject.h"

namespace VeiM
{
	class World;
	class InputEvent;
	class InputKeyEvent;
	class InputKeyTypeEvent;
	class InputMouseEvent;


	extern CORE_API bool s_EditorCameraEnabled;
	extern CORE_API void mouse_callback(double x, double y);
	extern double s_mx;
	extern double s_my;
}


struct GLFWwindow;

namespace VeiM
{
	extern CORE_API bool g_IsRunning;

	struct CORE_API ApplicationConfig
	{
		std::string Name = "VeiM Engine";
		bool EnableConsole = true;
		std::vector<String> CommandLineArgs;
	};


	struct CORE_API ApplicationSpecification
	{
		ApplicationConfig AppConfig;
		WindowConfig WndConfig;
	};


	class CORE_API Application : public InputHandler
	{
	public:
		FrameBuffer* DebugGetFramebuffer() { return m_PostProcessFramebuffer; }
		uint32 DebugGetFramebufferTexture() { return depthMap; }
	public:
		Application(const ApplicationSpecification& applicationSpecification);
		virtual ~Application();
		static Application& Get();

		void Run();
		void Tick();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close();

		inline float GetDeltaTime() { return m_DeltaTime; }
		Window& GetWindow() const { return *m_Window; }
		ApplicationConfig GetConfig() { return m_Config; }
		std::wstring GetConfiguration(); // TODO: Create enum class and put EnumToString in some file like PlatformMisc
#ifdef VM_WITH_EDITOR
		GUIContext* GetGUIContext() { return m_GUIContext; }
		void ViewportResize(float vwidth, float vheight) { m_Width = vwidth; m_Height = vheight; }

		bool ParseProjectFilePath(String& outProjectFilePath, String& outGameName);

	private:
		void InitGUI();
		void RenderGUI();
#endif
	private:
		void Startup();
		void Shutdown();

		struct ModifierState GetModifiers();
		const glm::vec2& GetCursorPos();
		const glm::vec2& GetLastCursorPos();

		void ProcessInputEvents();
		void ProcessInputEvent(InputEventInternal* inputEvent);
		void FinishInput();

		void OnEvent(InputEventInternal*);
		virtual bool OnKeyType(const uint32 character) override;
		virtual bool OnKeyDown(const int32 keyCode, const int32 scanCode, const bool bRepeat) override;
		virtual bool OnKeyUp(const int32 keyCode, const int32 scanCode) override;
		virtual bool OnMouseDown(const int32 button, const glm::vec2& cursorPos) override;
		virtual bool OnMouseUp(const int32 button, const glm::vec2& cursorPos) override;
		virtual bool OnMouseDoubleClick(const int32 button, const glm::vec2& cursorPos) override;
		virtual bool OnMouseMove(const int32 x, const int32 y) override;
		virtual bool OnMouseWheel(const float offset, const glm::vec2& cursorPos) override;
		virtual bool OnSizeChanged(const int32 width, const int32 height) override;
		virtual bool OnClose() override;
		virtual bool OnMinimized() override;
		virtual bool OnRestored() override;
		// TODO: Add Controller events

		bool HandleKeyTypeEvent(const InputKeyTypeEvent& keyTypeEvent);
		bool HandleKeyDownEvent(const InputKeyEvent& keyEvent);
		bool HandleKeyUpEvent(const InputKeyEvent& keyEvent);
		bool HandleMouseUpEvent(const InputMouseEvent& mouseEvent);
		bool HandleMouseDownEvent(const InputMouseEvent& mouseEvent);
		bool HandleMouseDoubleClickEvent(const InputMouseEvent& mouseEvent);
		bool HandleMouseMoveEvent(const InputMouseEvent& mouseEvent);
		bool HandleMouseWheelEvent(const InputMouseEvent& mouseEvent);

	private:
#ifdef VM_WITH_EDITOR
		GUIContext* m_GUIContext;
#endif
	private:
		ApplicationConfig m_Config;
		std::unique_ptr<Window> m_Window;
		std::vector<InputEventInternal*> m_InputEvents;
		std::unique_ptr<struct ModifierState> m_Modifiers;
		std::unordered_set<Key> m_PressedButtons;
		glm::vec2 m_CursorPos;
		glm::vec2 m_LastCursorPos;

		LayerStack m_LayerStack;

		bool m_Running = true;
		bool m_Minimized = false;

		double m_DeltaTime = 0.0f;
		double m_FrameTime = 0.0f;
		double m_LastFrameTime = 0.0f;


		FrameBuffer* m_Framebuffer;
		FrameBuffer* m_IntermediateFramebuffer;
		FrameBuffer* m_PostProcessFramebuffer;

		IMesh* m_CubeMesh;
		IMesh* m_SphereMesh;
		float cubePitch = 0;
		float cubeYaw = 0;
		float cubeRoll = 0;
		float fov = 45.f;
		float size = 10;
		bool bHasGame = false;

		bool bDrawDepthBuffer = false;
		bool bUseNatureSkybox = true;
		bool bExplode = false;
		bool bUseInstancing = true;
		bool bCastGlobalShadows = true;
		bool bCastPointShadows = true;
		bool bUseSRGB = false;
		bool bUseNormalMaps = true;
		int32 m_ShadowMapLevel = 3;
		uint32 SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
		uint32 pSHADOW_WIDTH = 1024, pSHADOW_HEIGHT = 1024;
		float m_ShadowBiasMin = 0.001f;
		float m_ShadowBiasMax = 0.005f;

		float m_Shininess = 64;

		glm::vec3 m_DirLightAmbient = { 0.2f, 0.2f, 0.2f };
		glm::vec3 m_DirLightDiffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_DirLightSpecular = { 1.f, 1.f, 1.f };
		glm::vec3 m_DirLightDirection = { -0.2f, -1.f, -0.3f };

		glm::vec3 m_PointLightAmbient = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_PointLightDiffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_PointLightSpecular = { 1.f, 1.f, 1.f };

		glm::vec3 m_SpotLightAmbient = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_SpotLightDiffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_SpotLightSpecular = { 1.f, 1.f, 1.f };


		glm::vec3 m_PointLightPos1 = { -1.0f, -20.2f,  0.0f };
		glm::vec3 m_PointLightPos2 = {  1.0f, -20.2f,  0.0f };
		glm::vec3 m_PointLightPos3 = {  0.0f, -20.2f, -1.0f };
		glm::vec3 m_PointLightPos4 = {  0.0f, -20.2f,  1.0f };

		glm::vec3 m_SpotLightPos = { 5.f, 4.0f, 5.0f };
		glm::vec3 m_SpotLightDir = { 0.f, -1.f, 0.f };
		float m_SpotLightInner = 12.5f;
		float m_SpotLightOuter = 15.f;


		uint32 m_Width = 600;
		uint32 m_Height = 400;

		// Shadows
		unsigned int depthMapFBO;
		unsigned int depthMap; // Depth Map for shadow maps
		unsigned int depthCubeMapFBO;
		uint32 depthCubemap;

	private:
		static Application* s_Instance;
	};

	CORE_API void CalculateTime();

	Application* CreateApplication(const std::vector<String>& arguments, const String& name, const String& title);
	
}

