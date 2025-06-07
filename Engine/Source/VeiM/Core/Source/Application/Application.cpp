#include "Application.h"

#ifdef VM_WITH_EDITOR
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#include "HAL/PlatformService.h"
#include "Windows/WindowsUtils.h"
#include "Misc/Paths.h"
#include "Misc/Timer.h"
#include "Logging/Log.h"
#include "Project/ModuleManager.h"
#include "Types/StringID.h"
#include "Misc/Globals.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 

#include "Engine/Reflection.h"
#include "Engine/ObjectPtr.h"
#include "Engine/CoreObject.h"
#include "Engine/World.h"
#include "Engine/GameEngine.h"
#include "Engine/CoreObjectStatics.h"

#include "Renderer/Shader.h"
#include "Renderer/CubeMesh.h"
#include "Renderer/SphereMesh.h"
#include "Renderer/Model.h"
#include "Renderer/Renderer.h"

#include "Input/Input.h"
#include "Input/Events.h"
#include "HAL/PlatformInput.h"
#include "RenderInterface/RenderInterfaceBase.h"

#ifdef VM_WITH_EDITOR
#include "Editor/Editor.h"
#include "Engine/Classes/Editor/EditorEngine.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp> 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <sstream>

// Temporary static functions
namespace VeiM
{
	CORE_API bool g_IsRunning = false;

}


namespace VeiM
{
	UniquePtr<EditorCamera> m_Camera;
#ifdef VM_WITH_EDITOR
	GLFWwindow* focusedWnd = nullptr;

	bool s_EditorCameraEnabled = true;
	double s_mx = 0;
	double s_my = 0;


	bool firstMouse = true;
	float lastX = 0;
	float lastY = 0;



	bool cursorHidden = false;
	bool switched = false;
	bool pan = false;

	void process_input(bool bDownRight, bool bUpRight, bool bDownMiddle, bool bUpMiddle)
	{
		if (s_EditorCameraEnabled)
		{
			switched = true;
			if (bDownRight)
			{
				cursorHidden = true;
			}
			if (bUpRight)
			{
				cursorHidden = false;
			}
			if (bDownMiddle)
			{
				pan = true;
			}
			if (bUpMiddle)
			{
				pan = false;
			}
		}
		if (cursorHidden && switched)
		{
			glfwSetInputMode(focusedWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (switched)
		{
			glfwSetInputMode(focusedWnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		switched = false;
		firstMouse = true;
	}
	void process_movement()
	{
		GLFWwindow* window = focusedWnd;
		if (!cursorHidden)
			return;
		glm::vec3 direction = { 0.f, 0.f, 0.f };

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			direction += m_Camera->GetForwardVector();
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			direction += -m_Camera->GetForwardVector();
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			direction += -m_Camera->GetRightVector();
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			direction += m_Camera->GetRightVector();
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			direction += glm::vec3(0.0f, 1.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			direction -= glm::vec3(0.0f, 1.0f, 0.0f);

		if (direction != glm::vec3{ 0.f, 0.f, 0.f })
			direction = glm::normalize(direction);
		m_Camera->InputKey(Application::Get().GetDeltaTime(), direction);
	}

	void mouse_callback(double xposIn, double yposIn)
	{
		static bool  justEnabled = true;
		if (!s_EditorCameraEnabled && !switched)
		{
			cursorHidden = true;
			if (justEnabled)
			{
				glfwSetInputMode(Application::Get().GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
				s_mx = ImGui::GetMousePos().x;
				s_mx = ImGui::GetMousePos().y;
			}
			justEnabled = false;
		}


		if (firstMouse)
		{
			xposIn = 0;
			yposIn = 0;
			firstMouse = false;
		}

		if (pan)
		{
			m_Camera->InputPan({ xposIn, yposIn });
		}
		else
			if (cursorHidden)
				m_Camera->InputMouse({ xposIn, yposIn });

	}

#endif
}

namespace VeiM
{
	Application* Application::s_Instance = nullptr;
	Application::Application(const ApplicationSpecification& applicationSpecification)
		: m_Config(applicationSpecification.AppConfig)
	{
		VM_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		PlatformService::SetCurrentWorkingDirectoryToBaseDir();

		IKey::Initialize();
		InitRenderer();

		m_Window = MakeUnique<Window>(applicationSpecification.WndConfig);  // TODO: Make static function Create() or smth
		m_Window->SetEventCallback([this](InputEventInternal* inputEvent) { Application::OnEvent(inputEvent); });

		g_IsRunning = true;

	}

	Application::~Application()
	{
		Shutdown();
		s_Instance = nullptr;
	}
	std::wstring Application::GetConfiguration()
	{
#if defined(VM_DEBUG)
		return TEXT("Debug");
#elif defined(VM_DEVELOPMENT)
		return TEXT("Development");
#elif defined(VM_SHIPPING)
		return TEXT("Shipping");
#endif
	}

#ifdef VM_WITH_EDITOR

	bool Application::ParseProjectFilePath(String& outProjectFilePath, String& outGameName)
	{
		std::vector<String>& cmdArgs = m_Config.CommandLineArgs;

		return std::any_of(cmdArgs.begin(), cmdArgs.end(), [&](const String& item)
			{
				std::filesystem::path path(item);
				bool hasProjectFilePath = path.has_extension() && path.extension() == ".vmproject";
				if (hasProjectFilePath)
				{
					outProjectFilePath = path.string();
					outGameName = path.stem().string();
				}
				return hasProjectFilePath;
			});
	}

	void Application::InitGUI()
	{
		m_GUIContext = new GUIContext();
		m_GUIContext->SetGUICustomCallbacks(
			[](void* wnd, int button, int action, int mods)
			{if (wnd == Application::Get().GetWindow().GetNativeWindow())return; MouseButtonEventInternal* be = new MouseButtonEventInternal(); be->NativeWindow = (GLFWwindow*)wnd; be->Action = action, be->ButtonCode = button; Application::Get().OnEvent(be); },
			[](void* wnd, double a, double b)
			{if (wnd == Application::Get().GetWindow().GetNativeWindow())return; MouseScrollEventInternal* se = new MouseScrollEventInternal(); se->NativeWindow = (GLFWwindow*)wnd; se->XOffset = a; se->YOffset = b;  Application::Get().OnEvent(se); },
			[](void* wnd, int key, int scancode, int action, int mods)
			{if (wnd == Application::Get().GetWindow().GetNativeWindow())return; KeyEventInternal* ke = new KeyEventInternal(); ke->NativeWindow = (GLFWwindow*)wnd; ke->Action = action; ke->KeyCode = key; ke->ScanCode = scancode; Application::Get().OnEvent(ke); },
			[](void* wnd, int a)
			{},
			[](void* wnd, double x, double y)
			{if (wnd == Application::Get().GetWindow().GetNativeWindow())return; MouseMoveEventInternal* me = new MouseMoveEventInternal(); me->NativeWindow = (GLFWwindow*)wnd; me->XPos = x; me->YPos = y; Application::Get().OnEvent(me); },
			[](void* wnd, int a)
			{},
			[](void* wnd, unsigned int c)
			{if (wnd == Application::Get().GetWindow().GetNativeWindow())return; KeyTypeEventInternal* ce = new KeyTypeEventInternal();  ce->NativeWindow = (GLFWwindow*)wnd; ce->TypeKeyCode = c; Application::Get().OnEvent(ce); }
		);
		PushOverlay(m_GUIContext);
	}

	void Application::RenderGUI()
	{

		m_GUIContext->BeginFrame();
		m_GUIContext->EnableDocking(*m_Window);
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdateGUI();
		}

		if (bHasGame)
		{


			ImGui::Begin("Test Renderer");

			ImGui::SeparatorText("General");
			ImGui::Checkbox("Draw Depth Buffer", &bDrawDepthBuffer);
			ImGui::Checkbox("Nature Skybox", &bUseNatureSkybox);
			ImGui::Checkbox("Explode", &bExplode);
			ImGui::Checkbox("Use Instancing", &bUseInstancing);
			ImGui::Checkbox("Use NormalMaps", &bUseNormalMaps);
			ImGui::Checkbox("Use SRGB", &bUseSRGB);
			bool bUseHDR = TestRenderer::Settings.bHDREnabled;
			if (ImGui::Checkbox("Use HDR", &bUseHDR))
			{
				TestRenderer::Settings.bHDREnabled = bUseHDR;
				TestRenderer::Settings.bMainFramebufferDirty = true;
			}
			ImGui::SliderFloat("Exposure", &TestRenderer::Settings.Exposure, 0.1f, 10.f, "%0.1f");
			ImGui::SliderFloat("Gamma", &TestRenderer::Settings.GammaCorrection, 0.1f, 4.f, "%0.1f");


			if (ImGui::SliderInt("Shadow maps level", &m_ShadowMapLevel, 0, 3))
			{
				SHADOW_WIDTH = SHADOW_HEIGHT = 512 * glm::pow(2, m_ShadowMapLevel);
				// Generate depth map

				glDeleteFramebuffers(1, &depthMapFBO);
				glDeleteTextures(1, &depthMap);

				glGenFramebuffers(1, &depthMapFBO);
				glGenTextures(1, &depthMap);
				glBindTexture(GL_TEXTURE_2D, depthMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

				// Attach depth map to framebuffer
				glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			//ImGui::SliderFloat("Shadow Bias", &m_ShadowBiasMin, 0.0001f, 0.001f, "%.4f");

			/* AA OPTIONS */
			static const char* aaOptions[] = {
				"No AA",
				"MSAA 2x",
				"MSAA 4x",
				"MSAA 8x",
				"MSAA 16x"
			};

			uint8 currentAAIndex = TestRenderer::Settings.bAntiAliasingQuality;

			if (ImGui::BeginCombo("Anti-Aliasing Mode", aaOptions[currentAAIndex]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(aaOptions); n++)
				{
					bool isSelected = (currentAAIndex == n);
					if (ImGui::Selectable(aaOptions[n], isSelected))
					{
						TestRenderer::Settings.bMainFramebufferDirty = 1;
						TestRenderer::Settings.bAntiAliasingQuality = n;
						currentAAIndex = TestRenderer::Settings.bAntiAliasingQuality;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			/* END AA OPTIONS*/


			ImGui::SeparatorText("Cube Controls");
			ImGui::SliderFloat("Cube Pitch", &cubePitch, 0, 360, "%.0f");
			ImGui::SliderFloat("Cube Yaw", &cubeYaw, 0, 360, "%.0f");
			ImGui::SliderFloat("Cube Roll", &cubeRoll, 0, 360, "%.0f");

			ImGui::SliderFloat("Shininess", &m_Shininess, 1.f, 128.f, "%.0f");

			ImGui::SeparatorText("Camera");
			const char* projectionTypes[] = { "Perspective", "Orthographic" };
			int currentProjectionIndex = static_cast<int>(m_Camera->GetProjectionType());

			if (ImGui::Combo("Projection", &currentProjectionIndex, projectionTypes, IM_ARRAYSIZE(projectionTypes)))
			{
				m_Camera->SetProjectionType(static_cast<Camera::ProjectionType>(currentProjectionIndex));
				m_Camera->SetOrthoHeight(size);
			}

			const char* fovAxis[] = { "Horizontal", "Vertical" };
			int currentAxis = static_cast<int>(m_Camera->GetFovAxis());

			if (ImGui::Combo("FOV Axis", &currentAxis, fovAxis, IM_ARRAYSIZE(fovAxis)))
			{
				m_Camera->SetFovAxis(static_cast<Camera::FOVAxis>(currentAxis));
			}

			if (ImGui::SliderFloat("FOV", &fov, 10, 100, "%.0f"))
			{
				m_Camera->SetFov(fov);
			}
			if (ImGui::SliderFloat("Ortho Size", &size, 1, 20, "%.0f"))
			{
				m_Camera->SetOrthoHeight(size);
			}
			ImGui::SliderFloat("Camera Speed", &m_Camera->MovementSpeed(), 1.f, 25.f, "%.0f");
			ImGui::SliderFloat("Camera Sensitivity", &m_Camera->Sensitivity(), 0.05f, 1.f, "%.2f");

			ImGui::SeparatorText("Directional Lighting");
			ImGui::SliderFloat3("Ambient Color D", glm::value_ptr(m_DirLightAmbient), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Diffuse Color D", glm::value_ptr(m_DirLightDiffuse), 0.0f, 100.0f, "%.3f");
			ImGui::SliderFloat3("Specular Color D", glm::value_ptr(m_DirLightSpecular), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Direction D", glm::value_ptr(m_DirLightDirection), -1.0f, 1.0f, "%.2f");
			ImGui::Checkbox("Cast Shadows D", &bCastGlobalShadows);

			ImGui::SeparatorText("Point Lights");
			ImGui::SliderFloat3("Ambient Color P", glm::value_ptr(m_PointLightAmbient), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Diffuse Color P", glm::value_ptr(m_PointLightDiffuse), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Specular Color P", glm::value_ptr(m_PointLightSpecular), 0.0f, 1.0f, "%.3f");
			ImGui::DragFloat3("Light 1 Position", glm::value_ptr(m_PointLightPos1), 0.1f);
			ImGui::Checkbox("Cast Shadows P", &bCastPointShadows);
			ImGui::DragFloat3("Light 2 Position", glm::value_ptr(m_PointLightPos2), 0.1f);
			ImGui::DragFloat3("Light 3 Position", glm::value_ptr(m_PointLightPos3), 0.1f);
			ImGui::DragFloat3("Light 4 Position", glm::value_ptr(m_PointLightPos4), 0.1f);

			ImGui::SeparatorText("Spot Lights");
			ImGui::SliderFloat3("Ambient Color S", glm::value_ptr(m_SpotLightAmbient), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Diffuse Color S", glm::value_ptr(m_SpotLightDiffuse), 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("Specular Color S", glm::value_ptr(m_SpotLightSpecular), 0.0f, 1.0f, "%.3f");
			ImGui::DragFloat3("Position S", glm::value_ptr(m_SpotLightPos), 0.1f);
			ImGui::SliderFloat3("Direction S", glm::value_ptr(m_SpotLightDir), -1.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Inner", &m_SpotLightInner, 1.f, 180.f, "%.0f");
			ImGui::SliderFloat("Outer", &m_SpotLightOuter, 1.f, 180.f, "%.0f");

			ImGui::End();

		}

		m_GUIContext->EndFrame();
	}
#endif

	void Application::Startup()
	{
#ifdef VM_WITH_EDITOR
		g_IsEditor = true;
		std::vector<String> cmdArgs = m_Config.CommandLineArgs;
		if (!cmdArgs.empty())
		{
			Paths::SetProjectFilePath(cmdArgs[0]);
			ModuleManager::Get().SetGameBianariesDir(Paths::ProjectDir() / "Binaries" / "Win64");
			ModuleManager::Get().LoadModule(Paths::GetProjectFilePath().stem().wstring());
			ModuleManager::Get().LoadModule(Paths::GetProjectFilePath().stem().wstring());
		}

		InitGUI();
#endif
		ClassRegistry::InitializeReflectionSystem();

		if (!g_IsEditor)
		{
			g_Engine = NewObject<GameEngine>(nullptr, StringID("GameEngine"));

			VM_CORE_INFO("[Engine] Created GameEngine");
		}
		else
		{
#ifdef VM_WITH_EDITOR
			g_Engine = NewObject<Engine>(ClassRegistry::FindClass(StringID("EditorEngine")), nullptr, StringID("EditorEngine"));
			// g_Editor is assigned in EditorEngine::Init()
#endif
		}
		g_Engine->Init();
		g_Engine->Start();

		Renderer::Get()->Startup();
	}


	void Application::Shutdown()
	{
		ShutdownRenderer();
		Renderer::Get()->Shutdown();

		g_IsRunning = false;
	}



	VeiM::ModifierState Application::GetModifiers()
	{
		return *m_Modifiers;
	}

	const glm::vec2& Application::GetCursorPos()
	{
		return m_CursorPos;
	}

	const glm::vec2& Application::GetLastCursorPos()
	{
		return m_LastCursorPos;
	}

	void Application::ProcessInputEvents()
	{
		std::vector<InputEventInternal*> events(m_InputEvents);
		m_InputEvents.clear();
		for (int32 i = 0; i < events.size(); i++)
		{
			ProcessInputEvent(events[i]);
			delete events[i];
		}
	}

	void Application::ProcessInputEvent(InputEventInternal* inputEvent)
	{
		if (!m_Modifiers)
		{
			m_Modifiers = MakeUnique<ModifierState>();
		}

		if (!inputEvent || !inputEvent->NativeWindow)
		{
			return;
		}
#ifdef VM_WITH_EDITOR
		focusedWnd = inputEvent->NativeWindow; // DELETE
#endif
		switch (inputEvent->EventType)
		{
		case EInputEventType::KeyInput:
		{
			KeyEventInternal* currentEvent = static_cast<KeyEventInternal*>(inputEvent);

			bool bIsLockKey = false;
			bool bLockActive = false;
			if (currentEvent->KeyCode == (int32)EKeyCode::CapsLock || currentEvent->KeyCode == (int32)EKeyCode::NumLock)
			{
				bIsLockKey = true;
				if (currentEvent->KeyCode == (int32)EKeyCode::CapsLock)
				{
					bLockActive = m_Modifiers->IsCapsLocked();
				}
				else if (currentEvent->KeyCode == (int32)EKeyCode::NumLock)
				{
					bLockActive = m_Modifiers->IsNumLocked();
				}
			}


			switch (currentEvent->Action)
			{
			case GLFW_RELEASE:
			{
				const bool bKeyState = (bIsLockKey && bLockActive);  // Not lock key: false | Lock key: save current state
				m_Modifiers->UpdateFromKeyEvent(currentEvent->KeyCode, bKeyState);
				const bool result = OnKeyUp(currentEvent->KeyCode, currentEvent->ScanCode);
			}
			break;
			case GLFW_PRESS:
			{
				const bool bKeyState = !bIsLockKey || (bIsLockKey && !bLockActive); // Not lock key: true | Lock key: opposite state 
				m_Modifiers->UpdateFromKeyEvent(currentEvent->KeyCode, bKeyState);
				const bool result = OnKeyDown(currentEvent->KeyCode, currentEvent->ScanCode, false);
			}
			break;
			case GLFW_REPEAT:
			{
				const bool result = OnKeyDown(currentEvent->KeyCode, currentEvent->ScanCode, true);
			}
			break;
			return;
			}
		}
		break;
		case EInputEventType::CharInput:
		{
			KeyTypeEventInternal* currentEvent = static_cast<KeyTypeEventInternal*>(inputEvent);
			const bool result = OnKeyType(currentEvent->TypeKeyCode);
			return;
		}
		break;
		case EInputEventType::MouseButtonInput:
		{
			MouseButtonEventInternal* currentEvent = static_cast<MouseButtonEventInternal*>(inputEvent);
			bool bMouseUp = currentEvent->Action == GLFW_RELEASE ? true : false;
			bool bDoubleClick = false;
			if (!bMouseUp)
			{
				static double lastClickTime = 0.0;
				double currentTime = Time::GetTime();
				constexpr double doubleClickThreshold = 0.25;
				if (currentTime - lastClickTime <= doubleClickThreshold)
				{
					bDoubleClick = true;
					lastClickTime = 0.0;
				}
				else
				{
					lastClickTime = currentTime;
				}
			}
			double xpos, ypos;
			glfwGetCursorPos(inputEvent->NativeWindow, &xpos, &ypos);
			bool result;
			if (bMouseUp)
			{
				result = OnMouseUp(currentEvent->ButtonCode, { xpos, ypos });
			}
			else if (bDoubleClick)
			{
				result = OnMouseDoubleClick(currentEvent->ButtonCode, { xpos, ypos });
			}
			else
			{
				result = OnMouseDown(currentEvent->ButtonCode, { xpos, ypos });
			}
		}
		break;
		case EInputEventType::MouseMoveInput:
		{
			MouseMoveEventInternal* currentEvent = static_cast<MouseMoveEventInternal*>(inputEvent);
			// TODO: Add wrapping handling
			m_CursorPos = { currentEvent->XPos, currentEvent->YPos };
			const bool result = OnMouseMove(currentEvent->XPos, currentEvent->YPos);
			m_LastCursorPos = m_CursorPos;
		}
		break;
		case EInputEventType::MouseScrollInput:
		{
			MouseScrollEventInternal* currentEvent = static_cast<MouseScrollEventInternal*>(inputEvent);
			const float spinFactor = 1 / 120.0f;
			double xpos, ypos;
			glfwGetCursorPos(inputEvent->NativeWindow, &xpos, &ypos);

			const bool result = OnMouseWheel(currentEvent->YOffset * spinFactor, { xpos, ypos });
		}
		break;
		case EInputEventType::WindowSizeInput:
		{
			WindowSizeEventInternal* currentEvent = static_cast<WindowSizeEventInternal*>(inputEvent);
			const bool result = OnSizeChanged(currentEvent->Width, currentEvent->Height);
		}
		break;
		case EInputEventType::WindowCloseInput:
		{
			WindowCloseEventInternal* currentEvent = static_cast<WindowCloseEventInternal*>(inputEvent);
			const bool result = OnClose();
		}
		break;
		case EInputEventType::WindowIconifyInput:
		{
			WindowIconifyEventInternal* currentEvent = static_cast<WindowIconifyEventInternal*>(inputEvent);
			if (currentEvent->Iconified)
			{
				OnMinimized();
			}
			else
			{
				OnRestored();
			}
		}
		break;
		case EInputEventType::InvalidInput:
		default:
			break;
		}

	}

	void Application::FinishInput()
	{
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			(*it)->OnFinishInput();
		}
	}

	bool Application::OnKeyType(const uint32 character)
	{
		InputKeyTypeEvent keyTypeEvent(GetModifiers(), character, false);
		return HandleKeyTypeEvent(keyTypeEvent);
	}

	bool Application::OnKeyDown(const int32 keyCode, const int32 scanCode, const bool bRepeat)
	{
		const Key key = KeyInputManager::Get().GetKeyFromKeyCode(keyCode);
		InputKeyEvent keyEvent(GetModifiers(), key, keyCode, scanCode, bRepeat);
		return HandleKeyDownEvent(keyEvent);
	}

	bool Application::OnKeyUp(const int32 keyCode, const int32 scanCode)
	{
		const Key key = KeyInputManager::Get().GetKeyFromKeyCode(keyCode);
		InputKeyEvent keyEvent(GetModifiers(), key, keyCode, scanCode, false);
		return HandleKeyUpEvent(keyEvent);
	}

	bool Application::OnMouseUp(const int32 button, const glm::vec2& cursorPos)
	{
		Key Key = KeyInputManager::Get().GetKeyFromKeyCode(button);
		InputMouseEvent mouseEvent(GetModifiers(), cursorPos, GetLastCursorPos(), m_PressedButtons, Key, 0);
		return HandleMouseUpEvent(mouseEvent);
	}

	bool Application::OnMouseDown(const int32 button, const glm::vec2& cursorPos)
	{
		Key key = KeyInputManager::Get().GetKeyFromKeyCode(button);
		InputMouseEvent mouseEvent(GetModifiers(), cursorPos, GetLastCursorPos(), m_PressedButtons, key, 0);
		return HandleMouseDownEvent(mouseEvent);
	}

	bool Application::OnMouseDoubleClick(const int32 button, const glm::vec2& cursorPos)
	{
		Key key = KeyInputManager::Get().GetKeyFromKeyCode(button);
		InputMouseEvent mouseEvent(GetModifiers(), cursorPos, GetLastCursorPos(), m_PressedButtons, key, 0);
		return HandleMouseDoubleClickEvent(mouseEvent);
	}

	bool Application::OnMouseMove(const int32 x, const int32 y)
	{
		const glm::vec2 cursorPos = { x, y };
		const glm::vec2 lastCursorPos = GetLastCursorPos();
		bool result = true;
		if (cursorPos != lastCursorPos)
		{
			InputMouseEvent mouseEvent(GetModifiers(), cursorPos, lastCursorPos, m_PressedButtons, IKey::Invalid, 0);
			result = HandleMouseMoveEvent(mouseEvent);
		}
		return result;
	}

	bool Application::OnMouseWheel(const float offset, const glm::vec2& cursorPos)
	{
		InputMouseEvent mouseEvent(GetModifiers(), cursorPos, cursorPos, m_PressedButtons, IKey::Invalid, offset);
		return HandleMouseWheelEvent(mouseEvent);
	}

	bool Application::OnSizeChanged(const int32 width, const int32 height)
	{
		// Handling the situation when window mode should be changed, but no other callbacks were called
		if (glfwGetWindowAttrib(GetWindow().GetNativeWindow(), GLFW_MAXIMIZED))
			Application::Get().GetWindow().SetWindowMode(EWindowMode::WindowedFullscreen);
		else if (!glfwGetWindowAttrib(GetWindow().GetNativeWindow(), GLFW_ICONIFIED))
			Application::Get().GetWindow().SetWindowMode(EWindowMode::Windowed);

		Application::Get().GetWindow().SetSize(width, height);
		// TODO: propagate to imgui
		return true;
	}

	bool Application::OnClose()
	{
		Close();
		return true;
	}

	bool Application::OnMinimized()
	{
		GetWindow().UpdateCachedMode();
		return true;
	}

	bool Application::OnRestored()
	{
		GetWindow().SetCachedMode();
		return true;
	}

	bool Application::HandleKeyTypeEvent(const InputKeyTypeEvent& keyTypeEvent)
	{
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnKeyType(keyTypeEvent))
			{
				return true;
			}
		}
		return false;
	}

	bool Application::HandleKeyDownEvent(const InputKeyEvent& keyEvent)
	{
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnKeyDown(keyEvent))
			{
				return true;
			}
		}
		return false;
	}

	bool Application::HandleKeyUpEvent(const InputKeyEvent& keyEvent)
	{
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnKeyUp(keyEvent))
			{
				return true;
			}
		}
		return false;
	}

	bool Application::HandleMouseDownEvent(const InputMouseEvent& mouseEvent)
	{
#ifdef VM_WITH_EDITOR
		if (mouseEvent.GetButton() == IKey::ButtonRight) process_input(true, false, false, false);
		if (mouseEvent.GetButton() == IKey::ButtonMiddle) process_input(false, false, true, false);
#endif

		m_PressedButtons.insert(mouseEvent.GetButton());
		// Maybe check for imgui drag/drog and skip event if it is active

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnMouseDown(mouseEvent))
			{
				return true;
			}
		}
		return false;
	}

	bool Application::HandleMouseUpEvent(const InputMouseEvent& mouseEvent)
	{
#ifdef VM_WITH_EDITOR
		if (mouseEvent.GetButton() == IKey::ButtonRight) process_input(false, true, false, false);
		if (mouseEvent.GetButton() == IKey::ButtonMiddle) process_input(false, false, false, true);
#endif
		m_PressedButtons.erase(mouseEvent.GetButton());

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnMouseUp(mouseEvent))
			{
				return true;
			}
		}
		return false;
	}



	bool Application::HandleMouseDoubleClickEvent(const InputMouseEvent& mouseEvent)
	{
		m_PressedButtons.insert(mouseEvent.GetButton());
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnMouseDoubleClick(mouseEvent))
			{
				return true;
			}
		}
		return false;
	}

	bool Application::HandleMouseMoveEvent(const InputMouseEvent& mouseEvent)
	{
		if (s_EditorCameraEnabled)
			mouse_callback(mouseEvent.GetOffset().x, mouseEvent.GetOffset().y);

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnMouseMove(mouseEvent))
			{
				return true;
			}
		}
		return false;
	}

	bool Application::HandleMouseWheelEvent(const InputMouseEvent& mouseEvent)
	{
#ifdef VM_WITH_EDITOR
		if (cursorHidden)
			m_Camera->InputScroll({ 0.f, mouseEvent.GetWheelOffset() });
		if (mouseEvent.GetWheelOffset() == 0)
		{
			return false;
		}
#endif
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if ((*it)->OnMouseWheel(mouseEvent))
			{
				return true;
			}
		}
		return false;
	}

	Application& Application::Get()
	{
		return *s_Instance;
	}




	void Application::OnEvent(InputEventInternal* newInput)
	{
		m_InputEvents.push_back(newInput);
	}

	void Application::Run()
	{
		Startup();

		TestRenderer::Init();
		// Compile shaders
		std::vector<String> cmdArgs = m_Config.CommandLineArgs;

		fs::path lightSourceShaderFilename;
		fs::path LitShaderFilename;
		fs::path borderShaderFilename;
		fs::path screenShaderFilename;
		fs::path skyboxShaderFilename;
		fs::path instanceShaderFilename;
		fs::path shadowmapShaderFilename;
		fs::path pointshadowmapShaderFilename;

		fs::path universalShaderFilename;

		fs::path grayTextureFilename;
		fs::path boxDiffuseTextureFilename;
		fs::path boxSpecularTextureFilename;
		fs::path boxEmissionTextureFilename;

		fs::path backpackModelFilename;
		fs::path coinMeshFilename;

		fs::path brickDiffuseFilename;
		fs::path brickNormalFilename;

		std::vector<fs::path> skyboxFilenames;
		std::vector<fs::path> skyboxSFilenames;

		std::vector<String> faces
		{
			"right.jpg",
				"left.jpg",
				"top.jpg",
				"bottom.jpg",
				"front.jpg",
				"back.jpg"
		};

		std::vector<String> facesstylized
		{
			"px.png",
				"nx.png",
				"py.png",
				"ny.png",
				"pz.png",
				"nz.png"
		};


		grayTextureFilename = Paths::EngineContentDir() / "Mess" / "T_Gray.png";
		lightSourceShaderFilename = Paths::EngineContentDir() / "Mess" / "lightsource.glsl";
		LitShaderFilename = Paths::EngineContentDir() / "Mess" / "Lit.glsl";
		borderShaderFilename = Paths::EngineContentDir() / "Mess" / "border.glsl";
		screenShaderFilename = Paths::EngineContentDir() / "Mess" / "screen.glsl";
		skyboxShaderFilename = Paths::EngineContentDir() / "Mess" / "skybox.glsl";
		instanceShaderFilename = Paths::EngineContentDir() / "Mess" / "instancing.glsl";
		shadowmapShaderFilename = Paths::EngineContentDir() / "Mess" / "shadowmap.glsl";
		pointshadowmapShaderFilename = Paths::EngineContentDir() / "Mess" / "pointshadowmap.glsl";

		universalShaderFilename = Paths::EngineContentDir() / "Mess" / "Shaders" / "UniversalShader.glsl";

		boxDiffuseTextureFilename = Paths::EngineContentDir() / "Mess" / "T_BoxDiffuse.png";
		boxSpecularTextureFilename = Paths::EngineContentDir() / "Mess" / "T_BoxSpecular.png";
		backpackModelFilename = Paths::EngineContentDir() / "Mess" / "backpack" / "backpack.obj";
		coinMeshFilename = Paths::EngineContentDir() / "Mess" / "coin" / "Coin.obj";

		brickDiffuseFilename = Paths::EngineContentDir() / "Mess" / "brick" / "T_BrickDiffuse.jpg";
		brickNormalFilename = Paths::EngineContentDir() / "Mess" / "brick" / "T_BrickNormal.jpg";

		for (int i = 0; i < 6; i++)
			skyboxFilenames.push_back(Paths::EngineContentDir() / "Mess" / "skybox" / faces[i]);

		for (int i = 0; i < 6; i++)
			skyboxSFilenames.push_back(Paths::EngineContentDir() / "Mess" / "skybox2" / facesstylized[i]);

		if (IsRunningGame())
		{
			bHasGame = true;
		}
		else if (!cmdArgs.empty())
		{
			bHasGame = true;
		}

		Shader* lightSourceShader;
		Shader* litShader;
		Shader* borderShader;
		Shader* screenShader;
		Shader* skyboxShader;
		Shader* instanceShader;
		Shader* shadowmapShader;
		Shader* pointshadowmapShader;

		Shader* universalShader;

		Texture grayTexture;

		Texture boxDiffuse;
		Texture boxSpecular;
		CubeMap skybox;
		CubeMap skybox2;

		Texture brickDiffuse;
		Texture brickNormal;
		Texture defaultNormal;


		Model* backpackMesh;
		Model* coinMesh;

		SkyBoxCube* skyboxMesh;
		unsigned int uboMatrices;
		uint32 screenVAO, screenVBO;

		uint32 coinsCount = 3000;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[coinsCount];
		srand(glfwGetTime());
		float radius = 50.0;
		float offset = 15.0f;


		for (unsigned int i = 0; i < coinsCount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)coinsCount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y + 10.f, z));

			// 2. scale: scale between 0.05 and 0.25f
			float scale = (rand() % 40) / 100.0f + 0.4;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
		}
		// vertex buffer object
		unsigned int coinMatrixBuffer;






		if (bHasGame)
		{
			lightSourceShader = new Shader(lightSourceShaderFilename);
			litShader = new Shader(LitShaderFilename);
			borderShader = new Shader(borderShaderFilename);
			screenShader = new Shader(screenShaderFilename);
			skyboxShader = new Shader(skyboxShaderFilename);
			instanceShader = new Shader(instanceShaderFilename);
			shadowmapShader = new Shader(shadowmapShaderFilename);
			pointshadowmapShader = new Shader(pointshadowmapShaderFilename);

			//universalShader = new Shader(universalShaderFilename);

			m_CubeMesh = new CubeMesh();
			m_SphereMesh = new SphereMesh(8, 8);

			//backpackMesh = new Model(backpackModelFilename);
			coinMesh = new Model(coinMeshFilename);
			skyboxMesh = new SkyBoxCube();
			skyboxMesh->Finilize();

			grayTexture = TextureFromFile(grayTextureFilename, ETextureColorSpace::sRGB);
			boxDiffuse = TextureFromFile(boxDiffuseTextureFilename, ETextureColorSpace::sRGB);
			boxSpecular = TextureFromFile(boxSpecularTextureFilename, ETextureColorSpace::Linear);
			brickDiffuse = TextureFromFile(brickDiffuseFilename, ETextureColorSpace::sRGB);
			brickNormal = TextureFromFile(brickNormalFilename, ETextureColorSpace::Linear);
			uint8 normalData[3] = { 128, 128, 255 };
			defaultNormal = GenerateTexture(normalData, 1, 1);

			m_CubeMesh->Tdiffuse = boxDiffuse;
			m_CubeMesh->Tspecualr = boxSpecular;

			skybox = loadCubemap(skyboxFilenames);
			skybox2 = loadCubemap(skyboxSFilenames);

			FramebufferSpecs fbspecs;
			fbspecs.Width = 1280;
			fbspecs.Height = 720;
			fbspecs.Samples = glm::pow(2, TestRenderer::Settings.bAntiAliasingQuality);
			m_Framebuffer = new FrameBuffer(fbspecs);
			fbspecs.Samples = 1;
			m_IntermediateFramebuffer = new FrameBuffer(fbspecs);
			m_PostProcessFramebuffer = new FrameBuffer(fbspecs);



			m_Camera = MakeUnique<EditorCamera>();
			m_Camera->SetPerspective(45.f, 1280.f / 720.f, 0.2f, 200.f);
			m_Camera->SetPosition({ 0.f, 5.f, 0.f });


			// Post Processing Quad
			float quadVertices[] = {
				// positions   // texcoords
				-1.0f, -1.0f,  0.0f, 0.0f,  // bottom left
				 1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
				-1.0f,  1.0f,  0.0f, 1.0f,  // top left
				-1.0f,  1.0f,  0.0f, 1.0f,  // top left
				 1.0f, -1.0f,  1.0f, 0.0f,  // bottom right
				 1.0f,  1.0f,  1.0f, 1.0f   // top right
			};


			glGenVertexArrays(1, &screenVAO);
			glGenBuffers(1, &screenVBO);
			glBindVertexArray(screenVAO);
			glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

			glBindVertexArray(0);

			// Uniform Buffer
			/* Set uniform block equal to binding point 0*/
			unsigned int uniformBlockIndexLit = glGetUniformBlockIndex(litShader->GetId(), "Matrices");
			unsigned int uniformBlockIndexLightSource = glGetUniformBlockIndex(lightSourceShader->GetId(), "Matrices");
			unsigned int uniformBlockinstanced = glGetUniformBlockIndex(instanceShader->GetId(), "Matrices");
			glUniformBlockBinding(litShader->GetId(), uniformBlockIndexLit, 0);
			glUniformBlockBinding(lightSourceShader->GetId(), uniformBlockIndexLightSource, 0);
			glUniformBlockBinding(instanceShader->GetId(), uniformBlockinstanced, 0);

			/* Create UBO and allocate memory */

			glGenBuffers(1, &uboMatrices);
			glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			/* Link entire range of the buffer to binding point 0*/
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(glm::mat4));



			/* Coint instancing */
			glGenBuffers(1, &coinMatrixBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, coinMatrixBuffer);
			glBufferData(GL_ARRAY_BUFFER, coinsCount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);



			/* Directional light Shadows */
			glGenFramebuffers(1, &depthMapFBO);

			// Generate depth map

			glGenTextures(1, &depthMap);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			// Attach depth map to framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			/* Point light shadows */
			glGenFramebuffers(1, &depthCubeMapFBO);
			glGenTextures(1, &depthCubemap);


			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			for (unsigned int i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
					pSHADOW_WIDTH, pSHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Instancing
			for (unsigned int i = 0; i < coinMesh->GetMeshesCount(); i++)
			{
				unsigned int VAO = coinMesh->GetMesh(i)->GetVAO();
				glBindVertexArray(VAO);
				// vertex attributes
				std::size_t vec4Size = sizeof(glm::vec4);
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

				glVertexAttribDivisor(3, 1);
				glVertexAttribDivisor(4, 1);
				glVertexAttribDivisor(5, 1);
				glVertexAttribDivisor(6, 1);

				glBindVertexArray(0);
			}

		}
		String valuePrev;
		String value;
		while (m_Running)
		{
			Tick();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(m_DeltaTime);

			if (bHasGame)
			{


#ifndef VM_WITH_EDITOR
				int d_width;
				int d_height;
				GetFramebufferSize(Application::Get().GetWindow().GetNativeWindow(), d_width, d_height);
				m_Width = d_width;
				m_Height = d_height;
#endif
				if (FramebufferSpecs spec = m_Framebuffer->Specs;
					m_Width > 0.0f && m_Height > 0.0 &&
					(spec.Width != m_Width || spec.Height != m_Height))
				{
					m_Framebuffer->Resize((uint32)m_Width, (uint32)m_Height);
					m_IntermediateFramebuffer->Resize((uint32)m_Width, (uint32)m_Height);
					m_PostProcessFramebuffer->Resize((uint32)m_Width, (uint32)m_Height);
					m_Camera->SetViewport(m_Width, m_Height);
				}
				if (TestRenderer::Settings.bMainFramebufferDirty)
				{
					m_Framebuffer->Specs.Samples = glm::pow(2, TestRenderer::Settings.bAntiAliasingQuality);
					m_Framebuffer->Specs.bHDR = TestRenderer::Settings.bHDREnabled;
					m_IntermediateFramebuffer->Specs.bHDR = m_Framebuffer->Specs.bHDR;
					m_Framebuffer->Invalidate();
					m_IntermediateFramebuffer->Invalidate();
					TestRenderer::Settings.bMainFramebufferDirty = 0;
					TestRenderer::Settings.bPostProcessFramebufferDirty = 0;
				}

				m_Camera->Update(m_DeltaTime);

				glm::mat4 floorModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -2.f, 0.f)) * glm::scale(glm::mat4(1.0f), glm::vec3(20.f, 1.f, 20.f));

				glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.f))
					* glm::toMat4(glm::quat(glm::vec3(glm::radians(-cubePitch), glm::radians(-cubeYaw), glm::radians(-cubeRoll))));

				glm::mat4 sphereModel = glm::mat4(1.0f);

				glm::mat4 viewProjection = m_Camera->GetViewProjection();
				glm::mat4 view = glm::mat4(glm::mat3(m_Camera->GetViewMatrix()));

				glm::mat4 pointLightModel1 = glm::translate(glm::mat4(1.0f), m_PointLightPos1) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel2 = glm::translate(glm::mat4(1.0f), m_PointLightPos2) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel3 = glm::translate(glm::mat4(1.0f), m_PointLightPos3) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
				glm::mat4 pointLightModel4 = glm::translate(glm::mat4(1.0f), m_PointLightPos4) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

				glm::vec3 spotLigthDirection = glm::normalize(m_SpotLightDir);
				glm::quat spotLightRotation = glm::rotation(glm::vec3(0.0f, 0.0f, -1.0f), spotLigthDirection);

				glm::mat4 spotLightModel = glm::translate(glm::mat4(1.0f), m_SpotLightPos) * glm::toMat4(spotLightRotation) * glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.1f));
				glm::mat4 backpackModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, -5.f));
				glm::mat4 coinModel = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 4.0f, -5.f));

				//				glEnable(GL_DEPTH_TEST);
				// 				glEnable(GL_STENCIL_TEST);
				// 				glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
				// 				glStencilMask(0xFF);

				float explodeTime = -1.0;
				if (bExplode)
					explodeTime = Time::GetTime();


				glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewProjection));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				m_CubeMesh->Tdiffuse = boxDiffuse;
				m_CubeMesh->Tspecualr = boxSpecular;

				// 1. Render to depth map
				// Configure shader and matrices
				float near_plane = 1.0f, far_plane = 50.f;
				glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
				glm::vec3 dlpos = -glm::normalize(m_DirLightDirection) * 10.0f;
				glm::mat4 lightView = glm::lookAt(
					dlpos,
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, 1.0f, 0.0f)
				);
				glm::mat4 lightSpaceMatrix = lightProjection * lightView;

				if (bCastGlobalShadows)
				{

					shadowmapShader->Bind();
					shadowmapShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);

					glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
					glDrawBuffer(GL_NONE);
					glReadBuffer(GL_NONE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glCullFace(GL_FRONT);
					// Render Scene
					// Floor
					shadowmapShader->SetMat4("u_Transform", floorModel);
					TestRenderer::RenderMesh(m_CubeMesh, *shadowmapShader, 0, 0, true);
					// Cubes
					shadowmapShader->SetMat4("u_Transform", cubeModel);
					TestRenderer::RenderMesh(m_CubeMesh, *shadowmapShader, 0, 0, true);

					shadowmapShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(5, 3, 5)));
					TestRenderer::RenderMesh(m_CubeMesh, *shadowmapShader, 0, 0, true);

					shadowmapShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 8)));
					TestRenderer::RenderMesh(m_CubeMesh, *shadowmapShader, 0, 0, true);

					shadowmapShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 2)));
					TestRenderer::RenderMesh(m_CubeMesh, *shadowmapShader, 0, 0, true);

					// Backpack

					shadowmapShader->SetMat4("u_Transform", backpackModel);
#ifdef DRAW_BACKPACK_MODEL
					backpackMesh->Draw(*shadowmapShader, 0, 0, true);
#endif
					// Coin

					shadowmapShader->SetMat4("u_Transform", coinModel);
					coinMesh->Draw(*shadowmapShader, 0, 0, true);
					glCullFace(GL_BACK);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}
				if (bCastPointShadows)
				{
					// Point light shadow map creation
					glViewport(0, 0, pSHADOW_WIDTH, pSHADOW_HEIGHT);
					glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
					glClear(GL_DEPTH_BUFFER_BIT);



					float aspect = (float)pSHADOW_WIDTH / (float)pSHADOW_HEIGHT;
					float psmNear = 0.1f;
					float psmFar = 25.0f;
					glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, psmNear, psmFar);
					std::vector<glm::mat4> shadowTransforms;
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(m_PointLightPos1, m_PointLightPos1 + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(m_PointLightPos1, m_PointLightPos1 + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(m_PointLightPos1, m_PointLightPos1 + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(m_PointLightPos1, m_PointLightPos1 + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(m_PointLightPos1, m_PointLightPos1 + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
					shadowTransforms.push_back(shadowProj *
						glm::lookAt(m_PointLightPos1, m_PointLightPos1 + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

					litShader->Bind();
					litShader->SetFloat("u_PointLights[0].farPlane", psmFar);

					pointshadowmapShader->Bind();
					for (int i = 0; i < 6; i++)
						pointshadowmapShader->SetMat4(String("u_ShadowMatrices[") + std::to_string(i) + "]", shadowTransforms[i]);

					pointshadowmapShader->SetFloat3("u_LightPos", m_PointLightPos1);
					pointshadowmapShader->SetFloat("u_FarPlane", psmFar);

					// Floor
					pointshadowmapShader->SetMat4("u_Transform", floorModel);
					TestRenderer::RenderMesh(m_CubeMesh, *pointshadowmapShader, 0, 0, true);
					// Cubes
					pointshadowmapShader->SetMat4("u_Transform", cubeModel);
					TestRenderer::RenderMesh(m_CubeMesh, *pointshadowmapShader, 0, 0, true);

					pointshadowmapShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(5, 3, 5)));
					TestRenderer::RenderMesh(m_CubeMesh, *pointshadowmapShader, 0, 0, true);

					pointshadowmapShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 8)));
					TestRenderer::RenderMesh(m_CubeMesh, *pointshadowmapShader, 0, 0, true);

					pointshadowmapShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 2)));
					TestRenderer::RenderMesh(m_CubeMesh, *pointshadowmapShader, 0, 0, true);

					// Backpack

					pointshadowmapShader->SetMat4("u_Transform", backpackModel);
#ifdef DRAW_BACKPACK_MODEL
					backpackMesh->Draw(*pointshadowmapShader, 0, 0, true);
#endif
					// Coin

					pointshadowmapShader->SetMat4("u_Transform", coinModel);
					coinMesh->Draw(*pointshadowmapShader, 0, 0, true);

					// Render Scene
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

				}
				// 2. Use depth map to render with shadow mapping
				m_Framebuffer->Bind();
				TestRenderer::SetClearColor(0.2f, 0.3f, 0.3f, 1.f);
				TestRenderer::Clear();

				litShader->Bind();
				litShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
				litShader->SetFloat("u_Time", explodeTime);
				litShader->SetBool("u_UseDepth", bDrawDepthBuffer);
				litShader->SetFloat("u_NearZ", m_Camera->GetNearClip());
				litShader->SetFloat("u_FarZ", m_Camera->GetFarClip());

				litShader->SetFloat3("u_ViewPos", m_Camera->GetPosition());

				litShader->SetUniformInt("u_Material.diffuse", 0);
				litShader->SetUniformInt("u_Material.specular", 1);
				litShader->SetUniformInt("u_Material.normal", 2);
				litShader->SetFloat("u_Material.shininess", m_Shininess);

				litShader->SetFloat3("u_DirLight.direction", m_DirLightDirection);
				litShader->SetFloat3("u_DirLight.ambient", m_DirLightAmbient);
				litShader->SetFloat3("u_DirLight.diffuse", m_DirLightDiffuse);
				litShader->SetFloat3("u_DirLight.specular", m_DirLightSpecular);
				litShader->SetBool("u_DirLight.castShadows", bCastGlobalShadows);

				litShader->SetFloat3("u_PointLights[0].position", m_PointLightPos1);
				litShader->SetFloat3("u_PointLights[1].position", m_PointLightPos2);
				litShader->SetFloat3("u_PointLights[2].position", m_PointLightPos3);
				litShader->SetFloat3("u_PointLights[3].position", m_PointLightPos4);
				litShader->SetFloat("u_PointLights[0].constant", 1.0f);
				litShader->SetFloat("u_PointLights[1].constant", 1.0f);
				litShader->SetFloat("u_PointLights[2].constant", 1.0f);
				litShader->SetFloat("u_PointLights[3].constant", 1.0f);
				litShader->SetFloat("u_PointLights[0].linear", 0.09f);
				litShader->SetFloat("u_PointLights[1].linear", 0.09f);
				litShader->SetFloat("u_PointLights[2].linear", 0.09f);
				litShader->SetFloat("u_PointLights[3].linear", 0.09f);
				litShader->SetFloat("u_PointLights[0].quadratic", 0.032f);
				litShader->SetFloat("u_PointLights[1].quadratic", 0.032f);
				litShader->SetFloat("u_PointLights[2].quadratic", 0.032f);
				litShader->SetFloat("u_PointLights[3].quadratic", 0.032f);
				litShader->SetFloat3("u_PointLights[0].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[1].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[2].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[3].ambient", m_PointLightAmbient);
				litShader->SetFloat3("u_PointLights[0].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[1].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[2].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[3].diffuse", m_PointLightDiffuse);
				litShader->SetFloat3("u_PointLights[0].specular", m_PointLightSpecular);
				litShader->SetFloat3("u_PointLights[1].specular", m_PointLightSpecular);
				litShader->SetFloat3("u_PointLights[2].specular", m_PointLightSpecular);
				litShader->SetFloat3("u_PointLights[3].specular", m_PointLightSpecular);
				litShader->SetBool("u_PointLights[0].castShadows", bCastPointShadows);

				litShader->SetFloat3("u_SpotLights[0].position", m_SpotLightPos);
				litShader->SetFloat3("u_SpotLights[0].direction", m_SpotLightDir);
				litShader->SetFloat("u_SpotLights[0].innerCutOff", glm::cos(glm::radians(m_SpotLightInner)));
				litShader->SetFloat("u_SpotLights[0].outerCutOff", glm::cos(glm::radians(m_SpotLightOuter)));
				litShader->SetFloat("u_SpotLights[0].constant", 1.0f);
				litShader->SetFloat("u_SpotLights[0].linear", 0.09f);
				litShader->SetFloat("u_SpotLights[0].quadratic", 0.032f);

				litShader->SetFloat3("u_SpotLights[0].ambient", m_SpotLightAmbient);
				litShader->SetFloat3("u_SpotLights[0].diffuse", m_SpotLightDiffuse);
				litShader->SetFloat3("u_SpotLights[0].specular", m_SpotLightSpecular);

				instanceShader->Bind();
				instanceShader->SetFloat("u_Time", explodeTime);
				instanceShader->SetBool("u_UseDepth", bDrawDepthBuffer);

				if (bUseInstancing)
				{
					instanceShader->SetFloat3("u_ViewPos", m_Camera->GetPosition());

					instanceShader->SetUniformInt("u_Material.diffuse", 0);
					instanceShader->SetUniformInt("u_Material.specular", 1);
					instanceShader->SetFloat("u_Material.shininess", m_Shininess);

					instanceShader->SetFloat3("u_DirLight.direction", m_DirLightDirection);
					instanceShader->SetFloat3("u_DirLight.ambient", m_DirLightAmbient);
					instanceShader->SetFloat3("u_DirLight.diffuse", m_DirLightDiffuse);
					instanceShader->SetFloat3("u_DirLight.specular", m_DirLightSpecular);

					instanceShader->SetFloat3("u_PointLights[0].position", m_PointLightPos1);
					instanceShader->SetFloat3("u_PointLights[1].position", m_PointLightPos2);
					instanceShader->SetFloat3("u_PointLights[2].position", m_PointLightPos3);
					instanceShader->SetFloat3("u_PointLights[3].position", m_PointLightPos4);
					instanceShader->SetFloat("u_PointLights[0].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[1].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[2].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[3].constant", 1.0f);
					instanceShader->SetFloat("u_PointLights[0].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[1].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[2].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[3].linear", 0.09f);
					instanceShader->SetFloat("u_PointLights[0].quadratic", 0.032f);
					instanceShader->SetFloat("u_PointLights[1].quadratic", 0.032f);
					instanceShader->SetFloat("u_PointLights[2].quadratic", 0.032f);
					instanceShader->SetFloat("u_PointLights[3].quadratic", 0.032f);
					instanceShader->SetFloat3("u_PointLights[0].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[1].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[2].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[3].ambient", m_PointLightAmbient);
					instanceShader->SetFloat3("u_PointLights[0].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[1].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[2].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[3].diffuse", m_PointLightDiffuse);
					instanceShader->SetFloat3("u_PointLights[0].specular", m_PointLightSpecular);
					instanceShader->SetFloat3("u_PointLights[1].specular", m_PointLightSpecular);
					instanceShader->SetFloat3("u_PointLights[2].specular", m_PointLightSpecular);
					instanceShader->SetFloat3("u_PointLights[3].specular", m_PointLightSpecular);

					instanceShader->SetFloat3("u_SpotLights[0].position", m_SpotLightPos);
					instanceShader->SetFloat3("u_SpotLights[0].direction", m_SpotLightDir);
					instanceShader->SetFloat("u_SpotLights[0].innerCutOff", glm::cos(glm::radians(m_SpotLightInner)));
					instanceShader->SetFloat("u_SpotLights[0].outerCutOff", glm::cos(glm::radians(m_SpotLightOuter)));
					instanceShader->SetFloat("u_SpotLights[0].constant", 1.0f);
					instanceShader->SetFloat("u_SpotLights[0].linear", 0.09f);
					instanceShader->SetFloat("u_SpotLights[0].quadratic", 0.032f);

					instanceShader->SetFloat3("u_SpotLights[0].ambient", m_SpotLightAmbient);
					instanceShader->SetFloat3("u_SpotLights[0].diffuse", m_SpotLightDiffuse);
					instanceShader->SetFloat3("u_SpotLights[0].specular", m_SpotLightSpecular);



					instanceShader->Bind();
					for (unsigned int i = 0; i < coinMesh->GetMeshesCount(); i++)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, coinMesh->GetMesh(i)->Tdiffuse.Id);


						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, coinMesh->GetMesh(i)->Tspecualr.Id);
						glBindVertexArray(coinMesh->GetMesh(i)->GetVAO());
						glDrawElementsInstanced(
							GL_TRIANGLES, coinMesh->GetMesh(i)->Indices.size(), GL_UNSIGNED_INT, 0, coinsCount
						);
						glBindVertexArray(0);
					}
				}

				// Floor
				m_CubeMesh->Tdiffuse = grayTexture;
				m_CubeMesh->Tspecualr = boxSpecular;
				m_CubeMesh->Tnormal = defaultNormal;
				litShader->Bind();
				litShader->SetMat4("u_Transform", floorModel);
				litShader->SetFloat("u_UseNormalMap", (float)bUseNormalMaps);
				TestRenderer::RenderMesh(m_CubeMesh, *litShader, depthMap, depthCubemap);

				m_CubeMesh->Tdiffuse = boxDiffuse;
				m_CubeMesh->Tspecualr = boxSpecular;
				m_CubeMesh->Tnormal = defaultNormal;

				litShader->SetMat4("u_Transform", cubeModel);
				litShader->SetFloat("u_UseNormalMap", 0.0f);
				TestRenderer::RenderMesh(m_CubeMesh, *litShader, depthMap, depthCubemap);

				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(5, 3, 5)));
				litShader->SetFloat("u_UseNormalMap", 0.0f);
				TestRenderer::RenderMesh(m_CubeMesh, *litShader, depthMap, depthCubemap);

				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 8)));
				litShader->SetFloat("u_UseNormalMap", 0.0f);
				TestRenderer::RenderMesh(m_CubeMesh, *litShader, depthMap, depthCubemap);

				m_CubeMesh->Tdiffuse = brickDiffuse;
				m_CubeMesh->Tnormal = brickNormal;
				litShader->SetMat4("u_Transform", cubeModel * glm::translate(glm::mat4(1.0f), glm::vec3(-3, 0, 2)));
				litShader->SetFloat("u_UseNormalMap", (float)bUseNormalMaps);
				TestRenderer::RenderMesh(m_CubeMesh, *litShader, depthMap, depthCubemap);
				m_CubeMesh->Tnormal = defaultNormal;


				// Backpack
				litShader->Bind();
				litShader->SetMat4("u_Transform", backpackModel);
				litShader->SetFloat("u_UseNormalMap", (float)bUseNormalMaps);
#ifdef DRAW_BACKPACK_MODEL
				backpackMesh->Draw(*litShader, depthMap, depthCubemap);
#endif
				// Coin
				coinMesh->GetMesh(0)->Tnormal = defaultNormal;
				litShader->Bind();
				litShader->SetMat4("u_Transform", coinModel);
				litShader->SetFloat("u_UseNormalMap", 0.0f);
				coinMesh->Draw(*litShader, depthMap, depthCubemap);

				lightSourceShader->Bind();
				lightSourceShader->SetFloat3("u_LightColor", m_PointLightDiffuse);


				lightSourceShader->SetMat4("u_Transform", pointLightModel1);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetMat4("u_Transform", pointLightModel2);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetMat4("u_Transform", pointLightModel3);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetMat4("u_Transform", pointLightModel4);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);

				lightSourceShader->SetFloat3("u_LightColor", m_SpotLightDiffuse);
				lightSourceShader->SetMat4("u_Transform", spotLightModel);
				TestRenderer::RenderMesh(m_SphereMesh, *lightSourceShader);


				if (!bUseInstancing)
				{
					for (uint32 i = 0; i < coinsCount; i++)
					{
						litShader->SetMat4("u_Transform", modelMatrices[i]);
						coinMesh->Draw(*litShader);
					}

				}

				/*				glStencilMask(0x00);*/
				uint32 skyboxId;
				if (bUseNatureSkybox)
					skyboxId = skybox.Id;
				else
					skyboxId = skybox2.Id;

				skyboxShader->Bind();
				skyboxShader->SetMat4("u_ViewProjection", m_Camera->GetProjectionMatrix() * view);
				skyboxShader->SetUniformInt("u_SkyBox", 0);
				glDisable(GL_CULL_FACE);
				glDepthFunc(GL_LEQUAL);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxId);
				glBindVertexArray(skyboxMesh->VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS);
				glEnable(GL_CULL_FACE);


				m_Framebuffer->UnBind();
				TestRenderer::BlitFramebufferTo(*m_Framebuffer, *m_IntermediateFramebuffer);



				m_PostProcessFramebuffer->Bind();
				TestRenderer::BlitStencil(*m_IntermediateFramebuffer, *m_PostProcessFramebuffer);

				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				screenShader->Bind();
				TestRenderer::UpdatePostprocessShader(*screenShader);
				glBindVertexArray(screenVAO);
				glDisable(GL_DEPTH_TEST);
				glBindTexture(GL_TEXTURE_2D, m_IntermediateFramebuffer->GetTexture());
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glEnable(GL_DEPTH_TEST);
				m_PostProcessFramebuffer->UnBind();

			}

#ifdef VM_WITH_EDITOR
			RenderGUI();
#else
			if (bHasGame)
			{
				TestRenderer::BlitFramebufferToSwapchain(*m_PostProcessFramebuffer);


			}
#endif

			m_Window->SwapBuffers();
		}
	}



	void Application::Tick()
	{
		m_Window->PollEvents();

		double time = Time::GetTime();
		m_DeltaTime = time - m_LastFrameTime;
		m_LastFrameTime = time;

		ProcessInputEvents();
#ifdef VM_WITH_EDITOR
		process_movement();
#endif
		// Delta time
		g_Engine->ResetRunGCFlag();
		GarbageCollector::Get().CollectGarbage(false);
		// 
		// RenderScene - startframe | for scene in Renderer::Get().GetScenes
		CalculateTime();

		FinishInput();

		// update layers
		g_Engine->Tick(m_DeltaTime);
		// gui update
		// 
		// RenderScene - EndFrame | for scene in Renderer::Get().GetScenes
		// 
		// swap buffers
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushLayer(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	CORE_API float g_AvgFPS = 0.0f;
	CORE_API float g_AvgMS = 0.0f;

	CORE_API void CalculateTime()
	{
		static double lastTime = 0.0;
		double currentTime = Time::GetTime();
		float frameTime = (float)((currentTime - lastTime) * 1000.0);

		g_AvgMS = g_AvgMS * 0.75f + frameTime * 0.25f;
		lastTime = currentTime;
		g_AvgFPS = 1000.f / g_AvgMS;
	}

}

#if IS_UNIFIED

#include <HAL/PlatformService.h>

namespace VeiM
{
	class GameApplication : public Application
	{
	public:
		GameApplication(const ApplicationSpecification& appSpecs)
			: Application(appSpecs)
		{

		}

		~GameApplication()
		{

		}
	};

	Application* CreateApplication(const std::vector<String>& arguments, const VeiM::String& name, const VeiM::String& title)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = name;
		specs.AppConfig.EnableConsole = true;
		specs.AppConfig.CommandLineArgs = arguments;

		specs.WndConfig.Title = title;
		specs.WndConfig.Width = 1280;
		specs.WndConfig.Height = 720;
		specs.WndConfig.VSync = false;
		specs.WndConfig.CustomTitlebar = false;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.Mode = EWindowMode::Fullscreen;
		// TODO: Make default icon


		Application* app = new GameApplication(specs);
		return app;
	}
}

#endif